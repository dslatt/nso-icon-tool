#pragma once

#include <switch.h>
#include "util/image.hpp"

namespace account
{
  struct UserInfo
  {
    AccountUid uid;
    AccountProfile profile;
    AccountUserData data;
    AccountProfileBase base;
  };

  void init();
  bool isValid(UserInfo &user);
  void selectUser(UserInfo &user);
  Image getProfileImage(UserInfo &user);
  bool setUserIcon(UserInfo &user, Image &image);
}