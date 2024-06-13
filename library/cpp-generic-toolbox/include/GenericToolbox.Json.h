//
// Created by Nadrino on 22/05/2021.
//

#ifndef GENERICTOOLBOX_JSONUTILS_H
#define GENERICTOOLBOX_JSONUTILS_H


#include "GenericToolbox.Vector.h"
#include "GenericToolbox.String.h"
#include "GenericToolbox.Fs.h"
#include "GenericToolbox.Os.h"

#include "nlohmann/json.hpp"

#include <iostream>
#include <string>
#include <vector>


// Declaration
namespace GenericToolbox {
  namespace Json {

    template<typename J> inline auto readConfigJsonStr(const std::string& configJsonStr_) -> J;
    template<typename J> inline auto readConfigFile(const std::string& configFilePath_) -> J;
    template<typename J> inline auto getForwardedConfig(const J& config_);
    template<typename J> inline auto getForwardedConfig(const J& config_, const std::string& keyName_);
    template<typename J> inline void forwardConfig(J& config_, const std::string& className_ = "");
    template<typename J> inline void unfoldConfig(J& config_);
    template<typename J> inline std::string toReadableString(const J& config_);

    template<typename J> inline std::vector<std::string> ls(const J& jsonConfig_);
    template<typename J> inline bool doKeyExist(const J& jsonConfig_, const std::string& keyName_);
    template<typename J> inline auto fetchSubEntry(const J& jsonConfig_, const std::vector<std::string>& keyPath_) -> J;
    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::string& keyName_) -> T;
    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::vector<std::string>& keyNames_) -> T;
    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::string& keyName_, const T& defaultValue_) -> T;
    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::vector<std::string>& keyName_, const T& defaultValue_) -> T;
    template<typename T, typename J> inline auto fetchValuePath(const J& jsonConfig_, const std::string& keyNamePath_) -> T;
    template<typename J, typename T> inline auto fetchMatchingEntry(const J& jsonConfig_, const std::string& keyName_, const T& keyValue_) -> J;
    template<typename J, typename F> inline bool deprecatedAction(const J& jsonConfig_, const std::string& keyName_, const F& action_);
    template<typename J, typename F> inline bool deprecatedAction(const J& jsonConfig_, const std::vector<std::string>& keyPath_, const F& action_);

    // template specialization when a string literal is passed:
    template<std::size_t N, typename J> inline auto fetchValue(const J& jsonConfig_, const std::string& keyName_, const char (&defaultValue_)[N]) -> std::string { return fetchValue(jsonConfig_, keyName_, std::string(defaultValue_)); }
    template<std::size_t N, typename J> inline auto fetchValue(const J& jsonConfig_, const std::vector<std::string>& keyName_, const char (&defaultValue_)[N]) -> std::string { return fetchValue(jsonConfig_, keyName_, std::string(defaultValue_)); }
    template<std::size_t N> inline auto fetchMatchingEntry(const nlohmann::json& jsonConfig_, const std::string& keyName_, const char (&keyValue_)[N]) -> nlohmann::json{ return fetchMatchingEntry(jsonConfig_, keyName_, std::string(keyValue_)); }

    // GUNDAM/ROOT specific
    template<typename J> inline std::string buildFormula(const J& jsonConfig_, const std::string& keyName_, const std::string& joinStr_);
    template<typename J> inline std::string buildFormula(const J& jsonConfig_, const std::string& keyName_, const std::string& joinStr_, const std::string& defaultFormula_);

    // defaults
    inline nlohmann::json readConfigJsonStr(const std::string& configJsonStr_) { return readConfigJsonStr<nlohmann::json>(configJsonStr_); }
    inline nlohmann::json readConfigFile(const std::string& configJsonStr_) { return readConfigFile<nlohmann::json>(configJsonStr_); }

  }


}


// Implementation
namespace GenericToolbox {
  namespace Json {

    template<typename J> inline auto readConfigJsonStr(const std::string& configJsonStr_) -> J{
      std::stringstream ss;
      ss << configJsonStr_;

      J output;
      ss >> output;

      return output;
    }
    template<typename J> inline auto readConfigFile(const std::string& configFilePath_) -> J{
      if( not GenericToolbox::isFile(configFilePath_) ){
        std::cout << "\"" << configFilePath_ << "\" could not be found." << std::endl;
        throw std::runtime_error("file not found.");
      }

      std::fstream fs;
      fs.open(configFilePath_, std::ios::in);

      if( not fs.is_open() ) {
        std::cout << "\"" << configFilePath_ << "\": could not read file." << std::endl;
        throw std::runtime_error("file not readable.");
      }

      J output;
      fs >> output;
      return output;
    }
    template<typename J> inline auto getForwardedConfig(const J& config_) -> J {
      J out = config_;
      while( out.is_string() ){
        out = GenericToolbox::Json::readConfigFile<J>(out.template get<std::string>());
      }
      return out;
    }
    template<typename J> inline auto getForwardedConfig(const J& config_, const std::string& keyName_) -> J{
      return GenericToolbox::Json::getForwardedConfig<J>(GenericToolbox::Json::fetchValue<J>(config_, keyName_));
    }
    template<typename J> inline void forwardConfig(J& config_, const std::string& className_){
      while( config_.is_string() ){
        std::cout << "Forwarding " << (className_.empty()? "": className_ + " ") << "config: \"" << config_.template get<std::string>() << "\"" << std::endl;
        auto name = config_.template get<std::string>();
        std::string expand = GenericToolbox::expandEnvironmentVariables(name);
        config_ = GenericToolbox::Json::readConfigFile<J>(expand);
      }
    }
    template<typename J> inline void unfoldConfig(J& config_){
      for( auto& entry : config_ ){
        if( entry.is_string() and (
            GenericToolbox::endsWith(entry.template get<std::string>(), ".yaml", true)
            or GenericToolbox::endsWith(entry.template get<std::string>(), ".json", true)
        ) ){
          GenericToolbox::Json::forwardConfig(entry);
          GenericToolbox::Json::unfoldConfig(config_); // remake the loop on the unfolder config
          break; // don't touch anymore
        }

        if( entry.is_structured() ){
          GenericToolbox::Json::unfoldConfig(entry);
        }
      }
    }
    template<typename J> inline std::string toReadableString(const J& config_){
      std::stringstream ss;
      ss << config_ << std::endl;

      std::string originalJson = ss.str();
      ss.str(""); ss.clear();
      int indentLevel{0};
      bool inQuote{false};
      for( char c : originalJson ){

        if( c == '"'){ inQuote = not inQuote; }

        if( not inQuote ){
          if( c == '{' or c == '[' ){
            ss << std::endl << GenericToolbox::repeatString("  ", indentLevel) << c;
            indentLevel++;
            ss << std::endl << GenericToolbox::repeatString("  ", indentLevel);
          }
          else if( c == '}' or c == ']' ){
            indentLevel--;
            ss << std::endl << GenericToolbox::repeatString("  ", indentLevel) << c;
          }
          else if( c == ':' ){
            ss << c << " ";
          }
          else if( c == ',' ){
            ss << c << std::endl << GenericToolbox::repeatString("  ", indentLevel);
          }
          else if( c == '\n' ){
            if( ss.str().back() != '\n' ) ss << c;
          }
          else{
            ss << c;
          }
        }
        else{
          ss << c;
        }

      }
      return ss.str();
    }

    template<typename J> inline bool doKeyExist(const J& jsonConfig_, const std::string& keyName_){
      return jsonConfig_.find(keyName_) != jsonConfig_.end();
    }
    template<typename J> inline std::vector<std::string> ls(const J& jsonConfig_){
      std::vector<std::string> out{};
      out.reserve( jsonConfig_.size() );
      for (auto& entry : jsonConfig_.items()){ out.emplace_back( entry.key() ); }
      return out;
    }
    template<typename J> inline auto fetchSubEntry(const J& jsonConfig_, const std::vector<std::string>& keyPath_) -> J {
      J output = jsonConfig_;
      for( const auto& key : keyPath_ ){
        output = GenericToolbox::Json::fetchValue<J>(output, key);
      }
      return output;
    }

    template<typename J> inline std::string buildFormula(const J& jsonConfig_, const std::string& keyName_, const std::string& joinStr_){
      std::string out;

      if( not GenericToolbox::Json::doKeyExist(jsonConfig_, keyName_) ){
        std::cout << "Could not find key \"" << keyName_ << "\" in " << jsonConfig_ << std::endl;
        throw std::runtime_error("Could not find key");
      }

      try{ return GenericToolbox::Json::fetchValue<std::string>(jsonConfig_, keyName_); }
      catch (...){
        // it's a vector of strings
      }

      std::vector<std::string> conditionsList;
      auto jsonList{ GenericToolbox::Json::fetchValue<J>(jsonConfig_, keyName_) };

      if( jsonList.size() == 1 and not jsonList[0].is_string() and jsonList[0].is_array() ){
        // hot fix for broken json versions
        jsonList = jsonList[0];
      }

      for( auto& condEntry : jsonList ){
        if( condEntry.is_string() ){
          conditionsList.emplace_back( condEntry.template get<std::string>() );
        }
        else{
          std::cout << "Could not recognise condition entry: " << condEntry << std::endl;
          throw std::runtime_error("Could not recognise condition entry");
        }
      }

      out += "(";
      out += GenericToolbox::joinVectorString(conditionsList, ") " + joinStr_ + " (");
      out += ")";

      return out;
    }
    template<typename J> inline std::string buildFormula(const J& jsonConfig_, const std::string& keyName_, const std::string& joinStr_, const std::string& defaultFormula_){
      if( not GenericToolbox::Json::doKeyExist(jsonConfig_, keyName_) ) return defaultFormula_;
      else return buildFormula(jsonConfig_, keyName_, joinStr_);
    }

    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::string& keyName_) -> T{
      auto jsonEntry = jsonConfig_.find(keyName_);
      if( jsonEntry == jsonConfig_.end() ){
        throw std::runtime_error("Could not find json entry: " + keyName_ + ":\n" + GenericToolbox::Json::toReadableString(jsonConfig_));
      }
      return jsonEntry->template get<T>();
    }
    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::vector<std::string>& keyNames_) -> T{
      for( auto& keyName : keyNames_){
        if( GenericToolbox::Json::doKeyExist(jsonConfig_, keyName) ){
          return GenericToolbox::Json::fetchValue<T>(jsonConfig_, keyName);
        }
      }
      throw std::runtime_error("Could not find any json entry: " + GenericToolbox::toString(keyNames_) + ":\n" + GenericToolbox::Json::toReadableString(jsonConfig_));
    }
    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::string& keyName_, const T& defaultValue_) -> T{
      try{
        T value = GenericToolbox::Json::fetchValue<T>(jsonConfig_, keyName_);
        return value; // if nothing has gone wrong
      }
      catch (...){
        return defaultValue_;
      }
    }
    template<typename T, typename J> inline auto fetchValue(const J& jsonConfig_, const std::vector<std::string>& keyName_, const T& defaultValue_) -> T{
      for( auto& keyName : keyName_ ){
        try{
          T value = GenericToolbox::Json::fetchValue<T>(jsonConfig_, keyName);
          return value; // if nothing has gone wrong
        }
        catch (...){
        }
      }
      return defaultValue_;
    }
    template<typename T, typename J> inline auto fetchValuePath(const J& jsonConfig_, const std::string& keyNamePath_) -> T{
      auto keyPathElements = GenericToolbox::splitString(keyNamePath_, "/", true);
      J elm{jsonConfig_};

      if( elm.is_array() and elm.size() == 1 ){ elm = elm[0]; }

      for( auto& keyPathElement : keyPathElements ){
        elm = GenericToolbox::Json::fetchValue<J>(elm, keyPathElement);
      }
      return elm.template get<T>();
    }
    template<typename J, typename T> inline auto fetchMatchingEntry(const J& jsonConfig_, const std::string& keyName_, const T& keyValue_) -> J{

      if( not jsonConfig_.is_array() ){
        std::cout << "key: " << keyName_ << std::endl;
        std::cout << "value: " << keyValue_ << std::endl;
        std::cout << "dump: " << GenericToolbox::Json::toReadableString(jsonConfig_) << std::endl;
        throw std::runtime_error("GenericToolbox::Json::fetchMatchingEntry: jsonConfig_ is not an array.");
      }

      for( const auto& jsonEntry : jsonConfig_ ){
        try{
          if(GenericToolbox::Json::fetchValue<T>(jsonEntry, keyName_) == keyValue_ ){
            return jsonEntry;
          }
        }
        catch (...){
          // key not present, skip
        }

      }
      return {}; // .empty()
    }
    template<typename J, typename F> inline bool deprecatedAction(const J& jsonConfig_, const std::string& keyName_, const F& action_){
      if( not GenericToolbox::Json::doKeyExist(jsonConfig_, keyName_) ){ return false; }
      std::cout << "DEPRECATED option: \"" << keyName_ << "\". Running defined action..." << std::endl;
      action_();
      return true;
    }
    template<typename J, typename F> inline bool deprecatedAction(const J& jsonConfig_, const std::vector<std::string>& keyPath_, const F& action_){
      J walkConfig{jsonConfig_};
      for( auto keyName : keyPath_ ){
        if( not GenericToolbox::Json::doKeyExist(walkConfig, keyName) ){ return false; } // alright
        walkConfig = GenericToolbox::Json::fetchValue<J>(walkConfig, keyName);
      }
      std::cout << "DEPRECATED option: \"" << GenericToolbox::joinVectorString(keyPath_, "/") << "\". Running defined action..." << std::endl;
      action_();
      return true;
    }

  }
}

#endif //GENERICTOOLBOX_JSONUTILS_H
