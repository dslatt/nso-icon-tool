#pragma once

#include <filesystem>
#include <string>
#include <fstream>

#include <minizip/unzip.h>

namespace zip {


constexpr size_t WRITE_BUFFER_SIZE = 0x10000;

// TODO: zipentry seems ok, but untested
//       zipfile & iterator need to be reworked from the ai suggesstion... theyre pretty bad

class ZipEntry {
public:
    ZipEntry(unzFile& zfile) : zfile(zfile) {}

    const std::string& filename() {
      if (filename_.empty()) {
        char bfr[0x301];
        unzGetCurrentFileInfo(zfile, nullptr, bfr, sizeof(bfr) ,nullptr, 0, nullptr, 0);
        filename_ = bfr;
      }

      return filename_;
    };

    const unz_file_info& info() {
      if (info_.uncompressed_size == 0) {
        unzGetCurrentFileInfo(zfile, &info_, nullptr, 0, nullptr, 0, nullptr, 0);
      }

      return info_; 
    };

    bool extractToFile(const std::string& outputFilename, bool forceCreateTree = false) {
      try {
        // create dir tree
        if (filename().back() == '/')
        {
          std::filesystem::create_directories(filename());
          return false;
        }
        if (forceCreateTree)
        {
          std::filesystem::create_directories(filename());
        }

        // Open the output file and write
        std::ofstream outfile(outputFilename, std::ios::binary | std::ios::out);

        auto buffer = std::make_unique<unsigned char[]>(WRITE_BUFFER_SIZE);
        for (auto j = unzReadCurrentFile(zfile, buffer.get(), WRITE_BUFFER_SIZE); j > 0; j = unzReadCurrentFile(zfile, buffer.get(), WRITE_BUFFER_SIZE))
        {
          outfile.write(reinterpret_cast<const char*>(buffer.get()), j);
        }
      } catch (const std::exception& e) {
        return false;
      }

      return true;
    }

    std::string filename_;
    unz_file_info info_;
    unzFile& zfile;
};

class ZipFile {
public:
    ZipFile(std::string filename) : filename(std::move(filename)) {
      fileHandle = unzOpen(filename.c_str());
      if (fileHandle == NULL) {
        throw std::runtime_error("Failed to open zip file");
      }

      unz_global_info gi;
      unzGetGlobalInfo(fileHandle, &gi);
      maxIndex = gi.number_entry;
    }

    ~ZipFile() {
      unzCloseCurrentFile(fileHandle);
      unzClose(fileHandle);
    }

    const s64 getUncompressedSize() {
      if (uncompressedSize == 0) {
        for (auto i = 0; i < maxIndex; ++i)
        {
          unz_file_info fi;
          unzOpenCurrentFile(fileHandle);
          unzGetCurrentFileInfo(fileHandle, &fi, NULL, 0, NULL, 0, NULL, 0);
          uncompressedSize += fi.uncompressed_size;
          unzCloseCurrentFile(fileHandle);
          unzGoToNextFile(fileHandle);
        }
        unzGoToFirstFile(fileHandle);
      }
      return uncompressedSize;
    }

    class iterator {
    public:
        iterator(unzFile& file, size_t index) : fileHandle(file), index(index) {
        }

        bool operator!=(const iterator& other) const {
            return index != other.index;
        }

        ZipEntry operator*() const {
            return ZipEntry(fileHandle);
        }

        iterator& operator++() {
            unzCloseCurrentFile(fileHandle);
            unzGoToNextFile(fileHandle);
            unzOpenCurrentFile(fileHandle);
            ++index;
            return *this;
        }

    private:
        unzFile& fileHandle;
        size_t index{0};
    };

    iterator begin() {
        return iterator(fileHandle, 0);
    }

    iterator end() {
        return iterator(fileHandle, maxIndex);
    }

    unzFile fileHandle;
    std::string filename;
    size_t index{0}, maxIndex{0};
    s64 uncompressedSize{0};
};

}