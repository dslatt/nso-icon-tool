#include "util/download.hpp"

#include <curl/curl.h>
#include <fmt/format.h>
#include <math.h>
#include <switch.h>
#include <time.h>

#include <algorithm>
#include <borealis.hpp>
#include <chrono>
#include <regex>
#include <string>
#include <thread>

#include "util/progress_event.hpp"

using namespace brls::literals; // for _i18n

constexpr const char API_AGENT[] = "nso-icons";
constexpr int _1MiB              = 0x100000;

using json = nlohmann::ordered_json;

namespace download {

namespace {

  std::chrono::_V2::steady_clock::time_point time_old;
  double dlold;

  typedef struct {
    char* memory;
    size_t size;
  } MemoryStruct_t;

  typedef struct {
    u_int8_t* data;
    size_t data_size;
    u_int64_t offset;
    FILE* out;
    Aes128CtrContext* aes;
  } ntwrk_struct_t;

  static size_t WriteMemoryCallback(void* contents, size_t size, size_t num_files, void* userp)
  {
    if (ProgressEvent::instance().getInterupt()) {
      return 0;
    }
    ntwrk_struct_t* data_struct = (ntwrk_struct_t*)userp;
    size_t realsize             = size * num_files;

    if (realsize + data_struct->offset >= data_struct->data_size) {
      fwrite(data_struct->data, data_struct->offset, 1, data_struct->out);
      data_struct->offset = 0;
    }

    if (data_struct->aes)
      aes128CtrCrypt(data_struct->aes, &data_struct->data[data_struct->offset], contents, realsize);
    else
      memcpy(&data_struct->data[data_struct->offset], contents, realsize);

    data_struct->offset += realsize;
    data_struct->data[data_struct->offset] = 0;
    return realsize;
  }

  int download_progress(void* p, double dltotal, double dlnow, double ultotal, double ulnow)
  {

    if (dltotal <= 0.0) {
      // chunk download
      int counter = (int)(0 * ProgressEvent::instance().getMax()); // 20 is the number of increments
      ProgressEvent::instance().setStep(std::min(ProgressEvent::instance().getMax() - 1, counter));
      ProgressEvent::instance().setNow(dlnow);
      ProgressEvent::instance().setTotalCount(dlnow);
    } else {
      double fractionDownloaded = dlnow / dltotal;

      int counter = (int)(fractionDownloaded * ProgressEvent::instance().getMax()); // 20 is the number of increments
      ProgressEvent::instance().setStep(std::min(ProgressEvent::instance().getMax() - 1, counter));
      ProgressEvent::instance().setNow(dlnow);
      ProgressEvent::instance().setTotalCount(dltotal);
    }

    auto time_now       = std::chrono::steady_clock::now();
    double elasped_time = ((std::chrono::duration<double>)(time_now - time_old)).count();
    if (elasped_time > 1.2f) {
      ProgressEvent::instance().setSpeed((dlnow - dlold) / elasped_time);
      dlold    = dlnow;
      time_old = time_now;
    }

    return 0;
  }

  struct MemoryStruct {
    char* memory;
    size_t size;
  };

  static size_t WriteMemoryCallback2(void* contents, size_t size, size_t nmemb, void* userp)
  {
    size_t realsize          = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

    char* ptr = static_cast<char*>(realloc(mem->memory, mem->size + realsize + 1));
    if (ptr == NULL) {
      /* out of memory! */
      return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
  }

  bool checkSize(CURL* curl, const std::string& url)
  {
    curl_off_t dl;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, API_AGENT);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_perform(curl);
    auto res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &dl);
    if (!res) {
      s64 freeStorage;
      if (R_SUCCEEDED(nsGetFreeSpaceSize(NcmStorageId_SdCard, &freeStorage)) && dl * 2.5 > freeStorage) {
        return false;
      }
      return true;
    }
    return true;
  }
} // namespace

long downloadFile(const std::string& url, const std::string& output, int api)
{
  std::vector<std::uint8_t> dummy;
  return downloadFile(url, dummy, output, api);
}

long downloadFile(const std::string& url, std::vector<std::uint8_t>& res, const std::string& output, int api)
{
  const char* out      = output.c_str();
  CURL* curl           = curl_easy_init();
  ntwrk_struct_t chunk = { 0 };
  long status_code;
  time_old             = std::chrono::steady_clock::now();
  dlold                = 0.0f;
  bool can_download    = true;
  bool is_mega         = false;
  std::string real_url = url;

  if (curl) {
    FILE* fp = fopen(out, "wb");
    if (fp || *out == 0) {
      chunk.data      = static_cast<u_int8_t*>(malloc(_1MiB));
      chunk.data_size = _1MiB;
      chunk.out       = fp;

      if (*out != 0) {
        can_download = checkSize(curl, url);
      }

      if (can_download) {
        curl_easy_setopt(curl, CURLOPT_URL, real_url.c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT, API_AGENT);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, nullptr);

        if (api == OFF) {
          curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
          curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, download_progress);
        }
        curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);

        if (fp && chunk.offset && can_download)
          fwrite(chunk.data, 1, chunk.offset, fp);

        curl_easy_cleanup(curl);
        ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
      }
    }
  }

  fclose(chunk.out);
  if (!can_download) {
    brls::Application::crash("app/errors/insufficient_storage"_i18n);
    std::this_thread::sleep_for(std::chrono::microseconds(2000000));
    brls::Application::quit();
    res = {};
  }

  if (*out == 0) {
    res.assign(chunk.data, chunk.data + chunk.offset);
  }

  free(chunk.data);
  free(chunk.aes);

  return status_code;
}

long downloadPage(
    const std::string& url, std::string& res, const std::vector<std::string>& headers, const std::string& body)
{
  CURL* curl_handle;
  struct MemoryStruct chunk;
  struct curl_slist* list = NULL;
  long status_code;

  chunk.memory = static_cast<char*>(malloc(1));
  chunk.size   = 0;

  curl_global_init(CURL_GLOBAL_ALL);
  curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
  if (!headers.empty()) {
    for (auto& h : headers) {
      list = curl_slist_append(list, h.c_str());
    }
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, list);
  }
  if (body != "") {
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, body.c_str());
  }

  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback2);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, API_AGENT);
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

  curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_perform(curl_handle);
  curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &status_code);
  curl_easy_cleanup(curl_handle);
  res = std::string(chunk.memory);
  free(chunk.memory);

  curl_global_cleanup();
  return status_code;
}

long getRequest(const std::string& url, nlohmann::ordered_json& res, const std::vector<std::string>& headers,
    const std::string& body)
{
  std::string request;
  brls::Logger::info("request {}", url);
  long status_code = downloadPage(url, request, headers, body);

  if (json::accept(request))
    res = nlohmann::ordered_json::parse(request);
  else
    res = nlohmann::ordered_json::object();

  return status_code;
}

void init() { curl_global_init(CURL_GLOBAL_ALL); }

} // namespace download