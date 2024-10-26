#include "view/main_view.hpp"

#include "view/icon_part_select.hpp"
#include "view/icon_part_select_grid.hpp"
#include "view/collection_grid.hpp"
#include "view/download_view.hpp"
#include "view/empty_message.hpp"
#include "util/uuid.hpp"
#include "util/paths.hpp"
#include "util/download.hpp"

#define __SWITCH__
#include "extern/json.hpp"

#include <filesystem>
#include <ranges>
#include <expected>

using namespace brls::literals;
namespace fs = std::filesystem;

std::expected<std::vector<CategoryPart>, std::string> getCategories(std::string_view subcategory)
{
  try {
    std::vector<CategoryPart> res;
    auto categories = std::ranges::subrange(fs::directory_iterator(paths::IconCachePath), fs::directory_iterator{}) |
      std::views::filter([](const fs::directory_entry &entry) { return entry.is_directory(); }) |
      std::views::transform([](const fs::directory_entry &entry) { return entry.path(); }) |
      std::ranges::to<std::vector<fs::path>>();

    for (auto &category : categories) {
      auto path = category / subcategory;

      if (fs::exists(path) && !fs::is_empty(path)) {
        fs::path iconPath;
        for (auto &file : fs::directory_iterator(category / "characters")) {
          iconPath = file.path();
          break;
        }

        if (!iconPath.empty()) {
          brls::Logger::debug("category {}, image {}", category.string(), iconPath.string());
          res.push_back(CategoryPart{category.filename(), iconPath.string()});
        }
      }
    }

    std::sort(res.begin(), res.end(), [](CategoryPart a, CategoryPart b){
      return a.name < b.name;
    });

    if (res.empty()) {
      return std::unexpected("No categories found");
    } else {
      // this is slow/stupid but it doesn't matter
      res.insert(res.begin(), CategoryPart{"custom", ""});
      res.insert(res.begin(), CategoryPart{"none", ""});
      return res;
    }
  } catch (const std::exception &e) {
    brls::Logger::error("Error getting categories: {}", e.what());
    return std::unexpected(e.what());
  }
}

std::expected<std::vector<std::string>, std::string> getImages(std::string_view path)
{
  try {
    auto res = std::ranges::subrange(fs::directory_iterator(path), fs::directory_iterator{}) |
      std::views::filter([](const fs::directory_entry &entry) { return entry.is_regular_file() && (entry.path().extension() == ".png" || entry.path().extension() == ".jpg" || entry.path().extension() == ".jpeg"); }) |
      std::views::transform([](const fs::directory_entry &entry) { return entry.path().string(); }) |
      std::ranges::to<std::vector<std::string>>();
    if (res.empty()) {
      return std::unexpected("No images found");
    } else {
      return res;
    }
  } catch (const std::exception &e) {
    brls::Logger::error("Error getting images: {}", e.what());
    return std::unexpected(e.what());
  }
}

MainView::MainView()
{
  // Inflate the tab from the XML file
  this->inflateFromXMLRes("xml/views/main_view.xml");

  btnChangeUser->registerClickAction([this](brls::View*)
                                     {
      handleUserSelection();
      return true; });

  btnFrame->registerClickAction([this](brls::View*)
                                {
      if (auto files = getCategories("frames"); files.has_value()) {
        for (auto& file : files.value()) {
          brls::Logger::debug("{}, {}", file.name, file.icon);
        }

        this->present(static_cast<brls::View*>(new IconPartSelect(files.value(), "frames", imageState, [this](std::string path) {
          brls::Logger::info("Recieved {} from selection.", path);
          imageState.updateFrame(path);
          image->setImageFromMemRGBA(imageState.working.data.get(), imageState.working.x, imageState.working.y);
        }, [](std::string path, ImageState& state){
          state.updateFrame(path);
        })));
        
      } else {
        this->present(new EmptyMessage("app/errors/nothing_icon_cache"_i18n));
      }
      return true; });

  btnCharacter->registerClickAction([this](brls::View*)
                                    {
      if (auto files = getCategories("characters"); files.has_value()) {
        for (auto& file : files.value()) {
          brls::Logger::debug("{}, {}", file.name, file.icon);
        }

        this->present(static_cast<brls::View*>(new IconPartSelect(files.value(), "characters", imageState, [this](std::string path) {
          brls::Logger::info("Recieved {} from selection.", path);
          imageState.updateCharacter(path);
          image->setImageFromMemRGBA(imageState.working.data.get(), imageState.working.x, imageState.working.y);
        }, [](std::string path, ImageState& state){
          state.updateCharacter(path);
        })));
        
      } else {
        this->present(new EmptyMessage("app/errors/nothing_icon_cache"_i18n));
      }
      return true; });

  btnBackground->registerClickAction([this](brls::View*)
                                     {
      if (auto files = getCategories("backgrounds"); files.has_value()) {
        for (auto& file : files.value()) {
          brls::Logger::debug("{}, {}", file.name, file.icon);
        }

        this->present(static_cast<brls::View*>(new IconPartSelect(files.value(), "backgrounds", imageState, [this](std::string path) {
          brls::Logger::info("Recieved {} from selection.", path);
          imageState.updateBackground(path);
          image->setImageFromMemRGBA(imageState.working.data.get(), imageState.working.x, imageState.working.y);
        }, [](std::string path, ImageState& state){
          state.updateBackground(path);
        })));
        
      } else {
        this->present(new EmptyMessage("app/errors/nothing_icon_cache"_i18n));
      }
      return true; });

  btnSave->registerClickAction([this](brls::View*)
                               {
      auto res = account::setUserIcon(user, imageState.working);
      brls::Logger::info("Icon set for user {}: {}", user.base.nickname, res);
      if (res) {
        currentImage->setImageFromMemRGBA(imageState.working.data.get(), imageState.working.x, imageState.working.y);

        // save to collection; hash beforehand to avoid duplicate copies
        auto path = fs::path(paths::CollectionPath) / (imageState.working.hash() + ".png");
        if (!fs::exists(path)) {
          res = imageState.working.writePng(path);
          brls::Logger::info("Writing to previous icons cache {}: {}", path.string(), res ? "success" : "failed");
        }
      }
      return true; });

  btnCustom->registerClickAction([this](brls::View*)
                                 {
        tempState = imageState;
        if (auto files = getImages(paths::BasePath); files.has_value()) {
          for (auto& file : files.value()) {
            brls::Logger::debug("{}", file);
          }
          
          this->present(static_cast<brls::View*>(new grid::IconPartSelectGrid(files.value(), "app/main/available_images"_i18n, tempState, [this](std::string path) {
              brls::Logger::info("Recieved {} from selection.", path);
              imageState.updateWorking(path);
              image->setImageFromMemRGBA(imageState.working.data.get(), imageState.working.x, imageState.working.y);
            }, [](std::string path, ImageState &state){
              state.updateWorking(path);
            })));
        } else {
          this->present(new EmptyMessage(fmt::format(fmt::runtime("app/errors/nothing_images"_i18n), paths::BasePath)));
        }


        return true; });

  btnCollectionLoad->registerClickAction([this](brls::View*)
                                 {
        tempState = imageState;
        if (auto files = getImages(paths::CollectionPath); files.has_value()) {
          for (auto& file : files.value()) {
            brls::Logger::debug("{}", file);
          }

          this->present(static_cast<brls::View*>(new collection::CollectionGrid(files.value(), "app/main/available_images"_i18n, tempState, [this](std::string path) {
              brls::Logger::info("Recieved {} from selection.", path);
              imageState.updateWorking(path);
              image->setImageFromMemRGBA(imageState.working.data.get(), imageState.working.x, imageState.working.y);
            }, [](std::string path, ImageState &state){
              state.updateWorking(path);
            })));
          
        } else {
          this->present(new EmptyMessage(fmt::format(fmt::runtime("app/errors/nothing"_i18n), paths::BasePath)));
        }


        return true; });

  btnSettings->registerClickAction([this](brls::View*)
                                   {
      this->present(new SettingsView(settings));
      return true; });

  image->allowCaching = false;
  currentImage->allowCaching = false;

  brls::Logger::info("Pre-init");
  account::init();
  download::init();
  brls::Logger::info("Post-init");

  brls::sync([]()
             { brls::Logger::info("{} the debug layer", true ? "Open" : "Close"); });

  handleUserSelection();

}

void MainView::handleUserSelection()
{
  account::selectUser(user);
  brls::Logger::info("User is {}", account::isValid(user) ? "valid" : "invalid");

  if (account::isValid(user))
  {
    auto image = account::getProfileImage(user);
    brls::Logger::info("Loaded User is {}", user.base.nickname);
    currentUser->setText(user.base.nickname);
    currentImage->setImageFromMemRGBA(image.data.get(), image.x, image.y);
  }
}

brls::View *MainView::create()
{
  // Called by the XML engine to create a new MainView
  return new MainView();
}
