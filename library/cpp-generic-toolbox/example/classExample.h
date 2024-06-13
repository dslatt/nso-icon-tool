//
// Created by Nadrino on 08/09/2020.
//

#ifndef CPP_GENERIC_TOOLBOX_CLASSEXAMPLE_H
#define CPP_GENERIC_TOOLBOX_CLASSEXAMPLE_H

#include <iostream>

#define ENUM_NAME MyEnum
#define ENUM_TYPE unsigned int
#define ENUM_OVERFLOW ENUM_FIELD(BadValue, 0xFFFF)
#define ENUM_FIELDS \
  ENUM_FIELD(Case0, 0) \
  ENUM_FIELD(Case1)    \
  ENUM_FIELD(Case2)    \
  ENUM_FIELD(Case33, 33)    \
  ENUM_FIELD(Case34)    \
  ENUM_FIELD(Case35)
#include "GenericToolbox.MakeEnum.h"

class ClassExample{

public:
  ClassExample(){ std::cout << "The name of this class is: " << __CLASS_NAME__ << std::endl; }
  static void MyGreatMethod(){ std::cout << "The name of this method is: " << __METHOD_NAME__ << std::endl; }

};

#endif //CPP_GENERIC_TOOLBOX_CLASSEXAMPLE_H
