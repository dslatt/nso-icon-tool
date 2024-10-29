#pragma once

constexpr int ON  = 1;
constexpr int OFF = 0;

#include <cstdint>
#include <string>
#include <vector>

#include "extern/json.hpp"

namespace download {
void init();
long downloadFile(
    const std::string& url, std::vector<std::uint8_t>& res, const std::string& output = "", int api = OFF);
long downloadFile(const std::string& url, const std::string& output = "", int api = OFF);
long downloadPage(const std::string& url, std::string& res, const std::vector<std::string>& headers = {},
    const std::string& body = "");
long getRequest(const std::string& url, nlohmann::ordered_json& res, const std::vector<std::string>& headers = {},
    const std::string& body = "");

}