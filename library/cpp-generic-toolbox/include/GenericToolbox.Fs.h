//
// Created by Nadrino on 23/12/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_FS_H
#define CPP_GENERIC_TOOLBOX_FS_H

// ***************************
//! Filesystem related tools
// ***************************

#include "GenericToolbox.String.h"

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <iosfwd>
#include <string>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"

#define USE_FILESYSTEM 0 // experimental
#if HAS_CPP_17 && USE_FILESYSTEM
#include <filesystem>
#endif


// Declaration section
namespace GenericToolbox{

  // -- no IO dependencies (string parsing)
  static bool hasExtension(const std::string &filePath_, const std::string &extension_);
  static std::string getExtension(const std::string& filePath_);
  static std::string getFolderPath(const std::string &filePath_);
  static std::string getFileName(const std::string &filePath_, bool withExtension_ = true);
  static std::string replaceExtension(const std::string& filePath_, const std::string& newExtension_);
  template<typename T1, typename T2> static std::string joinPath(const T1& str1_, const T2& str2_);
  template<typename T1, typename T2> static std::string joinPath(const std::vector<T1>& vec1_, const std::vector<T2>& vec2_);
  template<typename T, typename T2> static std::string joinPath(const std::vector<T>& vec_, const T2& str_);
  template<typename T1, typename T> static std::string joinPath(const T1& str_, const std::vector<T>& vec_);
  template<typename T> static std::string joinPath(const std::vector<T>& vec_);
  template<typename First, typename Second, typename... Args> static std::string joinPath(const First& first_, const Second& second_, const Args&... args_);

  // -- with direct IO dependencies
  static bool isPathValid(const std::string &filePath_);
  static bool isFile(const std::string &filePath_);
  static bool isDir(const std::string &dirPath_);
  static bool areSameFiles(const std::string &filePath1_, const std::string &filePath2_);
  static bool isDirEmpty(const std::string &dirPath_);

  // -- unix-like commands
  static bool mkdir(const std::string &dirPath_);
  static bool rm(const std::string &filePath_);
  static bool mv(const std::string &src_, const std::string &destination_, bool force_ = false);
  static bool cp(const std::string &src_, const std::string &destination_, bool force_ = false);
  static bool rmDir(const std::string &dirPath_); // works for empty dirs
  static std::vector<std::string> ls(const std::string &dirPath_, const std::string &entryNameRegex_ = "", int type_=-1, size_t maxEntries_ = 0);
  static std::vector<std::string> lsDirs(const std::string &dirPath_, const std::string &entryNameRegex_ = "", size_t maxEntries_ = 0);
  static std::vector<std::string> lsFiles(const std::string &dirPath_, const std::string &entryNameRegex_ = "", size_t maxEntries_ = 0);
  static std::vector<std::string> lsRecursive(const std::string &dirPath_, int type_ = -1);
  static std::vector<std::string> lsDirsRecursive(const std::string &dirPath_);
  static std::vector<std::string> lsFilesRecursive(const std::string &dirPath_);

  // -- read file
  static size_t getFileHash(const std::string &filePath_);
  static ssize_t getFileSize(const std::string& filePath_);
  static std::string dumpFileAsString(const std::string &filePath_);
  static std::vector<std::string> dumpFileAsVectorString(const std::string &filePath_, bool skipEmptyLines_=false);

  // -- write file
  static void dumpStringInFile(const std::string &outFilePath_, const std::string &stringToWrite_);

  // -- binary reader
  static void fillData( std::istream& file_, std::string& buffer_, size_t size_ );
  template<typename T> static void fillData( std::istream& file_, T& buffer_ );

  template<typename T> static void writeData( std::ofstream& file_, const T& buffer_ );
  template<> inline void writeData( std::ofstream& file_, const std::string& buffer_ );

}


// Implementation section
namespace GenericToolbox{

  // -- without IO dependencies (string parsing)
  static bool hasExtension(const std::string &filePath_, const std::string &extension_){
    return endsWith(filePath_, "." + extension_);
  }
  static std::string getExtension(const std::string& filePath_){
    if( filePath_.find_last_of('.') == size_t(-1) ) return {};
    return filePath_.substr(filePath_.find_last_of('.') + 1);
  }
  static std::string getFolderPath(const std::string &filePath_){
    if( filePath_.find_last_of("/\\") == size_t(-1) ) return {};
    return filePath_.substr(0,filePath_.find_last_of("/\\"));
  }
  static std::string getFileName(const std::string &filePath_, bool withExtension_){
#if HAS_CPP_17 && USE_FILESYSTEM
    std::filesystem::path pathObj(filePath_);
    return ( keepExtension_ ? pathObj.filename().string() : pathObj.stem().string());
#else
    const size_t pos = filePath_.find_last_of("/\\");
    const std::string filename = ( pos != std::string::npos ) ? filePath_.substr(pos + 1) : filePath_;
    return ( withExtension_ or filename.find('.') == std::string::npos ) ? filename : filename.substr(0, filename.find_last_of('.'));
#endif
  }
  static std::string replaceExtension(const std::string& filePath_, const std::string& newExtension_){
    if( newExtension_.empty() ) return filePath_.substr(0, filePath_.find_last_of('.'));
    return filePath_.substr(0, filePath_.find_last_of('.')) + "." + newExtension_;
  }
  template<typename T1, typename T2> static inline std::string joinPath(const T1& str1_, const T2& str2_){
    std::stringstream ss;
    ss << str1_;
    if( not ss.str().empty() ){ ss << "/"; }
    ss << str2_;
    // explicit type specification as `auto` is not converted into a std::string for GCC 4.8.5
    std::string out{ss.str()};
    GenericToolbox::removeRepeatedCharInsideInputStr( out, "/" );
    GenericToolbox::removeTrailingCharInsideInputStr( out, "/" );
    return out;
  }
  template<typename T1, typename T2> static inline std::string joinPath(const std::vector<T1>& vec1_, const std::vector<T2>& vec2_){
    return GenericToolbox::joinPath(GenericToolbox::joinPath(vec1_), GenericToolbox::joinPath(vec2_));
  }
  template<typename T, typename T2> static inline std::string joinPath(const std::vector<T>& vec_, const T2& str_){
    return GenericToolbox::joinPath(GenericToolbox::joinPath(vec_), str_);
  }
  template<typename T1, typename T> static inline std::string joinPath(const T1& str_, const std::vector<T>& vec_){
    return GenericToolbox::joinPath(str_, GenericToolbox::joinPath(vec_));
  }
  template<typename T> static inline std::string joinPath(const std::vector<T>& vec_){
    std::string out;
    for( auto& elm : vec_ ){ out = GenericToolbox::joinPath(out, elm); }
    return out;
  }
  template<typename First, typename Second, typename... Args> static inline std::string joinPath(const First& first_, const Second& second_, const Args&... args_){
    // unfold to binary expression
    auto out{joinPath(first_, joinPath(second_, args_...))};
    return out;
  }

  // -- with direct IO dependencies
  static inline bool isPathValid(const std::string &filePath_){
    return ( access( filePath_.c_str(), F_OK ) == 0 );
  }
  static inline bool isFile(const std::string &filePath_){
    struct stat info{};
    if( lstat(filePath_.c_str(), &info) != 0 ){ return false; /* Error occurred */ }
    return S_ISREG(info.st_mode);
  }
  static inline bool isDir(const std::string &dirPath_){
    struct stat info{};
    if( lstat(dirPath_.c_str(), &info) != 0 ){ return false; /* Error occurred */ }
    return S_ISDIR(info.st_mode);
  }
  static inline bool areSameFiles(const std::string &filePath1_, const std::string &filePath2_){

    if( not isFile(filePath1_) ) return false;
    if( not isFile(filePath2_) ) return false;

    std::ifstream fileStream1(filePath1_);
    std::ifstream fileStream2(filePath2_);

    // Buffer size 1 Megabyte (or any number you like)
    size_t buffer_size = 1<<20;
    char *buffer1 = new char[buffer_size];
    char *buffer2 = new char[buffer_size];

    std::hash<std::string> hashBuffer1;
    std::hash<std::string> hashBuffer2;

    while (fileStream1 and fileStream2) {
      // Try to read next chunk of data
      fileStream1.read(buffer1, long(buffer_size));
      fileStream2.read(buffer2, long(buffer_size));

      // Get the number of bytes actually read
      if(fileStream1.gcount() != fileStream2.gcount()){
        return false;
      }

      size_t count = fileStream1.gcount();
      // If nothing has been read, break
      if( count == 0 ){
        break;
      }

      // Compare hash files
      if(hashBuffer1(buffer1) != hashBuffer2(buffer2))
        return false;

    }

    delete[] buffer1;
    delete[] buffer2;

    return true;
  }
  static inline bool isDirEmpty(const std::string &dirPath_){
    if( not isDir(dirPath_) ){ return false; }
    return lsRecursive(dirPath_).empty();
  }

  // -- unix-like commands
  static inline bool mkdir(const std::string &dirPath_){
    bool result = false;
    if( isDir(dirPath_) ){ return true; }

    std::string current_level;
    std::string level;
    std::stringstream ss(dirPath_);

    // split path using slash as a separator
    while (std::getline(ss, level, '/')){
      current_level += level; // append folder to the current level
      if(current_level.empty()) current_level = "/";
      current_level = removeRepeatedCharacters(current_level, "/");
      // create current level
      if( not isDir(current_level) ){
        ::mkdir(current_level.c_str(), 0777);
        result = true;
      }
      current_level += "/"; // don't forget to append a slash
    }

    return result;
  }
  static inline bool rm(const std::string &filePath_){
    // not using ::delete as if the specified file path is a symbolic link,
    // it deletes the link and the file or directory that the link refers to

    // ::unlink: If the specified file path is a symbolic link,
    // only the link itself is deleted, not the file or directory that the link refers to
    return ( ::unlink(filePath_.c_str()) == 0 );
  }
  static inline bool mv(const std::string &src_, const std::string &destination_, bool force_){
    if( not isPathValid(src_) ){ return false; }

    if( isFile(destination_) ){
      if(force_){ rm(destination_); }
      else{ return false; }
    }
    else{
      std::string destination_folder_path = getFolderPath(destination_);
      if(not isFile(destination_folder_path)){ mkdir(destination_folder_path); }
    }

    return (std::rename(src_.c_str(), destination_.c_str()) == 0);
  }
  static inline bool cp(const std::string &src_, const std::string &destination_, bool force_){
    if( not isFile(src_) ){ return false; }
    if( isFile(destination_) ){
      if( force_ ){ rm(destination_); }
      else{ return false; }
    }

    std::ifstream  src(src_, std::ios::binary);
    std::ofstream  dst(destination_, std::ios::binary);

    dst << src.rdbuf();

    return true;
  }
  static inline bool rmDir(const std::string &dirPath_){
    return (::rmdir(dirPath_.c_str()) == 0);
  }
  static inline std::vector<std::string> ls(const std::string &dirPath_, const std::string &entryNameRegex_, int type_, size_t maxEntries_) {
    if( not isDir( dirPath_ ) ) return {};

    DIR* directory;
    directory = opendir(dirPath_.c_str()); //Open current-working-directory.
    if( directory == nullptr ){ return {}; }

    std::vector<std::string> nameElements;
    if(not entryNameRegex_.empty()){ nameElements = GenericToolbox::splitString(entryNameRegex_, "*"); }

    struct dirent* entry;
    std::vector<std::string> subFoldersList;
    while ( ( entry = readdir(directory) ) ) {
      if( type_ != -1 and entry->d_type != type_ ){ continue; }
      if( strcmp(entry->d_name, ".") == 0 or strcmp(entry->d_name, "..") == 0 ){ continue; }

      if(not entryNameRegex_.empty()){
        std::string entryCandidate = entry->d_name;

        bool isValid{true};
        for( size_t iElement = 0 ; iElement < nameElements.size() ; iElement++ ){
          if(nameElements[iElement].empty()) continue;

          if( iElement == 0 ){
            if( not GenericToolbox::startsWith(entryCandidate, nameElements[iElement]) ){
              isValid = false;
              break;
            }
          }
          else if( iElement+1 == nameElements.size() ){
            if(not GenericToolbox::endsWith(entryCandidate, nameElements[iElement]) ){
              isValid = false;
            }
          }
          else{
            if( not GenericToolbox::hasSubStr(entryCandidate, nameElements[iElement])
                ){
              isValid = false;
              break;
            }
          }

          if( iElement+1 != nameElements.size() ){
            entryCandidate = GenericToolbox::splitString(entryCandidate, nameElements[iElement]).back();
          }
        }
        if( not isValid ) continue;
      }
      subFoldersList.emplace_back(entry->d_name);
      if( maxEntries_ != 0 and subFoldersList.size() >= maxEntries_ ){ break; }
    }
    closedir(directory);
    return subFoldersList;
  }
  static inline std::vector<std::string> lsDirs(const std::string &dirPath_, const std::string &entryNameRegex_, size_t maxEntries_) {
    return GenericToolbox::ls(dirPath_, entryNameRegex_, DT_DIR, maxEntries_);
  }
  static inline std::vector<std::string> lsFiles(const std::string &dirPath_, const std::string &entryNameRegex_, size_t maxEntries_){
    return GenericToolbox::ls(dirPath_, entryNameRegex_, DT_REG, maxEntries_);
  }
  static inline std::vector<std::string> lsRecursive(const std::string &dirPath_, int type_){
    // WARNING : Recursive function

    // first, get the files in this folder
    std::vector<std::string> out;

    // then walk in sub-folders
    auto subFolderList = GenericToolbox::lsDirs( dirPath_ );
    for(auto &subFolder : subFolderList ){

      // recursive ////////
      auto subFileList = GenericToolbox::lsRecursive( GenericToolbox::joinPath(dirPath_, subFolder), type_ );
      /////////////////////

      out.reserve( out.size() + subFileList.size() );
      for(auto &subFile : subFileList ){
        out.emplace_back( GenericToolbox::joinPath(subFolder, subFile) );
      }
    }

    auto entries = GenericToolbox::ls(dirPath_, "", type_);
    out.reserve( out.size() + entries.size() );
    for( auto& entry : entries ){ out.emplace_back( entry ); }

    return out;
  }
  static inline std::vector<std::string> lsDirsRecursive(const std::string &dirPath_){ return lsRecursive(dirPath_, DT_DIR); }
  static inline std::vector<std::string> lsFilesRecursive(const std::string &dirPath_) { return lsRecursive(dirPath_, DT_REG); }

  // -- read file
  static inline size_t getFileHash(const std::string &filePath_) {
    std::hash<std::string> hashString;
    return hashString(dumpFileAsString(filePath_));
  }
  static inline ssize_t getFileSize(const std::string& filePath_){
    struct stat st{};
    stat(filePath_.c_str(), &st);
    return ssize_t(st.st_size);
  }
  static inline std::string dumpFileAsString(const std::string &filePath_){
    if( not isFile(filePath_) ){ return {}; }
    std::string data;
    std::ifstream input_file(filePath_.c_str(), std::ios::binary | std::ios::in );
    std::ostringstream ss;
    ss << input_file.rdbuf();
    data = ss.str();
    input_file.close();
    return data;
  }
  static inline std::vector<std::string> dumpFileAsVectorString(const std::string &filePath_, bool skipEmptyLines_){

    if( not isFile( filePath_ ) ){ return {}; }

    std::ifstream file( filePath_ );
    if( not file.is_open() ){ return {}; }

    std::vector<std::string> lines;

    std::string lineBuffer{};
    while( std::getline(file, lineBuffer) ){
      lines.emplace_back( lineBuffer );
    }

    return lines;
  }

  // -- write file
  static inline void dumpStringInFile(const std::string &outFilePath_, const std::string &stringToWrite_){
    std::ofstream out(outFilePath_.c_str());
    out << stringToWrite_;
    out.close();
  }

  // -- binary reader
  template<typename T> static inline void fillData( std::istream& file_, T& buffer_ ){
    file_.read( reinterpret_cast<char*>(&buffer_), sizeof(T) );
  }
  template<typename T> static inline void fillData( std::istream& file_, T* buffer_, size_t size_ ){
    file_.read( reinterpret_cast<char*>(buffer_), sizeof(T)*size_ );
  }
  template<typename T, size_t N> static inline void fillData( std::istream& file_, std::array<T, N>& buffer_ ){
    file_.read( reinterpret_cast<char*>(&buffer_[0]), sizeof(T)*N );
  }
  template<typename T> static inline void writeData( std::ofstream& file_, const T& buffer_ ){
    file_.write( reinterpret_cast<const char*>(&buffer_), sizeof(T) );
  }
  template<> inline void writeData( std::ofstream& file_, const std::string& buffer_ ){
    file_.write( &buffer_[0], long(buffer_.size()) );
  }
  static inline void fillData( std::istream& file_, std::string& buffer_, size_t size_ ){
    buffer_.clear();
    buffer_.resize(size_);
    file_.read( &buffer_[0], long(size_) );
  }

  // Deprecated
  GT_DEPRECATED("renamed: isPathValid") static bool doesPathIsValid(const std::string &filePath_){
    return isPathValid(filePath_);
  }
  GT_DEPRECATED("renamed: isFile") static bool doesPathIsFile(const std::string &filePath_){
    return isFile(filePath_);
  }
  GT_DEPRECATED("renamed: isDir") static bool doesPathIsFolder(const std::string &dirPath_){
    return isDir(dirPath_);
  }
  GT_DEPRECATED("renamed: areSameFiles") static bool doFilesAreTheSame(const std::string &filePath1_, const std::string &filePath2_){
    return areSameFiles(filePath1_, filePath2_);
  }

  GT_DEPRECATED("renamed: mkdir") static bool mkdirPath(const std::string &dirPath_){
    return mkdir(dirPath_);
  }
  GT_DEPRECATED("renamed: rm") static bool deleteFile(const std::string &filePath_){
    return rm(filePath_);
  }
  GT_DEPRECATED("renamed: mv") static bool mvFile(const std::string &src_, const std::string &destination_, bool force_ = false){
    return mv(src_, destination_, force_);
  }
  GT_DEPRECATED("renamed: cp") static bool copyFile(const std::string &src_, const std::string &destination_, bool force_ = false){
    return cp(src_, destination_, force_);
  }
  GT_DEPRECATED("renamed: rmDir") static bool deleteEmptyDirectory(const std::string &dirPath_){
    return rmDir(dirPath_);
  }

  GT_DEPRECATED("renamed: isDirEmpty") static bool doesFolderIsEmpty(const std::string &dirPath_){
    return isDirEmpty(dirPath_);
  }

  GT_DEPRECATED("renamed: hasExtension") static bool doesFilePathHasExtension(const std::string &filePath_, const std::string &extension_){
    return hasExtension(filePath_, extension_);
  }
  GT_DEPRECATED("renamed: getExtension") static std::string getFileExtension(const std::string& filePath_){
    return getExtension(filePath_);
  }
  GT_DEPRECATED("renamed: getFolderPath") static std::string getFolderPathFromFilePath(const std::string &filePath_){ return getFolderPath(filePath_); }
  GT_DEPRECATED("renamed: getFileName") static std::string getFileNameFromFilePath(const std::string &filePath_, bool keepExtension_ = true){
    return getFileName(filePath_, keepExtension_);
  }
  GT_DEPRECATED("renamed: replaceExtension") static std::string replaceFileExtension(const std::string& filePath_, const std::string& newExtension_){
    return replaceExtension(filePath_, newExtension_);
  }

  GT_DEPRECATED("renamed: getFileSize") static long int getFileSizeInBytes(const std::string &filePath_){
    return getFileSize(filePath_);
  }
  GT_DEPRECATED("renamed: ls") static std::vector<std::string> getListOfEntriesInFolder(const std::string &dirPath_, const std::string &entryNameRegex_ = "", int type_=-1, size_t maxEntries_ = 0){
    return ls(dirPath_, entryNameRegex_, type_, maxEntries_);
  }
  GT_DEPRECATED("renamed: lsDirs") static std::vector<std::string> getListOfSubFoldersInFolder(const std::string &dirPath_, const std::string &entryNameRegex_ = "", size_t maxEntries_ = 0){
    return lsDirs(dirPath_, entryNameRegex_, maxEntries_);
  }
  GT_DEPRECATED("renamed: lsFiles") static std::vector<std::string> getListOfFilesInFolder(const std::string &dirPath_, const std::string &entryNameRegex_ = "", size_t maxEntries_ = 0){
    return lsFiles(dirPath_, entryNameRegex_, maxEntries_);
  }
  GT_DEPRECATED("renamed: lsRecursive") static std::vector<std::string> getListOfEntriesInSubFolders(const std::string &dirPath_, int type_ = -1){
    return lsRecursive(dirPath_, type_);
  }
  GT_DEPRECATED("renamed: lsDirsRecursive") static std::vector<std::string> getListOfFoldersInSubFolders(const std::string &dirPath_){
    return lsDirsRecursive(dirPath_);
  }
  GT_DEPRECATED("renamed: lsFilesRecursive") static std::vector<std::string> getListOfFilesInSubFolders(const std::string &dirPath_){
    return lsFilesRecursive(dirPath_);
  }


}



#endif // CPP_GENERIC_TOOLBOX_FS_H
