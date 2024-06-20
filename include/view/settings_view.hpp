#pragma once

#include <borealis.hpp>
#include <map>
#include <chrono>

enum class UpdateState
{
  CHECK = 0,
  UPDATE
};

struct SettingsData
{
  bool overwriteDuringExtract = false;
};

class SettingsView : public brls::Box
{
public:
  SettingsView(SettingsData &settings);

  BRLS_BIND(brls::BooleanCell, debug, "debug");
  BRLS_BIND(brls::BooleanCell, extract_overwrite, "extract_overwrite");
  BRLS_BIND(brls::DetailCell, about, "about");
  BRLS_BIND(brls::Button, updateButton, "update_button");
  BRLS_BIND(brls::Label, updateText, "update_status");
  BRLS_BIND(brls::Label, checkText, "check_status");
  BRLS_BIND(brls::Label, cacheText, "cache_status");

  void updateUI();

  std::chrono::time_point<std::chrono::steady_clock> lastCheck;
  UpdateState updateState = UpdateState::CHECK;

  std::map<std::string, std::string> data = {};
  std::map<std::string, std::string> cacheData = {};

  SettingsData &settings;

  // static brls::View *create();
};
