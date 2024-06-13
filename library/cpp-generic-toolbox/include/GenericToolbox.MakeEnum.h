//
// Created by Nadrino on 13/12/2023.
//


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcomment"

#include <algorithm>
#include <string>
#include <cctype>


/*
 * required: ENUM_NAME, ENUM_FIELDS
 * optional: ENUM_TYPE, ENUM_OVERFLOW
 *
 * example:

#define ENUM_NAME GffDataType
#define ENUM_FIELDS \
  ENUM_FIELD( UChar, 0 ) ENUM_FIELD( Char ) \
  ENUM_FIELD( UShort ) ENUM_FIELD( Short ) \
  ENUM_FIELD( UInt ) ENUM_FIELD( Int ) \
  ENUM_FIELD( ULong ) ENUM_FIELD( Long ) \
  ENUM_FIELD( Float ) ENUM_FIELD( Double ) \
  ENUM_FIELD( ExoString ) \
  ENUM_FIELD( ResourceReference ) \
  ENUM_FIELD( LocalizedString ) \
  ENUM_FIELD( Void ) \
  ENUM_FIELD( Struct ) \
  ENUM_FIELD( List ) \
  ENUM_FIELD( Orientation ) \
  ENUM_FIELD( Position ) \
  ENUM_FIELD( StringReference ) \
  ENUM_FIELD( TopLevelStruct, 0xFFFFFFFF )
#define ENUM_TYPE unsigned int
#define ENUM_OVERFLOW ENUM_FIELD( BadGffDataType, 0x0FFFFFFF )
#include "GenericToolbox.MakeEnum.h"

 *
 */


// sanity checks
#ifndef ENUM_NAME
#error "ENUM_NAME not set."
#endif

#ifndef ENUM_FIELDS
#error "ENUM_FIELDS not set."
#endif

// define temp macros
#define TEMP_GET_OVERLOADED_MACRO2(_1,_2,NAME,...) NAME
#define TEMP_FIRST_ARG_TO_STR(entry_, ...) #entry_
#define TEMP_FIRST_ARG(entry_, ...) entry_
#define TEMP_ENUM_FIELD2(entry_, val_) entry_ = val_,
#define TEMP_ENUM_FIELD1(entry_) entry_,
#define TEMP_MERGE_DEPLOY(X, Y) X##Y
#define TEMP_MERGE(X, Y) TEMP_MERGE_DEPLOY(X, Y) // use a proxy for evaluating X and Y before merging


// Yes, this is a lie... It's actually a struct!
struct ENUM_NAME {

#ifdef ENUM_TYPE
  typedef ENUM_TYPE EnumType;
#else
  typedef int EnumType;
#endif

  enum TEMP_MERGE(ENUM_NAME, Enum) : EnumType {

#define ENUM_FIELD( ... ) TEMP_GET_OVERLOADED_MACRO2(__VA_ARGS__, TEMP_ENUM_FIELD2, TEMP_ENUM_FIELD1)(__VA_ARGS__)
    ENUM_FIELDS
#ifdef ENUM_OVERFLOW
    ENUM_OVERFLOW
#undef ENUM_FIELD
#define ENUM_FIELD( ... ) TEMP_FIRST_ARG(__VA_ARGS__)
  };
  static const EnumType overflowValue{ ENUM_OVERFLOW };
#else
    EnumOverflow
  };
  static const EnumType overflowValue{ EnumOverflow };
#endif
#undef ENUM_FIELD

  typedef TEMP_MERGE(ENUM_NAME, Enum) EnumTypeName;
  typedef ENUM_NAME StructType;

  // store the actual value
  EnumTypeName value{};

  // can't use "StructType" for CTors
  ENUM_NAME() = default;
  ENUM_NAME(EnumTypeName value_) : value(value_) {}
  ENUM_NAME(EnumType value_) : value(static_cast<EnumTypeName>(value_)) {}

  StructType& operator=(EnumTypeName value_){ this->value = value_; return *this; }
  StructType& operator=(EnumType value_){ this->value = static_cast<EnumTypeName>(value_); return *this; }

  friend bool operator==(const StructType& lhs, const StructType& rhs){ return lhs.value == rhs.value; }
  friend bool operator!=(const StructType& lhs, const StructType& rhs){ return lhs.value != rhs.value; }

  static int getEnumSize(){
#define ENUM_FIELD(...) TEMP_FIRST_ARG(__VA_ARGS__),
    static const EnumType indices[] = { ENUM_FIELDS };
    return sizeof(indices)/sizeof(EnumType);
  }
  static EnumType getEnumVal(int index_){
    static const EnumType indices[] = { ENUM_FIELDS };
    if( index_ < 0 or index_ >= int(sizeof(indices)/sizeof(EnumType)) ){ return StructType::overflowValue; }
    return indices[index_];
  }
#undef ENUM_FIELD

  static std::string getEnumEntryToStr(int index_){
    if( index_ < 0 or index_ >= getEnumSize() ){ return {"UNNAMED_ENUM"}; }
// MAKE_ENUM -> return only enum entry names
#define ENUM_FIELD(...) TEMP_FIRST_ARG_TO_STR(__VA_ARGS__),
    static const char *names[] = { ENUM_FIELDS };
#undef ENUM_FIELD
    return names[index_];
  }
  static StructType toEnum( const std::string& name_, bool ignoreCase_ = false ){
    int nEntries{getEnumSize()};
    for( int iEntry = 0 ; iEntry < nEntries ; iEntry++ ){
      if( ignoreCase_ ){

        std::string nameLower{name_};
        std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), [](unsigned char c) { return std::tolower(c); });
        std::string enumNameLower{getEnumEntryToStr(iEntry)};
        std::transform(enumNameLower.begin(), enumNameLower.end(), enumNameLower.begin(), [](unsigned char c) { return std::tolower(c); });

        if( nameLower == enumNameLower ){ return getEnumVal(iEntry); }
      }
      else{
        if( name_ == getEnumEntryToStr(iEntry) ){
          return getEnumVal(iEntry);
        }
      }
    }
    return StructType::overflowValue;
  }

  static std::string toString( EnumTypeName value_ ){
    int nEntries{getEnumSize()};
    for( int iEntry = 0 ; iEntry < nEntries ; iEntry++ ){
      if( value_ == getEnumVal(iEntry) ){
        return getEnumEntryToStr(iEntry);
      }
    }
    return {};
  }
  static std::string toString( EnumType value_ ){ return toString( static_cast<EnumTypeName>(value_) ); }
  static inline std::string toString( StructType enum_ ){ return toString( enum_.value ); }
  [[nodiscard]] inline std::string toString() const { return toString( this->value ); }
  friend std::ostream& operator<< (std::ostream& stream, const StructType& this_){ stream << this_.toString(); return stream; }

  static std::string generateEnumFieldsAsString(){
    std::string out{"{ "};
    int nEntries{getEnumSize()};
    for( int iEntry = 0 ; iEntry < nEntries ; iEntry++ ){
      out += getEnumEntryToStr(iEntry);
      if( iEntry < nEntries-1 ){ out += ", "; }
    }
    out += " }";
    return out;
  }

};


// clean up temp macros
#undef TEMP_MERGE
#undef TEMP_MERGE_DEPLOY
#undef TEMP_ENUM_FIELD1
#undef TEMP_ENUM_FIELD2
#undef TEMP_FIRST_ARG
#undef TEMP_FIRST_ARG_TO_STR
#undef TEMP_GET_OVERLOADED_MACRO2

// clean up user macros
#undef ENUM_NAME
#undef ENUM_TYPE
#undef ENUM_FIELD
#undef ENUM_FIELDS
#undef ENUM_OVERFLOW

