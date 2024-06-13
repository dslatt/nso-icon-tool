//
// Created by Nadrino on 23/12/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_STRING_H
#define CPP_GENERIC_TOOLBOX_STRING_H

// ***************************
//! String related tools
// ***************************

#include "GenericToolbox.Macro.h"

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <cmath>
#include <regex>
#include <map>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"


// Declaration section
namespace GenericToolbox{

  namespace ColorCodes{
#ifndef CPP_GENERIC_TOOLBOX_NOCOLOR
    // reset
    static const char* resetColor{"\x1b[0m"};

    // background color
    static const char* redBackground{"\x1b[41m"};
    static const char* greenBackground{"\x1b[42m"};
    static const char* yellowBackground{"\x1b[43m"};
    static const char* blueBackground{"\x1b[44m"};
    static const char* magentaBackground{"\x1b[45m"};
    static const char* cyanBackground{"\x1b[46m"};
    static const char* greyBackground{"\x1b[47m"};

    // text color
    static const char* redText{"\x1b[1;31m"};
    static const char* greenText{"\x1b[1;32m"};
    static const char* yellowText{"\x1b[1;33m"};
    static const char* blueText{"\x1b[1;34m"};
    static const char* magentaText{"\x1b[1;35m"};
    static const char* cyanText{"\x1b[1;36m"};
    static const char* whiteText{"\x1b[1;37m"};

    // lighter text color
    static const char* redLightText{"\x1b[1;91m"};
    static const char* greenLightText{"\x1b[1;92m"};
    static const char* yellowLightText{"\x1b[1;93m"};
    static const char* blueLightText{"\x1b[1;94m"};
    static const char* magentaLightText{"\x1b[1;95m"};
    static const char* cyanLightText{"\x1b[1;96m"};
    static const char* whiteLightText{"\x1b[1;97m"};
    static const char* greyText{"\x1b[1;90m"};
#else
    // reset
    static const char* resetColor{};

    // background color
    static const char* redBackground{};
    static const char* greenBackground{};
    static const char* yellowBackground{};
    static const char* blueBackground{};
    static const char* magentaBackground{};
    static const char* cyanBackground{};
    static const char* greyBackground{};

    // text color
    static const char* redText{};
    static const char* greenText{};
    static const char* yellowText{};
    static const char* blueText{};
    static const char* magentaText{};
    static const char* cyanText{};
    static const char* whiteText{};

    static const char* redLightText{};
    static const char* greenLightText{};
    static const char* yellowLightText{};
    static const char* blueLightText{};
    static const char* magentaLightText{};
    static const char* cyanLightText{};
    static const char* whiteLightText{};
    static const char* greyText{};
#endif
    static const std::array<const char*, 6> rainbowColorList{redText, greenText, yellowText, blueText, magentaText, cyanText};
  }

  // -- Quick checks
  static bool hasSubStr(const std::string& str_, const std::string& subStr_, bool ignoreCase_ = false);
  static bool startsWith(const std::string& str_, const std::string& subStr_, bool ignoreCase_ = false);
  static bool endsWith(const std::string& str_, const std::string& subStr_, bool ignoreCase_ = false);

  // -- Conversion Tools
  template<typename T, typename TT> static std::string toString(const T& iterable_, const TT& toStrFct_, bool jumpLine_=true, bool indentLine_=true);
  template<typename T> static std::string toString(const std::vector<T> &vector_, bool jumpLine_ = false, bool indentLine_ = false);
  template<> std::string toString(const std::vector<std::string> &vector_, bool jumpLine_, bool indentLine_);
  template<> std::string toString(const std::vector<unsigned char> &vector_, bool jumpLine_, bool indentLine_);
  template<> std::string toString(const std::vector<char> &vector_, bool jumpLine_, bool indentLine_);
  template<typename T, size_t N> static std::string toString(const std::array<T, N>& array_);
  template <typename T1, typename T2> static std::string toString(const std::map<T1, T2>& map_, bool enableLineJump_ = true);
  template <typename T1, typename T2, typename T3> static std::string toString(const std::map<T1, std::pair<T2,T3>>& map_, bool enableLineJump_ = true);

  // -- Transformations
  static std::string toLowerCase(const std::string &inputStr_);

  static std::string stripUnicode(const std::string &inputStr_);

  static std::string stripBracket(const std::string &inputStr_, char bra_, char ket_, bool allowUnclosed_ = true, std::vector<std::string>* argBuffer_ = nullptr);
  static std::string repeatString(const std::string &inputStr_, int amount_);
  static std::string trimString(const std::string &inputStr_, const std::string &strToTrim_);
  static std::string padString(const std::string& inputStr_, unsigned int padSize_, const char& padChar = ' ');
  static std::string indentString(const std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar = " ");
  static std::string removeRepeatedCharacters(const std::string& inputStr_, const std::string &repeatedChar_);
  template<typename T> static std::string joinVectorString(const std::vector<T> &stringList_, const std::string &delimiter_, int beginIndex_ = 0, int endIndex_ = 0);
  template<typename... Args> static std::string joinAsString(const std::string &delimiter_, const Args&... args);
  static std::string replaceSubstringInString(const std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_);
  static std::string replaceSubstringInString(const std::string &input_str_, const std::vector<std::string> &substr_to_look_for_, const std::vector<std::string> &substr_to_replace_);
  static std::vector<std::string> splitString(const std::string& inputString_, const std::string &delimiter_, bool removeEmpty_ = false);

  // -- Transformations (Fast)
  static void replaceSubstringInsideInputString(std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_);
  static void removeRepeatedCharInsideInputStr(std::string &inputStr_, const std::string &doubledChar_);
  static void removeTrailingCharInsideInputStr(std::string &inputStr_, const std::string &trailingChar_);
  static void indentInputString(std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar = " ");
  static void trimInputString(std::string &inputStr_, const std::string &strToTrim_);

  // -- Parsing
  static size_t getPrintSize(const std::string& str_);
  static size_t getNLines(const std::string& str_);
  static std::string parseDecimalValue(double val_, const std::string& format_="%s%s", bool allowSubOne_ = true);
  static std::string parseUnitPrefix(double val_, int maxPadSize_=-1);
  static std::string parseSizeUnits(double sizeInBytes_);
  static std::string formatString( const std::string& strToFormat_ ); // overrider: make sure this is the one used when no extra args are provided.
  template<typename ... Args> static std::string formatString(const std::string& strToFormat_, Args ... args );


  // -- Representations
  static std::string toHex(const void* address_, size_t nBytes_);
  template<typename T> static std::string toHex(const T& val_);
  template<typename T> static std::string stackToHex(const std::vector<T> &rawData_, size_t stackSize_);
  static bool toBool(const std::string& str);
  static std::string parseIntAsString(int intToFormat_);

  // -- Aesthetic
  static std::string addUpDownBars(const std::string& str_, bool stripUnicode_ = true);
  static std::string highlightIf(bool condition_, const std::string& text_);
  static std::string makeRainbowString(const std::string& inputStr_, bool stripUnicode_ = true);

}


// Implementation section
namespace GenericToolbox {

  // -- Quick checks
  static bool hasSubStr(const std::string& str_, const std::string& subStr_, bool ignoreCase_) {
    if( subStr_.empty() ){ return true; }
    if( ignoreCase_ ){ return toLowerCase(str_).find( toLowerCase( subStr_ ) ) != std::string::npos; }
    return str_.find( subStr_ ) != std::string::npos;
  }
  static bool startsWith(const std::string& str_, const std::string& subStr_, bool ignoreCase_) {
    if( subStr_.empty() ){ return true; }
    if( subStr_.size() > str_.size() ){ return false; }
    if( ignoreCase_ ){
      std::string subStrLower{toLowerCase(subStr_)};
      return std::equal( subStrLower.begin(), subStrLower.end(), toLowerCase(str_).begin() );
    }
    return std::equal( subStr_.begin(), subStr_.end(), str_.begin() );
  }
  static bool endsWith(const std::string& str_, const std::string& subStr_, bool ignoreCase_) {
    if( subStr_.empty() ){ return true; }
    if( subStr_.size() > str_.size() ){ return false; }
    if( ignoreCase_ ){
      std::string subStrLower{toLowerCase(subStr_)};
      return std::equal( subStrLower.begin(), subStrLower.end(), toLowerCase(str_).end() - long(subStrLower.size()) );
    }
    return std::equal( subStr_.begin(), subStr_.end(), str_.end() - long(subStr_.size()) );
  }

  // -- Aesthetic
  static std::string addUpDownBars(const std::string& str_, bool stripUnicode_){
    std::stringstream ss;
    size_t strLength = str_.size();
    if( stripUnicode_ ) strLength = GenericToolbox::stripUnicode(str_).size();
    std::string bar = GenericToolbox::repeatString("─", int(strLength));
    ss << bar << std::endl << str_ << std::endl << bar;
    return ss.str();
  }
  static std::string highlightIf(bool condition_, const std::string& text_){
    std::stringstream ss;
    ss << (condition_ ? ColorCodes::redBackground : "" );
    ss << text_;
    ss << ( condition_ ? ColorCodes::resetColor : "" );
    return ss.str();
  }
  static std::string makeRainbowString(const std::string& inputStr_, bool stripUnicode_){
    std::string outputString;
    std::string inputStrStripped;
    stripUnicode_ ? inputStrStripped = GenericToolbox::stripUnicode(inputStr_) : inputStrStripped = inputStr_;
    double nbCharsPerColor = double(inputStrStripped.size()) / double(ColorCodes::rainbowColorList.size());
    int colorSlot{0};
    for( size_t iChar = 0 ; iChar < inputStrStripped.size() ; iChar++ ){
      if( nbCharsPerColor < 1 or iChar == 0 or ( int(iChar+1) / nbCharsPerColor) - colorSlot + 1 > 1 ){
        outputString += ColorCodes::rainbowColorList[colorSlot++];
      }
      outputString += inputStrStripped[iChar];
    }
    outputString += ColorCodes::resetColor;
    return outputString;
  }

  static std::string toLowerCase(const std::string &inputStr_) {
    std::string output_str(inputStr_);
    std::transform(output_str.begin(), output_str.end(), output_str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return output_str;
  }
  static std::string stripUnicode(const std::string &inputStr_){
    std::string outputStr(inputStr_);

    if( GenericToolbox::hasSubStr(outputStr, "\033") ){
      // remove color
      std::string tempStr;
      auto splitOutputStr = GenericToolbox::splitString(outputStr, "\033");
      for(const auto& sliceStr : splitOutputStr){
        if(sliceStr.empty()) continue;
        if(tempStr.empty()){
          tempStr = sliceStr;
          continue;
        }
        // look for a 'm' char that determines the end of the color code
        bool mCharHasBeenFound = false;
        for(const char& c : sliceStr){
          if(not mCharHasBeenFound){
            if(c == 'm'){
              mCharHasBeenFound = true;
            }
          }
          else{
            tempStr += c;
          }
        }
      }
      outputStr = tempStr;
    }

    outputStr.erase(
        remove_if(
            outputStr.begin(), outputStr.end(),
            [](const char& c){return !isprint( static_cast<unsigned char>( c ) );}
        ),
        outputStr.end()
    );

    return outputStr;
  }

  static std::string stripBracket(const std::string &inputStr_, char bra_, char ket_, bool allowUnclosed_, std::vector<std::string>* argBuffer_){
    size_t iChar{0}; std::string out;
    char currentChar;
    while( iChar < inputStr_.size() ){

      // buffering
      currentChar = inputStr_[iChar++];

      // opening bracket?
      if ( currentChar == bra_ ){
        if(argBuffer_!= nullptr){ argBuffer_->emplace_back(); }

        int nestedLevel{0};
        while( iChar < inputStr_.size() ){

          // buffering
          currentChar = inputStr_[iChar++];

          // opening a nested bracket?
          if( currentChar == bra_ ) { nestedLevel++; }

          // closing a bracket?
          if( currentChar == ket_ ) {
            if( nestedLevel == 0 ){ break; } // closing the bracket -> safely leave the loop
            else                  { nestedLevel--; } // closing a nested bracket
          }

          // buffering bracket content
          if(argBuffer_!= nullptr){ argBuffer_->back() += currentChar; }

          // sanity check -> this loop is supposed to be left with "break;"
          if( iChar == inputStr_.size() and not allowUnclosed_ ){
            throw std::runtime_error("unclosed bracket: " + inputStr_);
          }
        }
      }
      else{ out += currentChar; }
    }

//    std::cout << inputStr_ << " -> " << out;
//    if(argBuffer_!= nullptr) std::cout << " args:" << GenericToolbox::toString(*argBuffer_);
//    std::cout << std::endl;

    return out;
  }

#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 9))
#else
  namespace StringManagementUtils{
    static std::regex ansiRegex("\033((\\[((\\d+;)*\\d+)?[A-DHJKMRcfghilmnprsu])|\\(|\\))");
  }
#endif
  static size_t getPrintSize(const std::string& str_){
    if( str_.empty() ) return 0;
#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 9))
    // this is gcc 4.8 or earlier
// std::regex support is buggy, so don't use in this block
  return str_.size();
#else
// this is gcc 4.9 or later, or other compilers like clang
// hopefully std::regex support is ok here
    std::string::iterator::difference_type result = 0;
    std::for_each(
        std::sregex_token_iterator(str_.begin(), str_.end(), StringManagementUtils::ansiRegex, -1),
        std::sregex_token_iterator(),
        [&result](std::sregex_token_iterator::value_type const& e) {
          std::string tmp(e);
          result += std::count_if(tmp.begin(), tmp.end(), ::isprint);
        }
    );
    return result;
#endif
  }

  static size_t getNLines(const std::string& str_){
    if( str_.empty() ) return 0;
    return std::count(str_.begin(), str_.end(), '\n')+1;
  }
  static std::string repeatString(const std::string &inputStr_, int amount_){
    std::string outputStr;
    if(amount_ <= 0) return outputStr;
    for(int i_count = 0 ; i_count < amount_ ; i_count++){
      outputStr += inputStr_;
    }
    return outputStr;
  }
  static std::string trimString(const std::string &inputStr_, const std::string &strToTrim_){
    std::string outputStr(inputStr_);
    GenericToolbox::trimInputString( outputStr, strToTrim_ );
    return outputStr;
  }
  static std::string padString(const std::string& inputStr_, unsigned int padSize_, const char& padChar){
    std::string outputString;
//    int padDelta = int(inputStr_.size()) - int(padSize_);
    int padDelta = int(GenericToolbox::getPrintSize(inputStr_)) - int(padSize_);
    while( padDelta < 0 ){
      // add extra chars if needed
      outputString += padChar;
      padDelta++;
    }
    outputString += inputStr_;
    return outputString.substr(0, outputString.size() - padDelta);
  }
  static std::string indentString(const std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar){
    std::string outStr = inputStr_;
    GenericToolbox::indentInputString(outStr, indentCount_, indentChar);
    return outStr;
  }
  static std::string removeRepeatedCharacters(const std::string &inputStr_, const std::string &repeatedChar_) {
    std::string outStr = inputStr_;
    GenericToolbox::removeRepeatedCharInsideInputStr(outStr, repeatedChar_);
    return outStr;
  }
  template<typename T> std::string joinVectorString(const std::vector<T> &stringList_, const std::string &delimiter_, int beginIndex_, int endIndex_) {
    std::stringstream ss;
    if( endIndex_ == 0 ) endIndex_ = int(stringList_.size());

    // circular permutation -> python style : tab[-1] = tab[tab.size - 1]
    if( endIndex_ < 0 and int(stringList_.size()) > std::abs(endIndex_) ) {
      endIndex_ = int(stringList_.size()) + endIndex_;
    }

    for(int iElm = beginIndex_ ; iElm < endIndex_; iElm++ ) {
      if( iElm > beginIndex_ ) ss << delimiter_;
      ss << stringList_[iElm];
    }

    return ss.str();
  }
  template<typename... Args> static std::string joinAsString(const std::string& delimiter, const Args&... args) {
    std::stringstream ss;
    int dummy[] = {0, ((void)(ss << args << delimiter), 0)...};
    (void)dummy;  // Avoid unused variable warning
    std::string result = ss.str();
    if( not result.empty() ) { result.resize(result.size() - delimiter.size()); } // remove the last delimiter
    return result;
  }

  std::string replaceSubstringInString(const std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_) {
    std::string stripped_str = input_str_;
    GenericToolbox::replaceSubstringInsideInputString(stripped_str, substr_to_look_for_, substr_to_replace_);
    return stripped_str;
  }
  std::string replaceSubstringInString(const std::string &input_str_, const std::vector<std::string> &substr_to_look_for_, const std::vector<std::string> &substr_to_replace_){
    std::string stripped_str = input_str_;
    if(substr_to_look_for_.size() != substr_to_replace_.size()){
      throw std::runtime_error("vec size mismatch btw substr_to_look_for_(" + std::to_string(substr_to_look_for_.size()) + ") != substr_to_replace_(" + std::to_string(substr_to_replace_.size()) + ")");
    }
    for( size_t iSub = 0 ; iSub < substr_to_look_for_.size() ; iSub++ ){
      GenericToolbox::replaceSubstringInsideInputString(stripped_str, substr_to_look_for_[iSub], substr_to_replace_[iSub]);
    }
    return input_str_;
  }
  static std::string parseDecimalValue(double val_, const std::string& format_, bool allowSubOne_){
    std::stringstream ss;

    // flip the sign
    if( val_ < 0 ){ ss << "-"; val_ = -val_; }

//    auto f = [](double val, const std::vector<>){
//
//    };
//
//    ss << formatString(format_, );

    return ss.str();
  }
  std::string parseUnitPrefix(double val_, int maxPadSize_){
    std::stringstream ss;

    if( val_ < 0 ){
      ss << "-";
      val_ = -val_;
      maxPadSize_--;
    }

    if(maxPadSize_ > -1){
      ss << std::setprecision(maxPadSize_-1);
    }

    auto reducedVal = size_t(std::abs(val_));
    if( reducedVal > 0 ){
      if     ( (reducedVal = (reducedVal / 1000)) == 0 ){
        ss << val_;
      }
      else if( (reducedVal = (reducedVal / 1000)) == 0 ){
        ss << val_/1E3 << "K";
      }
      else if( (reducedVal = (reducedVal / 1000)) == 0 ){
        ss << val_/1E6 << "M";
      }
      else if( (reducedVal = (reducedVal / 1000)) == 0 ){
        ss << val_/1E9 << "G";
      }
      else if( (reducedVal = (reducedVal / 1000)) == 0 ){
        ss << val_/1E12 << "T";
      }
      else {
        ss << val_/1E15 << "P";
      }
    } // K, M, G, T, P
    else{
      if( val_ < 1E-3 ){ // force scientific notation
        ss << std::scientific << val_;
      }
      else{
        ss << val_;
      }
    }


    return ss.str();
  }
  std::string parseSizeUnits(double sizeInBytes_){
    return parseUnitPrefix(sizeInBytes_) + "B";
  }
  static std::vector<std::string> splitString(const std::string &inputString_, const std::string &delimiter_, bool removeEmpty_) {

    std::vector<std::string> outputSliceList;

    const char *src = inputString_.c_str();
    const char *next = src;

    std::string out_string_piece;

    while ((next = std::strstr(src, delimiter_.c_str())) != nullptr) {
      out_string_piece = "";
      while (src != next) {
        out_string_piece += *src++;
      }
      outputSliceList.emplace_back(out_string_piece);
      /* Skip the delimiter_ */
      src += delimiter_.size();
    }

    /* Handle the last token */
    out_string_piece = "";
    while (*src != '\0')
      out_string_piece += *src++;

    outputSliceList.emplace_back(out_string_piece);

    if(not removeEmpty_){
      return outputSliceList;
    }
    else{
      std::vector<std::string> strippedOutput;
      for(const auto& slice : outputSliceList){
        if(not slice.empty()){
          strippedOutput.emplace_back(slice);
        }
      }
      return strippedOutput;
    }


  }
  static std::string formatString( const std::string& strToFormat_ ){
    return strToFormat_;
  }
  template<typename ... Args> std::string formatString(const std::string& strToFormat_, Args ... args) {
    size_t size = snprintf(nullptr, 0, strToFormat_.c_str(), args ...) + 1; // Extra space for '\0'
    if (size <= 0) { throw std::runtime_error("Error during formatting."); }
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, strToFormat_.c_str(), args ...);
    return {buf.get(), buf.get() + size - 1}; // We don't want the '\0' inside
  }

  static void replaceSubstringInsideInputString(std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_){
    size_t index = 0;
    while ((index = input_str_.find(substr_to_look_for_, index)) != std::string::npos) {
      input_str_.replace(index, substr_to_look_for_.length(), substr_to_replace_);
      index += substr_to_replace_.length();
    }
  }
  static void removeRepeatedCharInsideInputStr(std::string &inputStr_, const std::string &doubledChar_){
    std::string doubledCharStr = doubledChar_+doubledChar_;
    std::string lastStr;
    do{
      lastStr = inputStr_;
      GenericToolbox::replaceSubstringInsideInputString(inputStr_, doubledCharStr, doubledChar_);
    } while( lastStr != inputStr_ );
  }
  static void removeTrailingCharInsideInputStr(std::string &inputStr_, const std::string &trailingChar_){
    if( GenericToolbox::endsWith(inputStr_, trailingChar_) ){
      inputStr_.erase(inputStr_.size() - trailingChar_.size());
    }
  }
  static void indentInputString(std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar){
    int originalSize = int(inputStr_.size());
    for( int iChar = originalSize-1 ; iChar >= 0 ; iChar-- ){
      if( iChar == 0 or inputStr_[iChar] == '\n'){
        int offSet = 1;
        if( iChar == 0 ) offSet = 0;
        for( unsigned int iIndent = 0 ; iIndent < indentCount_ ; iIndent++ ){
          inputStr_.insert(iChar+offSet, indentChar);
        }
      }
    }
  }
  static void trimInputString(std::string &inputStr_, const std::string &strToTrim_){
    while( GenericToolbox::startsWith(inputStr_, strToTrim_) ){
      inputStr_ = inputStr_.substr(strToTrim_.size(), inputStr_.size());
    }
    while( GenericToolbox::endsWith(inputStr_, strToTrim_) ){
      inputStr_ = inputStr_.substr(0, inputStr_.size() - strToTrim_.size());
    }
  }

  // -- conversions
  template<typename T, typename TT> static std::string toString(const T& iterable_, const TT& toStrFct_, bool jumpLine_, bool indentLine_){
    std::stringstream ss;
    ss << "{ ";
    if( not iterable_.empty() ){
      if( jumpLine_ ){ ss << std::endl << "  "; }
      ss.str().reserve(256); // Reserve initial space
      auto elementIterator = iterable_.begin();
      ss << toStrFct_(*elementIterator);
      for( ++elementIterator; elementIterator != iterable_.end(); ++elementIterator ){
        ss << ", ";
        if( jumpLine_ ){
          ss << std::endl;
          if( indentLine_ ){ ss << "  "; }
        }
        ss << toStrFct_(*elementIterator);
      }
      if( not jumpLine_ ){ ss << " "; }
    }
    if( jumpLine_ ){ ss << std::endl; }
    ss << "}";
    return ss.str();
  }
  template<typename T> static std::string toString(const std::vector<T> &vector_, bool jumpLine_, bool indentLine_){
    return GenericToolbox::toString(vector_, [&](const T& elm_){ return elm_; }, jumpLine_, indentLine_);
  }
  template<> std::string toString(const std::vector<std::string> &vector_, bool jumpLine_, bool indentLine_){
    return GenericToolbox::toString(vector_, [&](const std::string& elm_){ return std::string{"\""+elm_+"\""}; }, jumpLine_, indentLine_);
  }
  template<> std::string toString(const std::vector<unsigned char> &vector_, bool jumpLine_, bool indentLine_){
    return GenericToolbox::toString(vector_, [&](const unsigned char& elm_){ return std::string{"0x"+GenericToolbox::toHex(elm_)}; }, jumpLine_, indentLine_);
  }
  template<> std::string toString(const std::vector<char> &vector_, bool jumpLine_, bool indentLine_){
    return GenericToolbox::toString(vector_, [&](const unsigned char& elm_){ return std::string{"0x"+GenericToolbox::toHex(elm_)}; }, jumpLine_, indentLine_);
  }
  template<typename T, size_t N> static std::string toString(const std::array<T, N>& array_){
    return std::string(std::begin(array_), std::end(array_));
  }
  template <typename T1, typename T2> static std::string toString(const std::map<T1, T2>& map_, bool enableLineJump_){
    return GenericToolbox::toString(
        map_,
        [&](const std::pair<T1, T2>& elm_){
          std::stringstream ss;
          ss << "{ " << elm_.first << ": " << elm_.second << " }";
          return ss.str();
        },
        enableLineJump_, enableLineJump_);
  }
  template <typename T1, typename T2, typename T3> static std::string toString(const std::map<T1, std::pair<T2,T3>>& map_, bool enableLineJump_){
    return GenericToolbox::toString(
        map_,
        [&](const std::pair<T1, std::pair<T2,T3>>& elm_){
          std::stringstream ss;
          ss << "{ " << elm_.first << ": {" << elm_.second.first << ", " << elm_.second.second << "} }";
          return ss.str();
        },
        enableLineJump_, enableLineJump_);
  }

  template<typename T> static std::string toHex(const T& val_){ return toHex(&val_, sizeof(val_)); }
  static std::string toHex(const void* address_, size_t nBytes_){
    std::stringstream ss(std::string(2*nBytes_, 0));
    unsigned char* address{(unsigned char*)(address_) + nBytes_-1};
    do{ ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(*(address--)); } while(address >= address_);
    return ss.str();
  }
  template<typename T> static std::string stackToHex(const std::vector<T> &rawData_, size_t stackSize_) {
    std::stringstream ss;
    size_t nChunks = rawData_.size()*sizeof(T)/stackSize_;
    const unsigned char* address{&rawData_[0]};
    for( int iChunk=0 ; iChunk < nChunks ; iChunk++ ){
      ss.str().empty()? ss << "{ ": ss << ", ";
      ss << "0x" << GenericToolbox::toHex(address, stackSize_);
      address += stackSize_;
    }

    if( address < &(rawData_.back())+sizeof(T) ) {
      ss.str().empty()? ss << "{ ": ss << ", ";
      ss << "0x" << GenericToolbox::repeatString("_-", address+stackSize_ - (&(rawData_.back())+sizeof(T)));
      ss << GenericToolbox::toHex(address, (&(rawData_.back()) + sizeof(T)) - address);
    }

    ss << " }";
    return ss.str();
  }
  static bool toBool(const std::string& str) {
    auto result = false;    // failure to assert is false

    std::istringstream is(str);
    // first try simple integer conversion
    is >> result;

    if (is.fail()) {
      // simple integer failed; try boolean
      is.clear();
      is >> std::boolalpha >> result;
    }

    if( is.fail() ){
      throw std::invalid_argument( str + " is not convertable to bool" );
      return false;
    }

    return result;
  }

  static std::string parseIntAsString(int intToFormat_){
    if(intToFormat_ / 1000 < 10){
      return std::to_string(intToFormat_);
    }
    intToFormat_/=1000.; // in K
    if(intToFormat_ / 1000 < 10){
      return std::to_string(intToFormat_) + "K";
    }
    intToFormat_/=1000.; // in M
    if(intToFormat_ / 1000 < 10){
      return std::to_string(intToFormat_) + "M";
    }
    intToFormat_/=1000.; // in G
    if(intToFormat_ / 1000 < 10){
      return std::to_string(intToFormat_) + "G";
    }
    intToFormat_/=1000.; // in T
    if(intToFormat_ / 1000 < 10){
      return std::to_string(intToFormat_) + "T";
    }
    intToFormat_/=1000.; // in P
    return std::to_string(intToFormat_) + "P";
  }

  // deprecated
  GT_DEPRECATED("renamed: hasSubStr") static bool doesStringContainsSubstring(const std::string& str_, const std::string& subStr_, bool ignoreCase_ = false) {
    return hasSubStr(str_, subStr_, ignoreCase_);
  }
  GT_DEPRECATED("renamed: startsWith") static bool doesStringStartsWithSubstring(const std::string& str_, const std::string& subStr_, bool ignoreCase_ = false) {
    return startsWith(str_, subStr_, ignoreCase_);
  }
  GT_DEPRECATED("renamed: endsWith") static bool doesStringEndsWithSubstring(const std::string& str_, const std::string& subStr_, bool ignoreCase_ = false) {
    return endsWith(str_, subStr_, ignoreCase_);
  }

  template<typename T, typename TT> GT_DEPRECATED("renamed: toString") static std::string iterableToString(const T& iterable_, const TT& toStrFct_, bool jumpLine_=true, bool indentLine_=true){
    return toString(iterable_, toStrFct_, jumpLine_, indentLine_);
  }
  template <typename T> GT_DEPRECATED("renamed: toString") static std::string parseVectorAsString(const std::vector<T>& vector_, bool jumpLine_=true, bool indentLine_=true){
    return toString(vector_, jumpLine_, indentLine_);
  }
  GT_DEPRECATED("renamed: toString") static std::string parseVectorAsString(const std::vector<std::string> &vector_, bool jumpLine_=true, bool indentLine_=true){
    return toString(vector_, jumpLine_, indentLine_);
  }
  GT_DEPRECATED("renamed: toString") static std::string parseVectorAsString(const std::vector<unsigned char> &vector_, bool jumpLine_=true, bool indentLine_=true){
    return toString(vector_, jumpLine_, indentLine_);
  }
  GT_DEPRECATED("renamed: toString") static std::string parseVectorAsString(const std::vector<char> &vector_, bool jumpLine_=true, bool indentLine_=true){
    return toString(vector_, jumpLine_, indentLine_);
  }
  template <typename T1, typename T2> GT_DEPRECATED("renamed: toString") static std::string parseMapAsString(const std::map<T1, T2>& map_, bool enableLineJump_=true){ return toString(map_, enableLineJump_); }
  template <typename T1, typename T2, typename T3> GT_DEPRECATED("renamed: toString") static std::string parseMapAsString(const std::map<T1, std::pair<T2,T3>>& map_, bool enableLineJump_){ return toString(map_, enableLineJump_); }

  GT_DEPRECATED("renamed: stripUnicode") static std::string stripStringUnicode(const std::string &inputStr_){
    return stripUnicode(inputStr_);
  }

  template<typename T> static std::string toHexString(T integerVal_, size_t nbDigit_){
    std::stringstream stream;
    stream << "0x" << toHex(&integerVal_, sizeof(integerVal_));
    if( nbDigit_ == 0 ) return stream.str();
    else return "0x" + stream.str().substr(2 + sizeof(T)*2 - nbDigit_, nbDigit_);
  }

}


#endif // CPP_GENERIC_TOOLBOX_STRING_H
