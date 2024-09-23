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
#include "GenericToolbox.Switch.h"
#include "extern/json.hpp"

using namespace brls::literals;

std::vector<CategoryPart> getCategories(std::string subcategory)
{
  std::vector<CategoryPart> res;

  auto categories = GenericToolbox::lsDirs(paths::IconCachePath);
  if (categories.size() > 0)
  {
    GenericToolbox::removeEntryIf(categories, [](const std::string &entry)
                                  { return GenericToolbox::startsWith(entry, "."); });
    for (auto &category : categories)
    {
      auto path = GenericToolbox::joinPath(paths::IconCachePath, category, subcategory);
      auto files = GenericToolbox::lsFiles(path);
      GenericToolbox::removeEntryIf(files, [](const std::string &entry)
                                    { return GenericToolbox::startsWith(entry, "."); });

      if (files.size() >= 1)
      {
        path = GenericToolbox::joinPath(paths::IconCachePath, category, "characters");
        files = GenericToolbox::lsFiles(path);
        GenericToolbox::removeEntryIf(files, [](const std::string &entry)
                                      { return !GenericToolbox::endsWith(entry, ".png"); });

        if (files.size() >= 1)
        {
          brls::Logger::debug("category {}", category);
          CategoryPart part{category, GenericToolbox::joinPath(path, files[0])};
          res.push_back(part);
        }
      }
    }
  }

  std::sort(res.begin(), res.end(), [](CategoryPart a, CategoryPart b){
    return a.name < b.name;
  });
  res.insert(res.begin(), CategoryPart{"none", ""});

  if (res.size() == 1)
  {
    res.clear();
  }

  return res;
}

std::vector<std::string> getImages(std::string path)
{
  std::vector<std::string> res;

  auto images = GenericToolbox::lsFiles(path);
  GenericToolbox::removeEntryIf(images, [](const std::string &entry)
                                { return !(GenericToolbox::endsWith(entry, ".png") ||
                                           GenericToolbox::endsWith(entry, ".jpg") ||
                                           GenericToolbox::endsWith(entry, ".jpeg")); });

  for (auto &image : images)
  {
    brls::Logger::debug("img {}", image);
    res.push_back(GenericToolbox::joinPath(path, image));
  }

  return res;
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
      auto files = getCategories("frames");
      for (auto file : files) {
        brls::Logger::debug("{}, {}", file.name, file.icon);
      }
      this->present(
        files.size() ? (brls::View*)new IconPartSelect(files, "frames", imageState, [this](std::string path) {
          brls::Logger::info("Recieved {} from selection.", path);
          imageState.updateFrame(path);
          image->setImageFromMemRGBA(imageState.working.img.get(), imageState.working.x, imageState.working.y);
        }, [](std::string path, ImageState& state){
          state.updateFrame(path);
        }) : new EmptyMessage("app/errors/nothing_icon_cache"_i18n)
      );

      return true; });

  btnCharacter->registerClickAction([this](brls::View*)
                                    {
      auto files = getCategories("characters");
      for (auto file : files) {
        brls::Logger::debug("{}, {}", file.name, file.icon);
      }

      this->present(
        files.size() ? static_cast<brls::View*>(new IconPartSelect(files, "characters", imageState, [this](std::string path) {
          brls::Logger::info("Recieved {} from selection.", path);
          imageState.updateCharacter(path);
          image->setImageFromMemRGBA(imageState.working.img.get(), imageState.working.x, imageState.working.y);
        }, [](std::string path, ImageState& state){
          state.updateCharacter(path);
        })) : new EmptyMessage("app/errors/nothing_icon_cache"_i18n)
      );

      return true; });

  btnBackground->registerClickAction([this](brls::View*)
                                     {
      auto files = getCategories("backgrounds");
      for (auto file : files) {
        brls::Logger::debug("{}, {}", file.name, file.icon);
      }

      this->present(
        files.size() ? static_cast<brls::View*>(new IconPartSelect(files, "backgrounds", imageState, [this](std::string path) {
          brls::Logger::info("Recieved {} from selection.", path);
          imageState.updateBackground(path);
          image->setImageFromMemRGBA(imageState.working.img.get(), imageState.working.x, imageState.working.y);
        }, [](std::string path, ImageState& state){
          state.updateBackground(path);
        })) : new EmptyMessage("app/errors/nothing_icon_cache"_i18n)
      );

      return true; });

  btnSave->registerClickAction([this](brls::View*)
                               {
      auto res = account::setUserIcon(user, imageState.working);
      brls::Logger::info("Icon set for user {}: []", user.base.nickname, res);
      if (res) {
        currentImage->setImageFromMemRGBA(imageState.working.img.get(), imageState.working.x, imageState.working.y);

        // save to collection; hash beforehand to avoid duplicate copies
        auto path = GenericToolbox::joinPath(paths::CollectionPath, imageState.working.hash() + ".png");
        if (!GenericToolbox::isFile(path)) { imageState.working.writePng(path); }
      }
      return true; });

  btnCustom->registerClickAction([this](brls::View*)
                                 {
        tempState = imageState;
        auto files = getImages(paths::BasePath);
        for (auto file : files) {
          brls::Logger::debug("{}", file);
        }

        this->present(
          files.size() ? static_cast<brls::View*>(new grid::IconPartSelectGrid(files, "app/main/available_images"_i18n, tempState, [this](std::string path) {
            brls::Logger::info("Recieved {} from selection.", path);
            imageState.updateWorking(path);
            image->setImageFromMemRGBA(imageState.working.img.get(), imageState.working.x, imageState.working.y);
          }, [](std::string path, ImageState &state){
            state.updateWorking(path);
          })) : new EmptyMessage(fmt::format(fmt::runtime("app/errors/nothing_images"_i18n), paths::BasePath))
        );

        return true; });

  btnCollectionLoad->registerClickAction([this](brls::View*)
                                 {
        tempState = imageState;
        auto files = getImages(paths::CollectionPath);
        for (auto file : files) {
          brls::Logger::debug("{}", file);
        }

        this->present(
          files.size() ? static_cast<brls::View*>(new collection::CollectionGrid(files, "app/main/available_images"_i18n, tempState, [this](std::string path) {
            brls::Logger::info("Recieved {} from selection.", path);
            imageState.updateWorking(path);
            image->setImageFromMemRGBA(imageState.working.img.get(), imageState.working.x, imageState.working.y);
          }, [](std::string path, ImageState &state){
            state.updateWorking(path);
          })) : new EmptyMessage(fmt::format(fmt::runtime("app/errors/nothing"_i18n), paths::BasePath))
        );

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
    currentImage->setImageFromMemRGBA(image.img.get(), image.x, image.y);
  }
}

brls::View *MainView::create()
{
  // Called by the XML engine to create a new MainView
  return new MainView();
}
