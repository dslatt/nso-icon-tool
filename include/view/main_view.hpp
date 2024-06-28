#pragma once

#include <borealis.hpp>
#include "util/account.hpp"
#include "util/image.hpp"
#include "view/settings_view.hpp"
#include "state/image_state.hpp"

class MainView : public brls::Box
{
public:
  MainView();

  static brls::View *create();

  void handleUserSelection();

  BRLS_BIND(brls::DetailCell, btnChangeUser, "btn_change_user");
  BRLS_BIND(brls::DetailCell, btnFrame, "btn_frame");
  BRLS_BIND(brls::DetailCell, btnCharacter, "btn_character");
  BRLS_BIND(brls::DetailCell, btnBackground, "btn_background");
  BRLS_BIND(brls::Button, btnSave, "btn_save");
  BRLS_BIND(brls::DetailCell, btnCustom, "btn_custom");
  BRLS_BIND(brls::DetailCell, btnSettings, "btn_settings");

  BRLS_BIND(brls::DetailCell, btnCollectionLoad, "btn_collection_load");

  BRLS_BIND(brls::Label, currentUser, "current_user");
  BRLS_BIND(brls::Image, currentImage, "current_image");
  BRLS_BIND(brls::Image, image, "image");

  ImageState imageState, tempState;
  account::UserInfo user;

  SettingsData settings;
};
