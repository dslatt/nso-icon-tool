#include "util/extract.hpp"

#include <dirent.h>
#include <minizip/unzip.h>

#include <string>
#include <vector>
#include <borealis.hpp>
#include <strings.h>

#include <switch.h>
#include <GenericToolbox.Fs.h>

#include "util/progress_event.hpp"

using namespace brls::literals; // for _i18n

constexpr size_t WRITE_BUFFER_SIZE = 0x10000;

namespace extract
{
  namespace
  {
    bool caselessCompare(const std::string &a, const std::string &b)
    {
      return strcasecmp(a.c_str(), b.c_str()) == 0;
    }

    s64 getUncompressedSize(const std::string &archivePath)
    {
      s64 size = 0;
      unzFile zfile = unzOpen(archivePath.c_str());
      unz_global_info gi;
      unzGetGlobalInfo(zfile, &gi);
      for (uLong i = 0; i < gi.number_entry; ++i)
      {
        unz_file_info fi;
        unzOpenCurrentFile(zfile);
        unzGetCurrentFileInfo(zfile, &fi, NULL, 0, NULL, 0, NULL, 0);
        size += fi.uncompressed_size;
        unzCloseCurrentFile(zfile);
        unzGoToNextFile(zfile);
      }
      unzClose(zfile);
      return size; // in B
    }

    void ensureAvailableStorage(const std::string &archivePath)
    {
      s64 uncompressedSize = getUncompressedSize(archivePath);
      s64 freeStorage;

      if (R_SUCCEEDED(nsGetFreeSpaceSize(NcmStorageId_SdCard, &freeStorage)))
      {
        brls::Logger::info("Uncompressed size of archive {}: {}. Available: {}", archivePath, uncompressedSize, freeStorage);
        if (uncompressedSize * 1.1 > freeStorage)
        {
          brls::Application::crash("app/errors/insufficient_storage"_i18n);
          std::this_thread::sleep_for(std::chrono::microseconds(2000000));
          brls::Application::quit();
        }
      }
    }

    void extractEntry(std::string filename, unzFile &zfile, bool forceCreateTree = false)
    {
      if (filename.back() == '/')
      {
        GenericToolbox::mkdir(filename);
        return;
      }
      if (forceCreateTree)
      {
        GenericToolbox::mkdir(filename);
      }
      void *buf = malloc(WRITE_BUFFER_SIZE);
      FILE *outfile;
      outfile = fopen(filename.c_str(), "wb");
      for (int j = unzReadCurrentFile(zfile, buf, WRITE_BUFFER_SIZE); j > 0; j = unzReadCurrentFile(zfile, buf, WRITE_BUFFER_SIZE))
      {
        fwrite(buf, 1, j, outfile);
      }
      free(buf);
      fclose(outfile);
    }
  }

  void extract(const std::string &archivePath, const std::string &workingPath, bool overwriteExisting, std::function<void()> func)
  {
    ensureAvailableStorage(archivePath);

    unzFile zfile = unzOpen(archivePath.c_str());
    unz_global_info gi;
    unzGetGlobalInfo(zfile, &gi);

    ProgressEvent::instance().setTotalSteps(gi.number_entry);
    ProgressEvent::instance().setStep(0);

    for (uLong i = 0; i < gi.number_entry; ++i)
    {
      char szFilename[0x301] = "";
      unzOpenCurrentFile(zfile);
      unzGetCurrentFileInfo(zfile, NULL, szFilename, sizeof(szFilename), NULL, 0, NULL, 0);
      std::string filename = workingPath + szFilename;

      if (ProgressEvent::instance().getInterupt())
      {
        unzCloseCurrentFile(zfile);
        break;
      }

      if (overwriteExisting || !GenericToolbox::isPathValid(filename))
      {
        ProgressEvent::instance().setMsg(filename);
        extractEntry(filename, zfile);
      }

      ProgressEvent::instance().setStep(i);
      unzCloseCurrentFile(zfile);
      unzGoToNextFile(zfile);
    }
    unzClose(zfile);
    ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
  }
}