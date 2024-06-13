//
// Created by Nadrino on 01/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_SWITCH_H
#define CPP_GENERIC_TOOLBOX_SWITCH_H

#ifdef __SWITCH__

#include "GenericToolbox.Utils.h"
#include "GenericToolbox.Macro.h"
#include "GenericToolbox.Time.h"
#include "GenericToolbox.Os.h"
#include "GenericToolbox.Fs.h"
#include "GenericToolbox.String.h"

#include "switch.h"
#include "zlib.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include <sys/stat.h>


namespace GenericToolbox::Switch{

  // IO
  namespace IO{

    struct ParametersHolder{
      bool useCrcCheck{true};
      static const size_t maxBufferSize{0x200000}; // 2 MB
      static const size_t minBufferSize{0x10000}; // 65 KB
    };
    static ParametersHolder p{};

    // Read/Write
    static bool copyFile(const std::string& srcFilePath_, const std::string& dstFilePath_, bool force_=true);
    static bool doFilesAreIdentical(const std::string& file1Path_, const std::string& file2Path_);
  }

  // Utils
  namespace Utils{
    struct BuffersHolder{
      std::map<std::string, std::string> strMap;
      std::map<std::string, double> progressMap;
    };
    static BuffersHolder b{};

    static bool isTidLike(const std::string& name_);
    static std::string lookForTidInSubFolders(const std::string& folderPath_, int maxDepth_ = 5);
    static uint8_t* getIconFromTitleId(const std::string& titleId_);
  }

  namespace UI{
    static std::string openKeyboardUi(const std::string &defaultStr_ = "");
  }

  // Printout
  namespace Terminal{
    static void printRight(const std::string& input_, const std::string& color_ = "", bool flush_ = false);
    static void printLeft(const std::string& input_, const std::string& color_ = "", bool flush_ = false);
    static void printLeftRight(const std::string& inputLeft_, const std::string& inputRight_, const std::string& color_ = "", bool flush_ = false);

    static void makePause(const std::string& message_ = "");
    template<typename T, typename TT> static void displayProgressBar( const T& iCurrent_, const TT& iTotal_,
        const std::string &title_ = "", bool forcePrint_ = false, const std::string& color_="");

  }

  // Hardware
  namespace Hardware{

#define ENUM_NAME PhysicalMemoryType
#define ENUM_FIELDS \
  ENUM_FIELD(TotalPhysicalMemorySize, 0) \
  ENUM_FIELD(UsedPhysicalMemorySize)
#include "GenericToolbox.MakeEnum.h"

#define ENUM_NAME PhysicalMemoryOf
#define ENUM_FIELDS \
  ENUM_FIELD(Application, 0) \
  ENUM_FIELD(Applet) \
  ENUM_FIELD(System) \
  ENUM_FIELD(SystemUnsafe)
#include "GenericToolbox.MakeEnum.h"

    static u64 getMemoryInfo(PhysicalMemoryType type_, PhysicalMemoryOf of_);
    static std::string getMemoryUsageStr(PhysicalMemoryOf of_);

  }

}

namespace GenericToolbox::Switch {

  namespace IO{
    static bool copyFile(const std::string& srcFilePath_, const std::string& dstFilePath_, bool force_){
      bool isSuccess{false};

      if( not GenericToolbox::isFile(srcFilePath_) ){ return false; }

      if( isFile(dstFilePath_) ){
        if( not force_ ){ return false; }
        if( not rm(dstFilePath_) ){ return false; }
      }

      auto outDir = getFolderPath( dstFilePath_ );
      mkdir( outDir );

      ssize_t srcFileSize = getFileSize(srcFilePath_);
      std::ifstream in(srcFilePath_, std::ios::in | std::ios::binary);
      std::ofstream out(dstFilePath_, std::ios::out | std::ios::binary);

      auto bufferSize = ssize_t(srcFileSize/500); // 0.2 chunk per % (can see on pixels)
      bufferSize = std::min(bufferSize, ssize_t(GenericToolbox::Switch::IO::ParametersHolder::maxBufferSize)); // cap the buffer size -> not too big
      bufferSize = std::max(bufferSize, ssize_t(GenericToolbox::Switch::IO::ParametersHolder::minBufferSize)); // cap the buffer size -> not too small
      std::vector<char> contentBuffer(bufferSize, 0);
      size_t nChunk = (size_t(srcFileSize)/bufferSize) + 1;
      Utils::b.progressMap["copyFile"] = double(1) / double(nChunk);
      std::string pTitle = GenericToolbox::getFileName(srcFilePath_) + " -> " + outDir;

      size_t timeLoad{0};
      size_t timeDrop{0};

      for( size_t iChunk = 0 ; iChunk < nChunk ; iChunk++ ) {
//        GenericToolbox::displayProgressBar(iChunk, nChunk, pTitle);
        Utils::b.progressMap["copyFile"] = double(iChunk+1) / double(nChunk);

        // buffering source file
        GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(1);
        in.read(contentBuffer.data(), bufferSize);
        timeLoad += GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(1);

        out.write(contentBuffer.data(), in.gcount());
        timeDrop += GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(1);
      }

//      GenericToolbox::displayProgressBar(nChunk, nChunk, pTitle);
//      std::cout << GET_VAR_NAME_VALUE(timeLoad) << " / " << GET_VAR_NAME_VALUE(timeDrop) << std::endl;

      Utils::b.progressMap["copyFile"] = 1.;
      return isSuccess;
    }
    static bool doFilesAreIdentical(const std::string& file1Path_, const std::string& file2Path_){
      if( not isFile(file1Path_) ) { return false; }
      if( not isFile(file2Path_) ) { return false; }

      ssize_t file1Size = getFileSize(file1Path_);
      if( file1Size != getFileSize(file2Path_) ) return false;
      if(p.useCrcCheck){
        std::ifstream file1(file1Path_, std::ios::in | std::ios::binary);
        std::ifstream file2(file2Path_, std::ios::in | std::ios::binary);

        auto bufferSize = ssize_t(file1Size/200); // 1 chunk per %
        bufferSize = std::min(bufferSize, ssize_t(GenericToolbox::Switch::IO::ParametersHolder::maxBufferSize)); // cap the buffer size -> not too big
        bufferSize = std::max(bufferSize, ssize_t(GenericToolbox::Switch::IO::ParametersHolder::minBufferSize)); // cap the buffer size -> not too small
        std::vector<u8> file1Buffer(bufferSize, 0);
        std::vector<u8> file2Buffer(bufferSize, 0);
        size_t nChunk = (size_t(file1Size)/bufferSize) + 1;
        Utils::b.progressMap["doFilesAreIdentical"] = double(1) / double(nChunk);

        auto file1Crc = crc32(0L, Z_NULL, 0);
        auto file2Crc = crc32(0L, Z_NULL, 0);

        for( size_t iChunk = 0 ; iChunk < nChunk ; iChunk++ ) {
          Utils::b.progressMap["doFilesAreIdentical"] = double(iChunk+1) / double(nChunk);

          // buffering source file
          file1.read(reinterpret_cast<char *>(file1Buffer.data()), bufferSize);
          file2.read(reinterpret_cast<char *>(file2Buffer.data()), bufferSize);

          // check read size
          if( file1.gcount() != file2.gcount() ) return false;

          // check crc
          file1Crc = crc32(file1Crc, file1Buffer.data(), file1.gcount());
          file2Crc = crc32(file2Crc, file2Buffer.data(), file2.gcount());
          if(file1Crc != file2Crc){ return false; }
        }
      }
      return true;
    }
  }

  namespace Utils{
    static bool isTidLike(const std::string& name_){
      // std::string tidExample = "010062601165E000";
      // 16 hex string that starts with '0'
      std::regex hexRegex("0[0-9a-fA-F]{15}");
      return std::regex_match(name_, hexRegex);
    }
    static std::string lookForTidInSubFolders(const std::string& folderPath_, int maxDepth_){
      // WARNING : Recursive function
      maxDepth_--;
      std::vector<std::string> subFolderList = GenericToolbox::lsDirs(folderPath_);

      for(auto &subFolder : subFolderList){
        if( isTidLike(subFolder) ) return subFolder;
      }

      if( maxDepth_ > 0 ){
        // if not found
        std::string tidCandidate;
        std::string path;
        for(auto &subFolder : subFolderList){
          tidCandidate = GenericToolbox::Switch::Utils::lookForTidInSubFolders( GenericToolbox::joinPath(folderPath_, subFolder), maxDepth_-1 );
          if(not tidCandidate.empty()){ return tidCandidate; }
        }
      }


      return "";
    }
    static uint8_t* getIconFromTitleId(const std::string& titleId_){
      if( titleId_.empty() ) return nullptr;

      uint8_t* icon = nullptr;
      NsApplicationControlData controlData;
      size_t controlSize  = 0;
      uint64_t tid;

      std::istringstream buffer(titleId_);
      buffer >> std::hex >> tid;

      if (R_FAILED(nsGetApplicationControlData(NsApplicationControlSource_Storage, tid, &controlData, sizeof(controlData), &controlSize))){ return nullptr; }

      icon = new uint8_t[0x20000];
      memcpy(icon, controlData.icon, 0x20000);
      return icon;
    }
  }

  namespace UI{
    static std::string openKeyboardUi(const std::string &defaultStr_) {
      SwkbdConfig kbd;
      char tmpoutstr[64];

      if (R_SUCCEEDED(swkbdCreate(&kbd, 0))) {
        swkbdConfigMakePresetDefault(&kbd);
        swkbdConfigSetInitialText(&kbd, defaultStr_.c_str());
        swkbdShow(&kbd, tmpoutstr, sizeof(tmpoutstr));
        swkbdClose(&kbd);
      }

      return {tmpoutstr};
    }
  }

  namespace Terminal {

    static void printRight(const std::string& input_, const std::string& color_, bool flush_){
      int nbSpaceLeft{GenericToolbox::getTerminalWidth()};
      nbSpaceLeft -= int(GenericToolbox::getPrintSize(input_));
      if( nbSpaceLeft <= 0 ){
        if( nbSpaceLeft == 0 ) nbSpaceLeft-=1;
        GenericToolbox::Switch::Terminal::printRight(
            input_.substr(0, input_.size() + nbSpaceLeft - int(flush_)), // remove extra char if flush
            color_,
            flush_
        );
        return;
      }

      if(flush_){ nbSpaceLeft-=1; std::cout << "\r"; }
      std::cout << color_ << GenericToolbox::repeatString(" ", nbSpaceLeft) << input_ << GenericToolbox::ColorCodes::resetColor;
      if(flush_) std::cout << "\r";
      else if(int(input_.size()) > GenericToolbox::getTerminalWidth()) std::cout << std::endl;
    }
    static void printLeft(const std::string& input_, const std::string& color_, bool flush_){
      int nbSpaceLeft{ GenericToolbox::getTerminalWidth() };
      nbSpaceLeft -= int(GenericToolbox::getPrintSize(input_));

      if( flush_ ){
        // can only flush if the terminal is not filled up
        nbSpaceLeft--;
        std::cout << "\r";
      }

      if( nbSpaceLeft < 0 ){
        GenericToolbox::Switch::Terminal::printLeft(
            input_.substr(0, input_.size() + nbSpaceLeft - int(flush_)), // remove extra char if flush
            color_,
            flush_
        );
        return;
      }

      std::cout << color_ << input_ << GenericToolbox::repeatString(" ", nbSpaceLeft) << GenericToolbox::ColorCodes::resetColor;
      if(flush_) std::cout << "\r";
    }
    static void printLeftRight(const std::string& inputLeft_, const std::string& inputRight_, const std::string& color_, bool flush_){
      int nbSpaceLeft{ GenericToolbox::getTerminalWidth() };
      nbSpaceLeft -= int( GenericToolbox::getPrintSize(inputLeft_) );
      nbSpaceLeft -= int( GenericToolbox::getPrintSize(inputRight_) );

      std::stringstream ss;

      if(nbSpaceLeft <= 0){ ss << inputLeft_.substr(0, inputLeft_.size() + nbSpaceLeft); }
      else{ ss << inputLeft_; }

      ss << GenericToolbox::repeatString(" ", nbSpaceLeft);
      ss << inputRight_;

      std::cout << color_ << ss.str() << GenericToolbox::ColorCodes::resetColor;
      if(flush_) std::cout << "\r";
      else{
        if( GenericToolbox::getTerminalWidth() < int(inputLeft_.size()) + int(inputRight_.size())) {
          std::cout << std::endl;
        }
      }
    }

    static void makePause(const std::string& message_){
      if( not message_.empty() ) std::cout << message_ << std::endl;
      std::cout << "+ to quit now or PRESS any button to continue." << std::endl;
      consoleUpdate(nullptr);

      PadState pad;
      padInitializeAny(&pad);

      // clear buttons?
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      while(appletMainLoop()){

        padUpdate( &pad );
        u64 kDown = padGetButtonsDown( &pad );

        if     ( kDown & HidNpadButton_Plus ) {
          consoleExit(nullptr);
          exit(EXIT_SUCCESS);
        }
        else if( kDown != 0 ) {
          consoleUpdate(nullptr);
          break; // break in order to return to hbmenu
        }
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      consoleUpdate(nullptr);
    }
    template<typename T, typename TT> static void displayProgressBar(
        const T& iCurrent_, const TT& iTotal_,
        const std::string &title_, bool forcePrint_,
        const std::string& color_
        ){
      if(forcePrint_ or ProgressBar::gProgressBar.template showProgressBar(iCurrent_, iTotal_) ){
        ProgressBar::gProgressBar.setDisableVt100Cmd( true );
        printRight(ProgressBar::gProgressBar.template generateProgressBarStr(iCurrent_, iTotal_, GenericToolbox::padString(title_, 40)), color_, true);
        consoleUpdate(nullptr);
      }
    }

  }

  namespace Hardware{
    static u64 getMemoryInfo(PhysicalMemoryType type_, PhysicalMemoryOf of_) {
      u64 out{0};
      svcGetSystemInfo(&out, type_.value, INVALID_HANDLE, of_.value);
      return out;
    }
    static std::string getMemoryUsageStr(PhysicalMemoryOf of_) {
      std::stringstream ss;
      ss << of_.toString() << ": ";
      ss << GenericToolbox::parseSizeUnits(double(getMemoryInfo(PhysicalMemoryType::UsedPhysicalMemorySize, of_)));
      ss << "/";
      ss << GenericToolbox::parseSizeUnits(double(getMemoryInfo(PhysicalMemoryType::TotalPhysicalMemorySize, of_)));
      return ss.str();
    }

    GT_DEPRECATED("use GenericToolbox::getTerminalWidth()") static int getTerminalWidth(){
      return GenericToolbox::getTerminalWidth();
    }
    GT_DEPRECATED("use GenericToolbox::getTerminalHeight()") static int getTerminalHeight(){
      return GenericToolbox::getTerminalHeight();
    }

  }

}

#endif


#endif // CPP_GENERIC_TOOLBOX_SWITCH_H
