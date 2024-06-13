/*
    Copyright 2021 natinusala

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "view/settings_view.hpp"
#include "view/download_view.hpp"

#include "extern/json.hpp"
#include "util/download.hpp"
#include <GenericToolbox.h>
#include "util/paths.hpp"

#include "version.h"

using json = nlohmann::json;

const std::string ApiPath = "http://api.github.com/repos/henry-debruin/nso-icons/branches/main";
const std::string DownloadPath = "https://github.com/henry-debruin/nso-icons/archive/refs/heads/main.zip";

const std::string TempPath = paths::BasePath + "icon_archive_temp.zip";

using namespace brls::literals; // for _i18n

void SettingsView::updateUI()
{

  appVersion->setText(fmt::format("Version: {}", version::AppVersion));
  appAuthor->setText(fmt::format("Author: {}", version::AppAuthor));
  gitSha->setText(fmt::format("Commit: {} ({})", version::GitHeadSHA1, version::GitDirty ? "dirty" : "clean"));
  gitDate->setText(fmt::format("Commit Date: {}", version::GitCommitDate));

  cacheText->setText(GenericToolbox::doesPathIsFolder(paths::IconCachePath) ? "Icon Cache Found" : "No Icon Cache Found");
  checkText->setText(fmt::format("Last Checked: {}", cacheData.count("checkTime") ? cacheData["checkTime"] : "Never"));
  if (updateState == UpdateState::CHECK)
  {
    updateText->setText(fmt::format("Current Version: {} (sha {})",
                                    cacheData.count("updateDate") ? cacheData["updateDate"] : "None",
                                    cacheData.count("updateSha") ? cacheData["updateSha"] : "None"));
    updateButton->setText("Check for Updates");
  }
  else if (updateState == UpdateState::UPDATE)
  {
    updateText->setText(fmt::format("Update Available: {} (sha {})", data["updateDate"], data["updateSha"]));
    updateButton->setText("Download/Apply Update");
  }
}

SettingsView::SettingsView(SettingsData &settings) : settings(settings)
{
  // Inflate the tab from the XML file
  this->inflateFromXMLRes("xml/views/settings.xml");

  checkSpinner->setVisibility(brls::Visibility::INVISIBLE);

  debug->init("Debug Layer", brls::Application::isDebuggingViewEnabled(), [](bool value)
              {
        brls::Application::enableDebuggingView(value);
        brls::sync([value](){
            brls::Logger::info("{} the debug layer", value ? "Open" : "Close");
        }); });

  extract_overwrite->init("Overwrite Existing Files During Update", settings.overwriteDuringExtract, [&settings](bool value)
                          { settings.overwriteDuringExtract = value;
                            brls::sync([value]()
                                       {
                                        brls::Logger::info("extract check? {}", value ? "Overwrite" : "No Overwrite"); 
                                         }); });

  updateButton->registerClickAction([this](...)
                                    {
      if (updateState == UpdateState::CHECK) {
        nlohmann::ordered_json jsondata = {};

        checkSpinner->setVisibility(brls::Visibility::VISIBLE);

        data["checkTime"] = fmt::format("{:%FT%T%Z}", fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())));
        cacheData["checkTime"] = data["checkTime"];

        {
          std::fstream stream(paths::CacheFilePath, std::ios::out);
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

          if (!cacheData.count("updateSha") || (!GenericToolbox::isDir(paths::IconCachePath) || GenericToolbox::isDirEmpty(paths::IconCachePath)) || data["updateSha"] != cacheData["updateSha"]) {
            brls::Logger::info("Update available: sha {}, date {}", data["updateSha"], data["updateDate"]);
            updateText->setText("Update Available");
            updateButton->setText("Download/Apply Update");
            updateState = UpdateState::UPDATE;
          }

          checkSpinner->setVisibility(brls::Visibility::INVISIBLE);
        }
      } else if (updateState == UpdateState::UPDATE) {

        auto view = new DownloadView(
            DownloadPath,
            TempPath,
            paths::BasePath,
            this->settings.overwriteDuringExtract,
            [this](std::string res) {
              updateState = UpdateState::CHECK;
              cacheData = data;

              {
                std::fstream stream(paths::CacheFilePath, std::ios::out);
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

  if (GenericToolbox::isFile(paths::CacheFilePath))
  {
    std::fstream stream(paths::CacheFilePath, std::ios::in);
    json j = json::parse(stream);
    cacheData = j;

    brls::Logger::info("Loaded cache file {}", paths::CacheFilePath);
  }

  updateUI();
}
