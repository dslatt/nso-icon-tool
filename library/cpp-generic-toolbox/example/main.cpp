//
// Created by Nadrino on 28/08/2020.
//

#include <string>
#include <iostream>
#include <thread>

#define WARN_DEPRECATED_FCT

#include "GenericToolbox.String.h"
#include "GenericToolbox.Vector.h"
#include "GenericToolbox.Fs.h"
#include "GenericToolbox.Thread.h" // extension
#include "GenericToolbox.Utils.h" // extension

//#ifdef ENABLE_ROOT_EXTENSION
//#include <GenericToolbox.Root.h>
//#endif

#include "classExample.h"

using namespace std;


int main(){

  // String management
  string str("This is a simple message.");
  cout << str << endl;
  cout << "Does str contains \"simple message\"? " << GenericToolbox::hasSubStr(str, "simple message") << endl;
  cout << "Remove the caps chars: " << GenericToolbox::toLowerCase(str) << endl;
  auto splitVec = GenericToolbox::splitString(str, " ");
  cout << "Those are the words in str: " << GenericToolbox::toString(splitVec);
  cout << "Let's_join_the_vector_element:_" << GenericToolbox::joinVectorString(splitVec, "_") << endl;
  cout << GenericToolbox::formatString("I would %s to create a printf-style string %i, %p", "like", 4, &splitVec) << endl;


  // IO examples
  cout << "Expanded path: ${HOME}/test.txt -> " << GenericToolbox::expandEnvironmentVariables("${HOME}/test.txt") << endl;
  cout << "Expanded path: $HOME/test.txt -> " << GenericToolbox::expandEnvironmentVariables("$HOME/test.txt") << endl;
  cout << "Expanded path: ~/test.txt -> " << GenericToolbox::expandEnvironmentVariables("~/test.txt") << endl;
  cout << "Expanded path: $$/test.txt -> " << GenericToolbox::expandEnvironmentVariables("$$/test.txt") << endl;
  cout << "Current working directory: " << GenericToolbox::getCurrentWorkingDirectory() << endl;
  cout << "Is this a directory? " << GenericToolbox::isDir("../include") << endl;
  cout << "Is this a file? " << GenericToolbox::isFile("../include/GenericToolbox.h") << endl;
  cout << "Its size: " << GenericToolbox::parseSizeUnits(GenericToolbox::getFileSize("../include/GenericToolbox.h")) << endl;
  cout << "Hash of this file: " << GenericToolbox::getFileHash("../include/GenericToolbox.h") << endl;
  cout << "Are they identical?: " << GenericToolbox::areSameFiles("../include/GenericToolbox.h","../include/GenericToolbox_impl.h") << endl;


  // Hardware
  cout << "Process takes this amount of RAM: " << GenericToolbox::parseSizeUnits(GenericToolbox::getProcessMemoryUsage()) << endl;
  cout << "Process has taken this amount of RAM at max: " << GenericToolbox::parseSizeUnits(GenericToolbox::getProcessMaxMemoryUsage()) << endl;

  cout << "getTerminalWidth() = " << GenericToolbox::getTerminalWidth() << endl;

  for(int i = 0 ; i < 1000 ; i++){
    GenericToolbox::displayProgressBar(i, 1000, "Loading...");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  GenericToolbox::ProgressBar::gProgressBar.setEnableRainbowProgressBar(true);
  GenericToolbox::ProgressBar::gProgressBar.setMaxBarLength(36);
  for(int i = 0 ; i < 1000 ; i++){
    GenericToolbox::displayProgressBar(i, 1000, "If you like colors, but not the default bar length:");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  ClassExample example;
  example.MyGreatMethod();

  std::cout << "MyEnum = " << MyEnum::Case1 << ": " << MyEnum::toString(MyEnum::Case1) << std::endl;
  std::cout << "MyEnum = " << MyEnum::Case2 << ": " << MyEnum::toString(MyEnum::Case2) << std::endl;
  MyEnum enumTest = MyEnum::Case33;
  std::cout << "MyEnum = " << enumTest << ": " << enumTest.toString() << std::endl;
  std::cout << "MyEnum = " << MyEnum::Case34 << ": " << MyEnum::toString(MyEnum::Case34) << std::endl;
  std::cout << "MyEnum = " << MyEnum::Case35 << ": " << MyEnum::toString(MyEnum::Case35) << std::endl;

  GenericToolbox::ParallelWorker p;
  p.setNThreads(4); // 3 parallel threads + 1 main
  p.setIsVerbose(true);
  p.addJob("exampleJob", [&p](int iThread_){
    std::cout << "Executing in thread: " << iThread_ << std::endl;
  });
  p.runJob("exampleJob");
  p.removeJob("exampleJob"); // not necessary, but stop the parallel threads if the worker is no longer waiting for jobs (for CPU time saving)

  GenericToolbox::VariablesMonitor v;
  v.addVariable("exp(-i*1E-6)");
  v.addVariable("i*i");

  v.addDisplayedQuantity("VarName");
  v.addDisplayedQuantity("LastAddedValue");
  v.addDisplayedQuantity("Accumulated");
  v.addDisplayedQuantity("AccumulationRate");
  v.addDisplayedQuantity("SlopePerSecond");
  v.addDisplayedQuantity("SlopePerCall");

  for( int i = 0 ; i <= 1000 ; i++ ){
    double sqrI = sqrt(i);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    v.getVariable("exp(-i*1E-6)").addQuantity(std::exp(-i/1000000.));
    v.getVariable("i*i").addQuantity(i*i);
    v.setHeaderString(GET_VAR_NAME_VALUE(i));
    std::cout << v.generateMonitorString(true);
  }
  std::cout << v.generateMonitorString();


  return EXIT_SUCCESS;
}
