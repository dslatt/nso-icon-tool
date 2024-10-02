/*
    Copyright 2020-2021 natinusala
    Copyright 2019 p-sam

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

#if defined(ANDROID) || defined(IOS)
#include <SDL2/SDL_main.h>
#endif

#include <borealis.hpp>
#include <cstdlib>
#include <string>
#include <filesystem>

#include <switch/services/acc.h>

#include "view/main_view.hpp"
#include "activity/main_activity.hpp"
#include "util/paths.hpp"

#include "view/recycling_grid.hpp"

using namespace brls::literals; // for _i18n
namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
  // We recommend to use INFO for real apps
  for (int i = 1; i < argc; i++)
  {
    if (std::strcmp(argv[i], "-d") == 0)
    { // Set log level
      brls::Logger::setLogLevel(brls::LogLevel::LOG_DEBUG);
    }
    else if (std::strcmp(argv[i], "-o") == 0)
    {
      const char *path = (i + 1 < argc) ? argv[++i] : "borealis.log";
      brls::Logger::setLogOutput(std::fopen(path, "w+"));
    }
    else if (std::strcmp(argv[i], "-v") == 0)
    {
      brls::Application::enableDebuggingView(true);
    }
  }

  try {
    // create expected directories; these need to exist for the app to work
    fs::create_directories(paths::CollectionPath);
  } catch(const std::exception &e) {
    brls::Logger::error("Error creating expected directories; Cant continue: {}", e.what());
    return EXIT_FAILURE;
  }

#ifdef NDEBUG
  brls::Logger::setLogOutput(fopen(std::string(paths::LogFilePath).c_str(), "w"));
#endif

  // Init the app and i18n
  if (!brls::Application::init())
  {
    brls::Logger::error("Unable to init Borealis application");
    return EXIT_FAILURE;
  }

  brls::Application::createWindow("demo/title"_i18n);

  brls::Application::getPlatform()->setThemeVariant(brls::ThemeVariant::DARK);

  // Have the application register an action on every activity that will quit when you press BUTTON_START
  brls::Application::setGlobalQuit(false);

  // Register custom views (including tabs, which are views)
  brls::Application::registerXMLView("MainView", MainView::create);

  brls::Application::registerXMLView("RecyclingGrid", RecyclingGrid::create);
  // <brls:View xml=@res/xml/views/recycling_grid.xml

  // Add custom values to the theme
  // brls::Theme::getLightTheme().addColor("captioned_image/caption", nvgRGB(2, 176, 183));
  // brls::Theme::getDarkTheme().addColor("captioned_image/caption", nvgRGB(51, 186, 227));

  // Add custom values to the style
  brls::getStyle().addMetric("about/padding_top_bottom", 50);
  brls::getStyle().addMetric("about/padding_sides", 75);
  brls::getStyle().addMetric("about/description_margin", 50);

  // Create and push the main activity to the stack
  brls::Application::pushActivity(new MainActivity());

  try {
    // Run the app
    while (brls::Application::mainLoop())
      ;
  } catch(const std::exception &e) {
    brls::Logger::error("Top level exception: {}", e.what());
    return EXIT_FAILURE;
  }

  // Exit
  return EXIT_SUCCESS;
}

#ifdef __WINRT__
#include <borealis/core/main.hpp>
#endif
