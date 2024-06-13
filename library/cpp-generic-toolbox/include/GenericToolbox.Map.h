//
// Created by Nadrino on 23/12/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_MAP_H
#define CPP_GENERIC_TOOLBOX_MAP_H

// ***************************
//! Map related tools
// ***************************

#include "GenericToolbox.String.h"

#include <string>
#include <map>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"


// Declaration section
namespace GenericToolbox{

  template <typename K, typename T> static bool isIn( const K& key_, const std::map<K,T>& map_ );
  template <typename K, typename T> static T* getElementPtrIsInMap( const K& key_, std::map<K,T>& map_ );
  template <typename T1, typename T2> static void appendToMap( std::map<T1, T2> &mapContainer_, const std::map<T1, T2> &mapToPushBack_, bool overwrite_ = true );
  template <typename T> static std::map<std::string, T> getSubMap(const std::map<std::string, T>& map_, const std::string &keyStrStartWith_ );

  template <typename K, typename T> [[deprecated("use isIn()")]] static bool doesKeyIsInMap( const K& key_, const std::map<K,T>& map_ ){ return isIn(key_, map_);}

}


// Implementation section
namespace GenericToolbox {

  template <typename K, typename  T> static bool isIn( const K& key_, const std::map<K,T>& map_ ){
    return ( map_.find(key_) != map_.end() );
  }
  template <typename K, typename T> static T* getElementPtrIsInMap( const K& key_, std::map<K,T>& map_ ){
    auto it = map_.find(key_);
    if( it == map_.end() ){
      return nullptr;
    }
    return &( it->second );
  }
  template <typename T1, typename T2> static void appendToMap(std::map<T1, T2> &mapContainer_, const std::map<T1, T2> &mapToPushBack_, bool overwrite_) {
    for(const auto& newEntry : mapToPushBack_){
      if(not overwrite_ and doesKeyIsInMap(newEntry.first, mapContainer_)){
        continue;
      }
      mapContainer_[newEntry.first] = newEntry.second;
    }
  }
  template <typename T> static std::map<std::string, T> getSubMap(const std::map<std::string, T>& map_, const std::string &keyStrStartWith_ ){
    std::map<std::string, T> outSubMap;
    for(const auto& mapPair : map_){
      if(GenericToolbox::startsWith(mapPair.first, keyStrStartWith_)){
        outSubMap[mapPair.first] = mapPair.second;
      }
    }
    return outSubMap;
  }

}


#endif // CPP_GENERIC_TOOLBOX_MAP_H
