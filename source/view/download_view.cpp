#include "view/download_view.hpp"
#include "util/extract.hpp"
#include "util/progress_event.hpp"
#include "util/download.hpp"

#include <regex>
#include <fmt/format.h>
#include <filesystem>

using namespace brls::literals;

DownloadView::DownloadView(std::string url, std::string downloadPath, std::string extractPath, bool overwriteExisting, DownloadDoneEvent::Callback cb) : url(url), downloadPath(downloadPath), extractPath(extractPath), overwriteExisting(overwriteExisting), cb(cb)
{
  this->inflateFromXMLRes("xml/views/download_view.xml");

  ProgressEvent::instance().reset();

  download_text->setText(url);
  extract_text->setText(fmt::format(fmt::runtime("app/download/path_to_path"_i18n), downloadPath, extractPath));

  status_current->setText("");
  status_percent->setText("");

  this->setActionAvailable(brls::ControllerButton::BUTTON_B, false);

  this->setFocusable(true);
  this->setHideHighlightBackground(true);
  this->setHideHighlightBorder(true);

  downloadThread = std::thread(&DownloadView::downloadFile, this);
  updateThread = std::thread(&DownloadView::updateProgress, this);

  brls::sync([this]()
             { getAppletFrame()->setActionAvailable(brls::ControllerButton::BUTTON_B, false); });
}

void DownloadView::downloadFile()
{
  {
    std::unique_lock<std::mutex> lock(threadMutex);
  }

  brls::Logger::info("Download started: {} to {}", url, downloadPath);
  ProgressEvent::instance().reset();
  download::downloadFile(url, downloadPath);
  brls::Logger::info("Download complete");
  this->downloadFinished = true;

  ProgressEvent::instance().reset();

  brls::Logger::info("Extract started: {} to {}", downloadPath, extractPath);
  extract::extract(downloadPath, extractPath, overwriteExisting);
  brls::Logger::info("Extract complete");
  this->extractFinished = true;

  cb("");
}

void DownloadView::updateProgress()
{
  {
    std::unique_lock<std::mutex> lock(threadMutex);
  }
  // DOWNLOAD
  {
    ASYNC_RETAIN
    brls::sync([ASYNC_TOKEN]()
               {
            ASYNC_RELEASE
            download_status->setText("app/download/downloading"_i18n);
            extract_status->setText("app/download/waiting"_i18n); });

    while (ProgressEvent::instance().getTotal() == 0)
    {
      if (downloadFinished)
        break;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    while (!downloadFinished)
    {
      ASYNC_RETAIN
      brls::sync([ASYNC_TOKEN]()
                 {
                ASYNC_RELEASE
                this->status_current->setText(fmt::format("{:.0f}MB ({:.1f}MB/s)",
                  ProgressEvent::instance().getNow() / 1000000.0,
                  ProgressEvent::instance().getSpeed() / 1000000.0)); });
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
  // EXTRACT
  {
    ASYNC_RETAIN
    brls::sync([ASYNC_TOKEN]()
               {
            ASYNC_RELEASE
            download_status->setText("app/download/downloaded"_i18n);
            extract_status->setText("app/download/extracting"_i18n); });
    while (ProgressEvent::instance().getMax() == 0)
    {
      if (extractFinished)
        break;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    while (ProgressEvent::instance().getStep() < ProgressEvent::instance().getMax() && !extractFinished)
    {
      ASYNC_RETAIN
      brls::sync([ASYNC_TOKEN]()
                 {
                ASYNC_RELEASE
                this->status_current->setText(ProgressEvent::instance().getMsg());
                this->status_percent->setText(fmt::format("{}%", (int)((ProgressEvent::instance().getStep() * 100 / ProgressEvent::instance().getMax())))); });
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    {
      ASYNC_RETAIN
      brls::sync([ASYNC_TOKEN]()
                 {
              ASYNC_RELEASE
              this->status_spinner->animate(false);
              this->status_spinner->setVisibility(brls::Visibility::INVISIBLE);
              this->status_current->setText("");
              this->status_percent->setText("");
              extract_status->setText("app/download/extracted"_i18n); });
    }
  }
  // CLEANUP
  {
    std::filesystem::remove(downloadPath);
  }

  // Add a button to go back after the end of the download
  ASYNC_RETAIN
  brls::sync([ASYNC_TOKEN]()
             {
        ASYNC_RELEASE
        auto button = new brls::Button();
        button->setText("app/download/back"_i18n);
        button->setFocusable(true);
        button->registerClickAction(brls::ActionListener([this](brls::View* view) {
            this->dismiss();
            return true;
        }));
        this->addView(button);
        brls::Application::giveFocus(button);
        getAppletFrame()->setActionAvailable(brls::ControllerButton::BUTTON_B, true); });
}
