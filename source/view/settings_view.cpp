#include "view/settings_view.hpp"

#include "view/download_view.hpp"
#include "view/about_view.hpp"

#include "extern/json.hpp"
#include "util/download.hpp"
#include <filesystem>
#include <fstream>
#include "util/paths.hpp"

using json = nlohmann::json;

const std::string ApiPath = "http://api.github.com/repos/henry-debruin/nso-icons/branches/main";
const std::string DownloadPath = "https://github.com/henry-debruin/nso-icons/archive/refs/heads/main.zip";

const std::string TempPath = std::string(paths::BasePath) + "icon_archive_temp.zip";

using namespace brls::literals; // for _i18n
namespace fs = std::filesystem;

void SettingsView::updateUI()
{
  cacheText->setText(fs::is_directory(paths::IconCachePath) ? "app/settings/icon_cache/yes"_i18n : "app/settings/icon_cache/no"_i18n);
  checkText->setText(fmt::format(fmt::runtime("app/settings/icon_cache/last_checked"_i18n), cacheData.count("checkTime") ? cacheData["checkTime"] : "app/settings/icon_cache/never"_i18n));
  if (updateState == UpdateState::CHECK)
  {
    updateText->setText(fmt::format(fmt::runtime("app/settings/icon_cache/current_version"_i18n),
                                    cacheData.count("updateDate") ? cacheData["updateDate"] : "app/settings/icon_cache/none"_i18n,
                                    cacheData.count("updateMessage") ? cacheData["updateMessage"] : "app/settings/icon_cache/none"_i18n));
    updateButton->setText("app/settings/icon_cache/check_updates"_i18n);
  }
  else if (updateState == UpdateState::UPDATE)
  {
    updateText->setText(fmt::format(fmt::runtime("app/settings/icon_cache/update_available"_i18n), data["updateDate"], data["updateMessage"]));
    updateButton->setText("app/settings/icon_cache/download_update"_i18n);
  }
}

SettingsView::SettingsView(SettingsData &settings) : settings(settings)
{
  // Inflate the tab from the XML file
  this->inflateFromXMLRes("xml/views/settings.xml");

  debug->init("app/settings/toggles/debug"_i18n, brls::Application::isDebuggingViewEnabled(), [](bool value)
              {
        brls::Application::enableDebuggingView(value);
        brls::sync([value](){
            brls::Logger::info("{} the debug layer", value ? "Open" : "Close");
        }); });

  extract_overwrite->init("app/settings/toggles/overwrite"_i18n, settings.overwriteDuringExtract, [&settings](bool value)
                          { settings.overwriteDuringExtract = value;
                            brls::sync([value]()
                                       {
                                        brls::Logger::info("extract check? {}", value ? "Overwrite" : "No Overwrite"); 
                                         }); });

  about->registerClickAction([this](...)
                          { 
                            this->present(new AboutView()); 
                            return true; });

  updateButton->registerClickAction([this](...)
                                    {
      if (updateState == UpdateState::CHECK) {
        nlohmann::ordered_json jsondata = {};

        data["checkTime"] = fmt::format("{:%FT%TZ}", fmt::gmtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
        cacheData["checkTime"] = data["checkTime"];

        {
          std::fstream stream(fs::path(paths::CacheFilePath), std::ios::out);
          json jdata = cacheData;
          stream << jdata;

          brls::Logger::info("Cache file saved {}", paths::CacheFilePath);
        }

        auto res = download::getRequest(ApiPath, jsondata, {
          "Accept: application/vnd.github+json",
          "X-GitHub-Api-Version: 2022-11-28"
        });

        if (jsondata.contains("commit")) {
          data["updateSha"] = jsondata["commit"]["sha"];
          data["updateDate"] = jsondata["commit"]["commit"]["author"]["date"];
          data["updateMessage"] = jsondata["commit"]["commit"]["message"];

          brls::Logger::info("Update check: sha {}, date {}", data["updateSha"], data["updateDate"]);

          if (!cacheData.count("updateSha") || !fs::is_directory(paths::IconCachePath) || fs::is_empty(paths::IconCachePath) || data["updateSha"] != cacheData["updateSha"]) {
            brls::Logger::info("Update available: sha {}, date {}", data["updateSha"], data["updateDate"]);
            updateState = UpdateState::UPDATE;
          }
        }
      } else if (updateState == UpdateState::UPDATE) {

        auto view = new DownloadView(
            DownloadPath,
            TempPath,
            std::string(paths::BasePath),
            this->settings.overwriteDuringExtract,
            [this](std::string res) {
              updateState = UpdateState::CHECK;
              cacheData = data;

              {
                std::fstream stream(std::string(paths::CacheFilePath), std::ios::out);
                json jdata = cacheData;
                stream << jdata;

                brls::Logger::info("Cache file saved {}", paths::CacheFilePath);
              }

              brls::sync([this](){
                updateUI();
              });
            });
        this->present(view);
      }

    brls::sync([this](){
      updateUI();
    });
      return true; });

  if (fs::exists(paths::CacheFilePath))
  {
    std::fstream stream(fs::path(paths::CacheFilePath), std::ios::in);
    json j = json::parse(stream);
    cacheData = j;

    brls::Logger::info("Loaded cache file {}", paths::CacheFilePath);
  }

  updateUI();
}
