#include "util/extract.hpp"

#include <string>
#include <borealis.hpp>
#include <strings.h>
#include <filesystem>
#include <fstream>
#include <archive.h>
#include <archive_entry.h>

#include <switch.h>
#include <chrono>

#include "util/progress_event.hpp"

using namespace brls::literals; // for _i18n
namespace fs = std::filesystem;

constexpr size_t WRITE_BUFFER_SIZE = 0x20000;

namespace extract
{
  std::tuple<int64_t, int64_t> getFileStats(const std::string &archivePath)
  {
    std::tuple<int64_t, int64_t> stats{0, 0};
    struct archive* archive;
    struct archive_entry* entry;

    archive = archive_read_new();
    archive_read_support_format_all(archive);
    archive_read_support_filter_all(archive);

    if(archive_read_open_filename(archive, archivePath.c_str(), 10240) == ARCHIVE_OK) {
        while(archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
            std::get<0>(stats) += 1;
            std::get<1>(stats) += archive_entry_size(entry);
        }
        archive_read_close(archive);
    }

    archive_read_free(archive);
    return stats;
  }

  void ensureAvailableStorage(size_t uncompressedSize)
  {
    s64 freeStorage;

    if (R_SUCCEEDED(nsGetFreeSpaceSize(NcmStorageId_SdCard, &freeStorage)))
    {
      brls::Logger::info("Uncompressed size of archive: {}. Available: {}", uncompressedSize, freeStorage);
      if (uncompressedSize * 1.1 > freeStorage)
      {
        brls::Application::crash("app/errors/insufficient_storage"_i18n);
        std::this_thread::sleep_for(std::chrono::microseconds(2000000));
        brls::Application::quit();
      }
    }
  }

  void extract(const std::string &archivePath, const std::string &workingPath, bool overwriteExisting) {
    auto start = std::chrono::high_resolution_clock::now();

    auto [totalFiles, totalSize] = getFileStats(archivePath);
    ensureAvailableStorage(totalSize);

    brls::sync([totalFiles, totalSize]() {
      brls::Logger::info("Extracting {} {} entries", totalFiles, totalSize);
    });

    ProgressEvent::instance().setTotalSteps(totalFiles);
    ProgressEvent::instance().setStep(0);

    struct archive *a;
    struct archive_entry *entry;
    int err = 0, i = 0, count = 0;

    //char buf[WRITE_BUFFER_SIZE];
    //auto buf = std::make_unique<char[]>(WRITE_BUFFER_SIZE);

    a = archive_read_new();
    archive_read_support_format_all(a);
    archive_read_support_filter_all(a);

    if ((err = archive_read_open_filename(a, archivePath.c_str(), 10240))) {
      brls::sync([err = std::string(archive_error_string(a))]() {
        brls::Logger::error("Error opening archive: {}", err);
      });
      archive_read_free(a);
      return;
    }

    for (;;) {
      if (ProgressEvent::instance().getInterupt()) {
        ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
        break;
      }

      err = archive_read_next_header(a, &entry);
      if (err == ARCHIVE_EOF) {
        ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
        break;
      }
      if (err < ARCHIVE_OK)
        brls::sync([archivePath, err = std::string(archive_error_string(a))]() {
          brls::Logger::error("Error reading archive entry: {}", err);
        });
      if (err < ARCHIVE_WARN) {
        break;
      }

      auto filepath = fs::path(workingPath) / archive_entry_pathname(entry);

      if (archive_entry_filetype(entry) == AE_IFDIR) {
        fs::create_directories(filepath);
        ProgressEvent::instance().setStep(++i);
        continue;
      }

      if (fs::exists(filepath) && !overwriteExisting) {
        ProgressEvent::instance().setStep(++i);
        continue;
      }

      std::ofstream outfile(filepath.string(), std::ios::binary | std::ios::trunc);
      if (!outfile.is_open()) {
        brls::sync([path = filepath.string()]() {
          brls::Logger::error("Error opening write file for archive entry: {}", path);
        });
        break;
      }

      const void* buff = nullptr;
      size_t size = 0;
      int64_t offset = 0;
      int res = -1;
      while ((res = archive_read_data_block(a, &buff, &size, &offset)) == ARCHIVE_OK) {
        try {
          outfile.write(static_cast<const char*>(buff), size);
        } catch(const std::exception& e) {
          res = ARCHIVE_FATAL;
          break;
        }
      }

      if (res != ARCHIVE_EOF) {
        brls::sync([res = std::string(archive_error_string(a))]() {
          brls::Logger::error("Error writing out archive entry: {}", res);
        });
        outfile.close();
        fs::remove(filepath);
        break;
      }

      count++;
      ProgressEvent::instance().setStep(++i);
    }

    archive_read_close(a);
    archive_read_free(a);

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

    brls::sync([elapsed, totalFiles]() {
      brls::Logger::info("Total extraction time: {}s for {} files", elapsed, count);
    });
  }
}