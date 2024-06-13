#pragma once

#include <borealis.hpp>
#include <string>

class EmptyMessage : public brls::Box
{
public:
  EmptyMessage(std::string message);

private:
  BRLS_BIND(brls::Label, missingMsg, "missing_msg");
};
