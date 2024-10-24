#pragma once

#include <borealis.hpp>
#include <atomic>

typedef brls::Event<std::string> DownloadDoneEvent;

class DownloadView : public brls::Box
{
public:
  DownloadView(std::string url, std::string downloadPath, std::string extractPath, bool overwriteExisting, DownloadDoneEvent::Callback cb);
  ~DownloadView()
  {
    if (downloadThread.joinable())
      downloadThread.join();
    if (updateThread.joinable())
      updateThread.join();
  }

private:
  std::string url, downloadPath, extractPath;

  BRLS_BIND(brls::Label, download_text, "download_text");
  BRLS_BIND(brls::Label, download_status, "download_status");
  BRLS_BIND(brls::Label, extract_text, "extract_text");
  BRLS_BIND(brls::Label, extract_status, "extract_status");

  BRLS_BIND(brls::ProgressSpinner, status_spinner, "status_spinner");
  BRLS_BIND(brls::Label, status_percent, "status_percent");
  BRLS_BIND(brls::Label, status_current, "status_current");

  void downloadFile();
  void updateProgress();

  std::jthread updateThread;
  std::jthread downloadThread;
  std::mutex threadMutex;
  std::condition_variable threadCondition;

  DownloadDoneEvent::Callback cb;

  std::atomic_flag downloadFinished;
  std::atomic_flag extractFinished;
  bool overwriteExisting;
};