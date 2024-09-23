#include "view/about_view.hpp"

#include "version.h"


using namespace brls::literals; // for _i18n

AboutView::AboutView()
{
  // Inflate the tab from the XML file
  this->inflateFromXMLRes("xml/views/about.xml");

  appVersion->setText(fmt::format(fmt::runtime("app/settings/version/version"_i18n), version::AppVersion));
  appAuthor->setText(fmt::format(fmt::runtime("app/settings/version/author"_i18n), version::AppAuthor));
  gitSha->setText(fmt::format(fmt::runtime("app/settings/version/commit"_i18n), version::GitHeadSHA1, version::GitDirty ? "app/settings/version/commit_dirty"_i18n : "app/settings/version/commit_clean"_i18n));
  gitDate->setText(fmt::format(fmt::runtime("app/settings/version/commit_date"_i18n), version::GitCommitDate));
}
