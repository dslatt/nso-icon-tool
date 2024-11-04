#include "util/account.hpp"

#include <switch.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <string>

#include "borealis.hpp"
#include "util/paths.hpp"

namespace fs = std::filesystem;

namespace account {

void init() { accountInitialize(AccountServiceType_Administrator); }

bool isValid(UserInfo& user) { return accountUidIsValid(&user.uid); }

void selectUser(UserInfo& user)
{
  AccountUid uid                      = {};
  const PselUserSelectionSettings cfg = {};
  auto res                            = pselShowUserSelector(&uid, &cfg);
  if (R_SUCCEEDED(res) && accountUidIsValid(&uid))
    user.uid = uid;
}

Image getProfileImage(UserInfo& user)
{
  Image image;
  auto* service = accountGetServiceSession();
  if (!service)
    return image;

  if (!accountUidIsValid(&user.uid))
    return image;

  auto res = accountGetProfile(&user.profile, user.uid);
  if (!R_SUCCEEDED(res))
    return image;

  res = accountProfileGet(&user.profile, &user.data, &user.base);
  if (!R_SUCCEEDED(res))
    return image;

  u32 imageSize = 0, tmpSize = 0;
  res = accountProfileGetImageSize(&user.profile, &imageSize);
  if (!R_SUCCEEDED(res))
    return image;

  auto buffer = std::make_unique<char[]>(imageSize);
  res         = accountProfileLoadImage(&user.profile, (void*)buffer.get(), imageSize, &tmpSize);
  if (!R_SUCCEEDED(res))
    return image;

  image = Image((unsigned char*)buffer.get(), imageSize);
  return image;
}

bool setUserIcon(UserInfo& user, Image& image)
{
  auto path = fs::path(paths::BaseAppPath) / "tmpicon.jpg";
  image.writeJpg(path);

  auto* service = accountGetServiceSession();
  if (!service)
    return false;

  if (!accountUidIsValid(&user.uid))
    return false;

  AccountProfile profile;
  auto res = accountGetProfile(&profile, user.uid);
  if (!R_SUCCEEDED(res))
    return false;

  AccountProfileBase base = {};
  AccountUserData data    = {};
  res                     = accountProfileGet(&profile, &data, &base);
  if (!R_SUCCEEDED(res))
    return false;

  Service editor;
  res = serviceDispatchIn(service, 205, user.uid, .out_num_objects = 1, .out_objects = &editor, );
  if (!R_SUCCEEDED(res))
    return false;

  {
    auto size   = fs::file_size(path);
    auto buffer = std::make_unique<char[]>(size);
    std::fstream stream(path, std::ios::in);
    stream.read(buffer.get(), size);

    res = serviceDispatchIn(&editor, 101, base,
                              .buffer_attrs = {
                                  SfBufferAttr_FixedSize | SfBufferAttr_In | SfBufferAttr_HipcPointer,
                                  SfBufferAttr_HipcMapAlias | SfBufferAttr_In,
                              },
                              .buffers = {
                                  {&data, sizeof(data)},
                                  {(void *)buffer.get(), (size_t)size},
                              });
  }

  fs::remove(path);
  return true;
}
}