#pragma once

#include <borealis.hpp>

class AboutView : public brls::Box {
public:
  AboutView();

  BRLS_BIND(brls::Label, appVersion, "version");
  BRLS_BIND(brls::Label, appAuthor, "author");
  BRLS_BIND(brls::Label, gitSha, "git_sha");
  BRLS_BIND(brls::Label, gitDate, "git_date");
};
