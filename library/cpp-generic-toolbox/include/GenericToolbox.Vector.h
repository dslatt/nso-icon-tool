//
// Created by Nadrino on 23/12/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_VECTOR_H
#define CPP_GENERIC_TOOLBOX_VECTOR_H

// ***************************
//! Vector related tools
// ***************************

#include "GenericToolbox.Macro.h"

#include <functional>
#include <numeric>
#include <string>
#include <vector>
#include <cmath>
#include <list>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"


// Declaration section
namespace GenericToolbox{

  // Content management
  template<typename Elem, typename Cont> static bool isIn( const Elem& element_, const Cont& container_ );
  template<typename Elem, typename Cont, typename Lambda> static bool isIn( const Elem& element_, const Cont& container_, const Lambda& fetchElem_ );
  template<typename Elm, typename Val, typename Lambda> static int isIn(const Val& value_, const std::vector<Elm>& vector_, const Lambda& fetchElmValueFct_);

  template<typename T> static int findElementIndex( const T& element_, const std::vector<T>& vector_ );
  static int findElementIndex(const char* element_, const std::vector<std::string>& vector_ );
  template<typename T> static void insertInVector(std::vector<T> &vector_, const std::vector<T> &vectorToInsert_, size_t insertBeforeThisIndex_);
  template<typename T> static void insertInVector(std::vector<T> &vector_, const T &elementToInsert_, size_t insertBeforeThisIndex_);
  template<typename T> static void mergeInVector(std::vector<T> &vector_, const std::vector<T> &other_, bool allowDuplicates_ = true);
  template<typename T> static void addIfNotInVector(const T& element_, std::vector<T> &vector_);
  static void addIfNotInVector(const char* element_, std::vector<std::string> &vector_);
  template <typename Elm, typename Val, typename Lambda> static int findElementIndex(const Val& value_, const std::vector<Elm>& vector_, const Lambda& fetchElmValueFct_);

  // Generators
  template<typename T> static std::vector<size_t> indices(const std::vector<T> &vector_);
  template<typename T> static std::vector<T> getSubVector( const std::vector<T>& vector_, size_t beginIndex_, int endIndex_ = -1 );
  template<typename T, typename TT> static std::vector<TT> convertVectorType( const std::vector<T>& vector_, std::function<TT(T)>& convertTypeFunction_ );

  // Stats
  template <typename T> static double getAverage(const std::vector<T>& vector_, const std::function<double(const T&)>& evalElementFct_ = [](const T& var){return var;});
  template<typename T> static double getAveragedSlope(const std::vector<T> &yValues_);
  template<typename T, typename TT> static double getAveragedSlope(const std::vector<T> &yValues_, const std::vector<TT> &xValues_);
  template <typename T> static double getStdDev(const std::vector<T>& vector_, const std::function<double(const T&)>& evalElementFct_ = [](const T& var){return var;});

  // Sorting
  template <typename T, typename Lambda> static std::vector<size_t> getSortPermutation(const std::vector<T>& vectorToSort_, const Lambda& firstArgGoesFirstFct_ );
  template <typename T> static std::vector<T> getSortedVector(const std::vector<T>& unsortedVector_, const std::vector<std::size_t>& sortPermutation_);
  template <typename T> static void applyPermutation(std::vector<T>& vectorToPermute_, const std::vector<std::size_t>& sortPermutation_);
  template <typename T, typename Lambda> static void sortVector(std::vector<T>& vectorToSort_, const Lambda& firstArgGoesFirstFct_);
  template <typename T, typename Lambda> static void removeEntryIf(std::vector<T>& vector_, const Lambda& removeIfFct_);

  // Others
  template<typename T, typename TT> static T& getListEntry(std::list<T>& list_, TT index_);
  template<typename T, typename TT> static const T& getListEntry(const std::list<T>& list_, TT index_);

  // deprecated
  template <typename T> static bool doesElementIsInVector( const T& element_, const std::vector<T>& vector_ );
  static bool doesElementIsInVector(const char* element_, const std::vector<std::string>& vector_);
  template <typename Elm, typename Val, typename Lambda> static int doesElementIsInVector(const Val& value_, const std::vector<Elm>& vector_, const Lambda& fetchElmValueFct_);

}


// Implementation section
namespace GenericToolbox {

  // Content management
  template<typename Elem, typename Cont> static bool isIn( const Elem& element_, const Cont& container_ ){
    return std::find(container_.cbegin(), container_.cend(), element_) != container_.cend();
  }
  template<typename Elem, typename Cont, typename Lambda> static bool isIn( const Elem& element_, const Cont& container_, const Lambda& fetchElem_ ){
    return std::find_if( container_.begin(), container_.end(), [&](const Elem& t){ return fetchElem_(t) == element_; }) != container_.end();
  }
  template <typename Elm, typename Val, typename Lambda> static int isIn(const Val& value_, const std::vector<Elm>& vector_, const Lambda& fetchElmValueFct_){
    return std::find_if( vector_.begin(), vector_.end(), [&](const Elm& t){ return fetchElmValueFct_(t) == value_; }) != vector_.end();
  }

  template <typename T> static bool doesElementIsInVector(const T& element_, const std::vector<T>& vector_){
    return std::find(vector_.cbegin(), vector_.cend(), element_) != vector_.cend();
  }
  static bool doesElementIsInVector(const char* element_, const std::vector<std::string>& vector_){
    return std::find(vector_.cbegin(), vector_.cend(), element_) != vector_.cend();
  }
  template <typename Elm, typename Val, typename Lambda> static int doesElementIsInVector(const Val& value_, const std::vector<Elm>& vector_, const Lambda& fetchElmValueFct_){
    return std::find_if( vector_.begin(), vector_.end(), [&](const Elm& t){ return fetchElmValueFct_(t) == value_; }) != vector_.end();
  }
  template <typename T> inline static int findElementIndex(const T& element_, const std::vector<T>& vector_ ){ // test
    auto it = std::find(vector_.cbegin(), vector_.cend(), element_);
    if( it == vector_.cend() ){ return -1; }
    return static_cast<int>( std::distance(vector_.cbegin(), it) );
  }
  inline static int findElementIndex(const char* element_, const std::vector<std::string>& vector_ ){
    auto it = std::find(vector_.cbegin(), vector_.cend(), element_);
    if( it == vector_.cend() ){ return -1; }
    return static_cast<int>( std::distance(vector_.cbegin(), it) );
  }
  template<typename T> inline static void insertInVector(std::vector<T> &vector_, const std::vector<T> &vectorToInsert_, size_t insertBeforeThisIndex_){
    if( insertBeforeThisIndex_ > vector_.size() ){
      throw std::runtime_error("GenericToolBox::insertInVector error: insertBeforeThisIndex_ >= vector_.size()");
    }
    if( vector_.empty() ){ vector_ = vectorToInsert_; return; }
    if( vectorToInsert_.empty() ){ return; }
#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ <= 4)
    vector_.insert( vector_.begin() + insertBeforeThisIndex_, vectorToInsert_.begin(), vectorToInsert_.end() );
#else
    vector_.insert( vector_.cbegin() + insertBeforeThisIndex_, vectorToInsert_.cbegin(), vectorToInsert_.cend() );
#endif
  }
  template<typename T> inline static void insertInVector(std::vector<T> &vector_, const T &elementToInsert_, size_t insertBeforeThisIndex_){
    insertInVector(vector_, std::vector<T>{elementToInsert_}, insertBeforeThisIndex_);
  }
  template<typename T> inline static void mergeInVector( std::vector<T> &vector_, const std::vector<T> &other_, bool allowDuplicates_ ){
    if( allowDuplicates_ ){ GenericToolbox::insertInVector(vector_, other_, vector_.size()); }
    else{
      vector_.reserve( vector_.size() + other_.size() );
      for( auto& element : other_ ){ GenericToolbox::addIfNotInVector(element, vector_); }
      vector_.shrink_to_fit();
    }
  }
  template<typename T> static void addIfNotInVector(const T& element_, std::vector<T> &vector_){
    if( not GenericToolbox::doesElementIsInVector(element_, vector_) ){
      vector_.template emplace_back(element_);
    }
  }
  static void addIfNotInVector(const char* element_, std::vector<std::string> &vector_){
    if( not GenericToolbox::doesElementIsInVector(element_, vector_) ){
      vector_.emplace_back(element_);
    }
  }
  template <typename Elm, typename Val, typename Lambda> inline static int findElementIndex(const Val& value_, const std::vector<Elm>& vector_, const Lambda& fetchElmValueFct_){
    auto it = std::find_if( vector_.begin(), vector_.end(), [&](const Elm& t){ return fetchElmValueFct_(t) == value_; });
    if( it == vector_.end() ){ return -1; }
    else{ return std::distance( vector_.begin(), it ); }
  }

  // Generators
  template<typename T> static std::vector<size_t> indices(const std::vector<T> &vector_){
    std::vector<size_t> output(vector_.size(), 0);
    for( size_t iIndex = 0 ; iIndex < output.size() ; iIndex++ ){
      output.at(iIndex) = iIndex;
    }
    return output;
  }
  template <typename T> static std::vector<T> getSubVector( const std::vector<T>& vector_, size_t beginIndex_, int endIndex_ ){
    if( endIndex_ < 0 ){ endIndex_ += vector_.size(); }
    if( beginIndex_ >= endIndex_ ){ return std::vector<T> (); }
    return std::vector<T> ( &vector_[beginIndex_] , &vector_[endIndex_+1] );
  }
  template <typename T, typename TT> static std::vector<TT> convertVectorType( const std::vector<T>& vector_, std::function<TT(T)>& convertTypeFunction_ ){
    std::vector<TT> outVec;
    for(const auto& element : vector_){
      outVec.emplace_back(convertTypeFunction_(element));
    }
    return outVec;
  }

  // Stats
  template <typename T> static double getAverage(const std::vector<T>& vector_, const std::function<double(const T&)>& evalElementFct_){
    double outVal = 0;
    for( auto& element : vector_ ){ outVal += static_cast<double>(evalElementFct_(element)); }
    return outVal / vector_.size();
  }
  template<typename T> static double getAveragedSlope(const std::vector<T> &yValues_){
    auto xValues = yValues_;
    for( size_t iVal = 0 ; iVal < yValues_.size() ; iVal++ ){
      xValues.at(iVal) = iVal;
    }
    return getAveragedSlope(yValues_, xValues);
  }
  template<typename T, typename TT> static double getAveragedSlope(const std::vector<T> &yValues_, const std::vector<TT> &xValues_){
    if(xValues_.size() != yValues_.size()){
      throw std::logic_error("x and y values list do have the same size.");
    }
    const auto n    = xValues_.size();
    const auto s_x  = std::accumulate(xValues_.begin(), xValues_.end(), 0.0);
    const auto s_y  = std::accumulate(yValues_.begin(), yValues_.end(), 0.0);
    const auto s_xx = std::inner_product(xValues_.begin(), xValues_.end(), xValues_.begin(), 0.0);
    const auto s_xy = std::inner_product(xValues_.begin(), xValues_.end(), yValues_.begin(), 0.0);
    const auto a    = (n * s_xy - s_x * s_y) / (n * s_xx - s_x * s_x);
    return a;
  }
  template <typename T> static double getStdDev(const std::vector<T>& vector_, const std::function<double(const T&)>& evalElementFct_){
    double outVal = 0;
    double mean = getAverage(vector_, evalElementFct_);
    for( auto& element : vector_ ){
      outVal += std::pow(
          static_cast<double>(evalElementFct_(element)) - mean,
          2
      );
    }
    return sqrt( outVal / vector_.size() );
  }


  // Sorting
  template <typename T, typename Lambda> static std::vector<size_t> getSortPermutation(const std::vector<T>& vectorToSort_, const Lambda& firstArgGoesFirstFct_ ){
    std::vector<size_t> p(vectorToSort_.size());
    std::iota(p.begin(), p.end(), 0);
    std::sort(p.begin(), p.end(),
              [&](size_t i, size_t j){ return firstArgGoesFirstFct_(vectorToSort_.at(i), vectorToSort_.at(j)); });
    return p;
  }
  template <typename T> static std::vector<T> getSortedVector(const std::vector<T>& unsortedVector_, const std::vector<std::size_t>& sortPermutation_ ){
    if(unsortedVector_.empty() or sortPermutation_.size() != unsortedVector_.size()) return {};
    std::vector<T> sortedVec(unsortedVector_.size(), unsortedVector_[0]);
    std::transform(sortPermutation_.begin(), sortPermutation_.end(), sortedVec.begin(),
                   [&](std::size_t i){ return unsortedVector_[i]; });
    return sortedVec;
  }
  template <typename T> static void applyPermutation(std::vector<T>& vectorToPermute_, const std::vector<std::size_t>& sortPermutation_){
    std::vector<bool> done(vectorToPermute_.size(), false);
    for( std::size_t iEntry = 0; iEntry < vectorToPermute_.size(); iEntry++ ){
      if( done[iEntry] ){ continue; }
      done[iEntry] = true;
      std::size_t jPrev = iEntry;
      std::size_t jEntry = sortPermutation_[iEntry];
      while( iEntry != jEntry ){
        std::swap(vectorToPermute_[jPrev], vectorToPermute_[jEntry]);
        done[jEntry] = true;
        jPrev = jEntry;
        jEntry = sortPermutation_[jEntry];
      }
    }
  }
  template <typename T, typename Lambda> static void sortVector(std::vector<T>& vectorToSort_, const Lambda& firstArgGoesFirstFct_){
    std::sort(vectorToSort_.begin(), vectorToSort_.end(), firstArgGoesFirstFct_);
  }
  template <typename T, typename Lambda> static void removeEntryIf(std::vector<T>& vector_, const Lambda& removeIfFct_){
    vector_.erase( std::remove_if(vector_.begin(), vector_.end(), removeIfFct_), vector_.end() );
  }

  // Others
  template<typename T, typename TT> static T& getListEntry(std::list<T>& list_, TT index_){
    typename std::list<T>::iterator it = list_.begin();
    std::advance(it, index_);
    return *it;
  }
  template<typename T, typename TT> static const T& getListEntry(const std::list<T>& list_, TT index_){
    typename std::list<T>::const_iterator it = list_.begin();
    std::advance(it, index_);
    return *it;
  }

}


#endif // CPP_GENERIC_TOOLBOX_VECTOR_H
