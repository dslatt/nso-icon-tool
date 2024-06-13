//
// Created by Nadrino on 08/01/2024.
//

#ifndef CPP_GENERIC_TOOLBOX_UTILS_H
#define CPP_GENERIC_TOOLBOX_UTILS_H


#include "GenericToolbox.Wrappers.h"
#include "GenericToolbox.Vector.h"
#include "GenericToolbox.String.h"
#include "GenericToolbox.Os.h"

#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"


#ifndef CPP_GENERIC_TOOLBOX_BATCH
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_LTCORN "┌"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_LBCORN "└"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_RTCORN "┐"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_RBCORN "┘"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TRIGHT "├"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TLEFT "┤"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TTOP "┬"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TBOT "┴"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_CROSS "┼"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_HLINE "─"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_VLINE "│"
#else
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_LTCORN "#"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_LBCORN "#"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_RTCORN "#"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_RBCORN "#"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TRIGHT "|"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TLEFT "|"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TTOP "-"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TBOT "-"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_CROSS "|"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_HLINE "-"
#define CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_VLINE "|"
#endif

// Declarations
namespace GenericToolbox{
  class InitBaseClass;
  template<class ConfigType> class ConfigBaseClass;
  class ScopedGuard;
  class RawDataArray;
  class TablePrinter;
  class VariableMonitor;
  struct QuantityMonitor;
  class VariablesMonitor;
  class AnyType;
  namespace ProgressBar{ class ProgressBar; }
}


// Implementations

// ProgressBar
namespace GenericToolbox{

#ifndef PROGRESS_BAR_ENABLE_RAINBOW
#define PROGRESS_BAR_ENABLE_RAINBOW 0
#endif

#ifndef PROGRESS_BAR_LENGTH
#define PROGRESS_BAR_LENGTH 36
#endif

#ifndef PROGRESS_BAR_SHOW_SPEED
#define PROGRESS_BAR_SHOW_SPEED 1
#endif

#ifndef PROGRESS_BAR_REFRESH_DURATION_IN_MS
// 33 ms per frame = 0.033 seconds per frame = 1/30 sec per frame = 30 fps
#define PROGRESS_BAR_REFRESH_DURATION_IN_MS 33
#endif

#ifndef PROGRESS_BAR_FILL_TAG
  // multi-char is possible
#define PROGRESS_BAR_FILL_TAG "#"
#endif

  template<typename T, typename TT> static void displayProgressBar( const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "", bool forcePrint_ = false);
  template<typename T, typename TT> static std::string getProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "", bool forcePrint_ = false );
  template<typename T, typename TT> static std::string generateProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_ = "" );
  template<typename T, typename TT> static bool showProgressBar(const T& iCurrent_, const TT& iTotal_);
  static void resetLastDisplayedValue();
  static void waitProgressBar(unsigned int nbMilliSecToWait_, const std::string &progressTitle_ = "Waiting...");

  namespace ProgressBar{
    class ProgressBar{

    public:
      inline ProgressBar() = default;
      inline virtual ~ProgressBar() = default;

      inline void setMaxBarLength(int maxBarLength){ _maxBarLength_ = maxBarLength; }
      inline void setDisableVt100Cmd(bool disableVt100Cmd_){ _disableVt100Cmd_ = disableVt100Cmd_; }
      inline void setEnableRainbowProgressBar(bool enableRainbowProgressBar){_enableRainbowProgressBar_ = enableRainbowProgressBar;}

      inline void resetLastDisplayedValue();

      template<typename T, typename TT> inline std::string generateProgressBarStr(T iCurrent_, TT iTotal_, const std::string &title_ = "");
      template<typename T, typename TT> inline bool showProgressBar(T iCurrent_, TT iTotal_);
      template<typename T, typename TT> inline std::string getProgressBarStr(T iCurrent_, TT iTotal_, const std::string &title_ = "", bool forcePrint_ = false);
      template<typename T, typename TT> inline void displayProgressBar(T iCurrent_, TT iTotal_, const std::string &title_ = "", bool forcePrint_ = false);

    protected:
      inline std::string _generateProgressBarStr(double iCurrent_, double iTotal_, const std::string &title_);

    private:
      bool _debugMode_{false};
      bool _disableVt100Cmd_{false};
      bool _enableRainbowProgressBar_{PROGRESS_BAR_ENABLE_RAINBOW};
      bool _displaySpeed_{PROGRESS_BAR_SHOW_SPEED};
      int _maxBarLength_{PROGRESS_BAR_LENGTH};
      size_t _refreshRateInMilliSec_{PROGRESS_BAR_REFRESH_DURATION_IN_MS};
      std::string _fillTag_{PROGRESS_BAR_FILL_TAG};
      std::ostream* _outputStreamPtr_{&std::cout};

      int _lastDisplayedPercentValue_{-1};
      int _lastDisplayedValue_ = {-1};
      double _lastDisplayedSpeed_{0};
      std::chrono::high_resolution_clock::time_point _lastDisplayedTimePoint_{std::chrono::high_resolution_clock::now()};
      std::thread::id _selectedThreadId_{std::this_thread::get_id()}; // get the main thread id

      //buffers
      std::chrono::high_resolution_clock::time_point _timePointBuffer_{};
      long _deltaTimeMilliSec_{};
      double _timeIntervalBuffer_{};

    };

    static ProgressBar gProgressBar;

    template<typename T, typename TT> inline std::string ProgressBar::generateProgressBarStr(T iCurrent_, TT iTotal_, const std::string &title_) {
      return this->_generateProgressBarStr(double(iCurrent_), double(iTotal_), title_);
    }
    template<typename T, typename TT> inline bool ProgressBar::showProgressBar(T iCurrent_, TT iTotal_) {

      //    if( // Only the main thread
      //      this->_selectedThreadId_ != std::this_thread::get_id()
      //      ){
      //      return false;
      //    }

      _deltaTimeMilliSec_ = std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::high_resolution_clock::now() - this->_lastDisplayedTimePoint_
      ).count();
      if ( // REQUIRED TO PRINTOUT
          iCurrent_ == 0 // First call
          or this->_lastDisplayedPercentValue_ == -1 // never printed before
          or _deltaTimeMilliSec_ >= long(this->_refreshRateInMilliSec_)
          or iCurrent_ + 1 >= T(iTotal_) // last entry (mandatory to print at least once: need to print endl)
          ) {

        int percent = int(std::round(double(iCurrent_) / double(iTotal_) * 100.));

        if (percent >= 100) { percent = 100; }
        else if (percent < 0) percent = 0;

        if ( // EXCLUSION CASES
            percent == this->_lastDisplayedPercentValue_ // already printed
            ) {
          if (this->_debugMode_) {
            std::cout << "Print PBar NOT Ok:" << std::endl;
            std::cout << "percent == this->lastDisplayedPercentValue" << std::endl;
          }
          return false;
        }

        if (this->_debugMode_) {
          std::cout << "Print PBar Ok:" << std::endl;
          std::cout << "percent = " << percent << std::endl;
          std::cout << "iCurrent_ = " << iCurrent_ << std::endl;
          std::cout << "iTotal_ = " << iTotal_ << std::endl;
          std::cout << "this->lastDisplayedPercentValue = " << this->_lastDisplayedPercentValue_ << std::endl;
          std::cout << "this->refreshRateInMilliSec = " << this->_refreshRateInMilliSec_ << std::endl;
        }

        // OK!
        return true;
      }

      return false;
    }
    template<typename T, typename TT> inline std::string ProgressBar::getProgressBarStr(T iCurrent_, TT iTotal_, const std::string &title_, bool forcePrint_) {
      if (forcePrint_ or this->showProgressBar(iCurrent_, iTotal_)) {
        return this->generateProgressBarStr(iCurrent_, iTotal_, title_);
      }
      return {};
    }
    template<typename T, typename TT> inline void ProgressBar::displayProgressBar(T iCurrent_, TT iTotal_, const std::string &title_, bool forcePrint_) {
      if (forcePrint_ or this->showProgressBar(iCurrent_, iTotal_)) {
        (*this->_outputStreamPtr_) << GenericToolbox::generateProgressBarStr(iCurrent_, iTotal_, title_);
      }
    }
    inline void ProgressBar::resetLastDisplayedValue() {
      this->_lastDisplayedValue_ = -1;
      this->_lastDisplayedPercentValue_ = -1;
    }

    // protected
    inline std::string ProgressBar::_generateProgressBarStr(double iCurrent_, double iTotal_, const std::string &title_) {

      _lastDisplayedPercentValue_ = int(std::round(iCurrent_ / iTotal_ * 100.));
      if (_lastDisplayedPercentValue_ > 100) { _lastDisplayedPercentValue_ = 100; }
      else if (_lastDisplayedPercentValue_ < 0) { _lastDisplayedPercentValue_ = 0; }

      int termWidth{0};
#ifndef CPP_GENERIC_TOOLBOX_BATCH
      // this returns non-zero value if it measurable
      termWidth = GenericToolbox::getTerminalWidth();
#endif

      std::stringstream ssPrefix;

      if (not title_.empty()){
        auto titleLines = GenericToolbox::splitString(title_, "\n");

        for( size_t iLine = 0 ; iLine < titleLines.size()-1 ; iLine++ ){
          if( termWidth == 0 ){
            // print the whole line
            ssPrefix << titleLines[iLine] << std::endl;
          }
          else{
            // print the line in the available space
            std::string buffer{titleLines[iLine]};
            while( int(GenericToolbox::getPrintSize(buffer)) >= termWidth ){
              // removing the last character and test if the printed size fit the terminal window.
              buffer = buffer.substr(0, buffer.size()-1);
            }
            ssPrefix << buffer << std::endl;
          }
        }

        ssPrefix << titleLines.back() << " ";
      }

      std::stringstream ssTail;
      ssTail << GenericToolbox::padString(std::to_string(_lastDisplayedPercentValue_), 3, ' ') << "%";

      _timePointBuffer_ = std::chrono::high_resolution_clock::now();
      if (_displaySpeed_) {

        // How much time since last call?
        _timeIntervalBuffer_ = double(std::chrono::duration_cast<std::chrono::milliseconds>(
            _timePointBuffer_ - _lastDisplayedTimePoint_).count()) / 1000.;
        if (_timeIntervalBuffer_ != 0) {
          // How much iterations since last call?
          _lastDisplayedSpeed_ = iCurrent_ - _lastDisplayedValue_;
          // Count per second
          _lastDisplayedSpeed_ /= _timeIntervalBuffer_;
        }

        // When iCurrent go back to zero -> _lastDisplayedValue_ makes no sense
        if (_lastDisplayedSpeed_ < 0) _lastDisplayedSpeed_ = 0;

        ssTail << " (";
        ssTail << GenericToolbox::padString(GenericToolbox::parseIntAsString(int(_lastDisplayedSpeed_)), 5, ' ');
        ssTail << " it/s)";
      }
      _lastDisplayedValue_ = int( iCurrent_ );
      _lastDisplayedTimePoint_ = _timePointBuffer_;


      // test if the bar is too wide wrt the prompt width
      int displayedBarLength = _maxBarLength_;
      if (termWidth > 0) { // terminal width is measurable

        size_t lastLinePos = ssPrefix.str().find_last_of('\n');
        if (lastLinePos == size_t(-1)) lastLinePos = 0;

        size_t lastLineLength = GenericToolbox::getPrintSize(ssPrefix.str().substr(lastLinePos));
        if (displayedBarLength > 0) {
          lastLineLength += 2; // []
          lastLineLength += displayedBarLength;
          lastLineLength += 1; // space before tail
        }
        lastLineLength += ssTail.str().size();
        lastLineLength += 1; // 1 extra space is necessary to std::endl

        int remainingSpaces = termWidth;
        remainingSpaces -= int(lastLineLength);

        if (remainingSpaces < 0) {
          if (displayedBarLength >= 0) {
            // ok, can take some extra space in the bar
            displayedBarLength -= std::abs(remainingSpaces);
            if (displayedBarLength < 12) {
              displayedBarLength = 0;
              remainingSpaces += 2; // get back the [] of the pBar
            }
            remainingSpaces += (this->_maxBarLength_ - displayedBarLength);
          }
        }

        // if it's still to big, cut the title
        if (remainingSpaces < 0) {
          std::string cutPrefix = ssPrefix.str().substr(0, int(ssPrefix.str().size()) - std::abs(remainingSpaces) - 3);
          ssPrefix.str("");
          ssPrefix << cutPrefix;
          ssPrefix << static_cast<char>(27) << "[0m" << "...";
        }
      } else {
        displayedBarLength = 0;
      }

      std::stringstream ssProgressBar;
      ssProgressBar << ssPrefix.str();

      if (displayedBarLength > 0) {
        int nbTags = _lastDisplayedPercentValue_ * displayedBarLength / 100;
        int nbSpaces = displayedBarLength - nbTags;

        std::string padProgressBar;
        for (int iTag = 0; iTag < nbTags; iTag++) {
          padProgressBar += this->_fillTag_[iTag % this->_fillTag_.size()];
        }
        padProgressBar += GenericToolbox::repeatString(" ", nbSpaces);

        if (_enableRainbowProgressBar_) {
          padProgressBar = GenericToolbox::makeRainbowString(padProgressBar, false);
        }

        ssProgressBar << "[" << padProgressBar << "] ";
      }

      ssProgressBar << ssTail.str();

      if( not _disableVt100Cmd_ ){
        ssProgressBar << std::endl; // always jump line to force flush on screen
#ifndef CPP_GENERIC_TOOLBOX_BATCH
        auto nLines = GenericToolbox::getNLines(ssProgressBar.str());
        if (_lastDisplayedPercentValue_ != 100) {
          // those commands won't be flushed until a new print is called:
          // pull back to cursor on the line of the progress bar
          ssProgressBar << static_cast<char>(27) << "[" << nLines - 1 << "F";
          // Clear the line and add "\r" since a logger (in charge of printing this string)
          // might intercept it to trigger a print of a line header
          ssProgressBar << static_cast<char>(27) << "[1K" << "\r"; // trick to clear
        }
#endif
      }


      if (this->_debugMode_) {
        std::cout << "New timestamp: " << this->_lastDisplayedTimePoint_.time_since_epoch().count() << std::endl;
        std::cout << "this->lastDisplayedValue: " << this->_lastDisplayedValue_ << std::endl;
        std::cout << "this->lastDisplayedPercentValue: " << this->_lastDisplayedPercentValue_ << std::endl;
      }

      return ssProgressBar.str();
    }
  }

  template<typename T, typename TT> static std::string generateProgressBarStr( const T& iCurrent_, const TT& iTotal_, const std::string &title_ ){
    return ProgressBar::gProgressBar.template generateProgressBarStr(iCurrent_, iTotal_, title_);
  }
  template<typename T, typename TT> static bool showProgressBar(const T& iCurrent_, const TT& iTotal_){
    return ProgressBar::gProgressBar.template showProgressBar(iCurrent_, iTotal_);
  }
  template<typename T, typename TT> static std::string getProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_, bool forcePrint_ ){
    return ProgressBar::gProgressBar.template getProgressBarStr(iCurrent_, iTotal_, title_, forcePrint_);
  }
  template<typename T, typename TT> static void displayProgressBar(const T& iCurrent_, const TT& iTotal_, const std::string &title_, bool forcePrint_) {
    return ProgressBar::gProgressBar.template displayProgressBar(iCurrent_, iTotal_, title_, forcePrint_);
  }
  static void resetLastDisplayedValue(){
    ProgressBar::gProgressBar.resetLastDisplayedValue();
  }
  static void waitProgressBar(unsigned int nbMilliSecToWait_, const std::string &progressTitle_) {

    auto anchorTimePoint = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds totalDurationToWait(nbMilliSecToWait_*1000);
    std::chrono::microseconds cumulatedDuration(0);
    std::chrono::microseconds loopUpdateMaxFrequency(nbMilliSecToWait_); // 1000x faster than the whole time

    GenericToolbox::displayProgressBar( 0, totalDurationToWait.count(), progressTitle_);
    while( true ){
      std::this_thread::sleep_for( loopUpdateMaxFrequency );
      cumulatedDuration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - anchorTimePoint);
      if( cumulatedDuration >= totalDurationToWait ){
        return;
      }
      else{
        GenericToolbox::displayProgressBar( cumulatedDuration.count(), totalDurationToWait.count(), progressTitle_);
      }
    }
    GenericToolbox::displayProgressBar( totalDurationToWait.count(), totalDurationToWait.count(), progressTitle_);

  }

}

// InitBaseClass
namespace GenericToolbox{
  class InitBaseClass{

  public:
    // Common structure
    inline InitBaseClass() = default;
    inline virtual ~InitBaseClass() = default;

    virtual inline void initialize();
    inline void unInitialize(){ _isInitialized_ = false; }

    [[nodiscard]] inline bool isInitialized() const{ return _isInitialized_; }

    inline void throwIfInitialized(const std::string& functionName_ = {}) const;
    inline void throwIfNotInitialized(const std::string& functionName_ = {}) const;

  protected:
    // where the derivative classes will specify (although override is optional)
    inline virtual void initializeImpl(){};

  private:
    bool _isInitialized_{false};

  };

  inline void InitBaseClass::initialize() {
    if( _isInitialized_ ) throw std::logic_error("Can't re-initialize while already done. Call unInitialize() before.");
    this->initializeImpl();
    _isInitialized_ = true;
  }

  inline void InitBaseClass::throwIfInitialized(const std::string& functionName_) const {
    if( _isInitialized_ ){
      if( functionName_.empty() ){
        throw std::runtime_error(__METHOD_NAME__ + ": Object already initialized.");
      }
      else{
        throw std::runtime_error(__METHOD_NAME__ + "Can't \""+functionName_+"\" while already initialized.");
      }
    }
  }
  inline void InitBaseClass::throwIfNotInitialized( const std::string& functionName_ ) const{
    if( not _isInitialized_ ){
      if( functionName_.empty() ){
        throw std::runtime_error(__METHOD_NAME__ + ": Object not initialized.");
      }
      else{
        throw std::runtime_error(__METHOD_NAME__ + ": Can't \""+functionName_+"\" while not initialized.");
      }
    }
  }
}

// ConfigBaseClass
namespace GenericToolbox{
  template<class ConfigType> class ConfigBaseClass : public InitBaseClass {

  public:
    // Common structure
    inline ConfigBaseClass() = default;
    inline ~ConfigBaseClass() override = default;

    inline virtual void setConfig(const ConfigType& config_);

    inline void readConfig();
    inline void readConfig(const ConfigType& config_);

    inline void initialize() override;

    [[nodiscard]] inline bool isConfigReadDone() const { return _isConfigReadDone_; }
    inline const ConfigType &getConfig() const { return _config_; }

  protected:
    // where the derivative classes will specify (although override is optional)
    inline virtual void readConfigImpl(){};

    // Can be accessed by derivative classes
    ConfigType _config_{};

  private:
    bool _isConfigReadDone_{false};

  };

  template<class ConfigType> inline void ConfigBaseClass<ConfigType>::setConfig(const ConfigType &config_) {
    if( this->isInitialized() ) throw std::logic_error("Can't read the config while already initialized.");
    _config_ = config_;
  }

  template<class ConfigType> inline void ConfigBaseClass<ConfigType>::readConfig() {
    if( this->isInitialized() ) throw std::logic_error("Can't read the config while already initialized.");
    _isConfigReadDone_ = true;
    this->readConfigImpl();
  }
  template<class ConfigType> inline void ConfigBaseClass<ConfigType>::readConfig(const ConfigType& config_){
    this->setConfig(config_);
    this->readConfig();
  }

  template<class ConfigType> inline void ConfigBaseClass<ConfigType>::initialize() {
    if( this->isInitialized() ) throw std::logic_error("Can't re-initialize while already done. Call unInitialize() before.");
    if( not _isConfigReadDone_ ) this->readConfig();
    InitBaseClass::initialize();
  }
}

// ScopedGuard
namespace GenericToolbox{
  class ScopedGuard{

  public:
    using Action = std::function<void()>;

    explicit ScopedGuard(const Action& onCreate_, Action onDelete_) : _onDelete_{std::move(onDelete_)} { if( onCreate_ ){ onCreate_(); } }
    ~ScopedGuard() { fireOnDelete(); }

    void fireOnDelete(){ if( _onDelete_ ){ _onDelete_(); } this->dismiss(); }
    void dismiss(){ _onDelete_ = []{}; }

    // non-copiable
    ScopedGuard() = default;
    ScopedGuard( ScopedGuard const& ) = delete;
    ScopedGuard( ScopedGuard&& ) = default;
    auto operator=( ScopedGuard&& ) -> ScopedGuard& = default;
    auto operator=( ScopedGuard const& ) -> ScopedGuard& = delete;

  private:
    Action _onDelete_{};

  };
}

// RawDataArray
namespace GenericToolbox{
  class RawDataArray{

  public:
    inline RawDataArray() = default;
    inline virtual ~RawDataArray() = default;

    inline void reset();

    inline std::vector<unsigned char> &getRawDataArray();
    inline const std::vector<unsigned char> &getRawDataArray() const;
    inline void writeMemoryContent( const void *address_, size_t dataSize_ );
    inline void writeMemoryContent( const void *address_, size_t dataSize_, size_t byteOffset_ );

    template<typename T>
    inline void writeRawData( const T &data ); // auto incrementing "_currentOffset_"
    template<typename T>
    inline void writeRawData( const T &data, size_t byteOffset_ );

    void resetCurrentByteOffset();

    void lockArraySize();

    void unlockArraySize();

  private:
    bool _lockArraySize_{false};
    size_t _currentByteOffset_{0};
    std::vector<unsigned char> rawData{};

  };

  inline void RawDataArray::reset(){
    rawData = std::vector<unsigned char>();
    resetCurrentByteOffset();
    unlockArraySize();
  }
  inline std::vector<unsigned char>& RawDataArray::getRawDataArray(){
    return rawData;
  }
  inline const std::vector<unsigned char>& RawDataArray::getRawDataArray() const{
    return rawData;
  }
  inline void RawDataArray::writeMemoryContent(const void* address_, size_t dataSize_){
    this->writeMemoryContent(address_, dataSize_, _currentByteOffset_);
    _currentByteOffset_+=dataSize_;
  }
  inline void RawDataArray::writeMemoryContent(const void* address_, size_t dataSize_, size_t byteOffset_){
    if(rawData.size() < byteOffset_ + dataSize_ ){
      if( _lockArraySize_ ) throw std::runtime_error("Can't resize raw array since _lockArraySize_ is true.");
      rawData.resize(byteOffset_ + dataSize_);
    }
    memcpy(&rawData[byteOffset_], address_, dataSize_);
  }
  template<typename T> inline void RawDataArray::writeRawData(const T& data){
    this->writeMemoryContent(&data, sizeof(data));
  }
  template<typename T> inline void RawDataArray::writeRawData(const T& data, size_t byteOffset_){
    this->writeMemoryContent(&data, sizeof(data), byteOffset_);
  }
  inline void RawDataArray::resetCurrentByteOffset(){
    _currentByteOffset_=0;
  }
  inline void RawDataArray::lockArraySize(){
    _lockArraySize_=true;
  }
  inline void RawDataArray::unlockArraySize(){
    _lockArraySize_=false;
  }
}

// TablePrinter
namespace GenericToolbox{
  class TablePrinter{

  public:
    inline TablePrinter() = default;

    inline virtual ~TablePrinter() = default;

    inline void reset();

    inline void fillTable( const std::vector<std::vector<std::string>> &tableLines_ );

    inline size_t setColTitles( const std::vector<std::string> &colTitles_ );

    inline size_t addColTitle( const std::string &colTitle_ );

    inline size_t addTableLine( const std::vector<std::string> &colValues_ = std::vector<std::string>(),
                                const std::string &colorCode_ = "" );

    inline void setTableContent( size_t colIndex_, size_t rowIndex_, const std::string &value_ );

    inline int getNbRows(){ return int(_colTitleList_.size()); }

    inline std::string generateTableString();

    inline void printTable(){ std::cout << generateTableString() << std::endl; }

    inline void setColorBuffer( const std::string &colorBuffer_ ){ _colorBuffer_ = colorBuffer_; }

  private:
    std::vector<std::string> _colTitleList_{};
    std::vector<std::vector<std::string>> _tableContent_{};
    std::vector<int> _colMaxWidthList_{};

    std::vector<std::string> _lineBuffer_;
    std::string _colorBuffer_;
    size_t _currentRow_{0};

    std::string _currentEntryBuffer_{};
    std::vector<std::string> _currentLineBuffer_{};

  public:
    typedef enum{
      Reset = 0,
      NextColumn,
      NextLine
    }
        Action;

    template<typename T>
    inline TablePrinter &operator<<( const T &data );

    inline TablePrinter &operator<<( Action action_ );

  };

  void TablePrinter::reset(){
    _colTitleList_.clear();
    _tableContent_.clear();
    _colMaxWidthList_.clear();
  }
  void TablePrinter::fillTable(const std::vector<std::vector<std::string>> &tableLines_){
    this->reset();
    if( tableLines_.empty() ) return;
    this->setColTitles(tableLines_[0]);
    for( size_t iLine = 1 ; iLine < tableLines_.size() ; iLine++ ){
      this->addTableLine(tableLines_[iLine]);
    }
  }
  inline size_t TablePrinter::setColTitles(const std::vector<std::string>& colTitles_){
    if( colTitles_.empty() ) throw std::runtime_error("colTitles_ is empty.");
    for( auto& colTitle : colTitles_ ){
      this->addColTitle(colTitle);
    }
    return _colTitleList_.size()-1;
  }
  size_t TablePrinter::addColTitle(const std::string& colTitle_){
    _colTitleList_.emplace_back(colTitle_);
    _tableContent_.emplace_back();
    _colMaxWidthList_.emplace_back(-1);
    return _colTitleList_.size()-1;
  }
  size_t TablePrinter::addTableLine(const std::vector<std::string>& colValues_, const std::string&  colorCode_){
    size_t rowIndex{0};
    size_t colIndex{0};
    for( auto& colTable : _tableContent_ ){
      colTable.emplace_back();
      if( not colValues_.empty() ) {
        if( not colorCode_.empty() ) colTable.back() += colorCode_;
        colTable.back() += colValues_[colIndex++];
        if( not colorCode_.empty() ) colTable.back() += GenericToolbox::ColorCodes::resetColor;
      }
      rowIndex = colTable.size()-1;
    }
    return rowIndex;
  }
  void TablePrinter::setTableContent(size_t colIndex_, size_t rowIndex_, const std::string& value_){
    if( colIndex_ >= _tableContent_.size() ) throw std::runtime_error("invalid col index");
    if( rowIndex_ >= _tableContent_[colIndex_].size() ) throw std::runtime_error("invalid row index");
    _tableContent_[colIndex_][rowIndex_] = value_;
  }
  std::string TablePrinter::generateTableString(){
    std::stringstream ss;

    std::vector<size_t> paveColList(_tableContent_.size(),0);
    for( int iCol = 0 ; iCol < int(_colTitleList_.size()) ; iCol++ ){
      paveColList[iCol] = GenericToolbox::getPrintSize(_colTitleList_[iCol]);
      for( int iRow = 0 ; iRow < int(_tableContent_[iCol].size()) ; iRow++ ){
        paveColList[iCol] = std::max(paveColList[iCol], GenericToolbox::getPrintSize(_tableContent_[iCol][iRow]));
      }
    }

    // ┌───────────┬───────────────┬──────────────────┐
    // or
    // #----------------------------------------------#
    ss << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_LTCORN;
    for( int iCol = 0 ; iCol < int(_colTitleList_.size())-1 ; iCol++ ){
      ss << GenericToolbox::repeatString(CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_HLINE, paveColList[iCol]+2);
      ss << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TTOP;
    }
    ss << GenericToolbox::repeatString(CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_HLINE, paveColList.back()+2) << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_RTCORN<< std::endl;

    // │ Likelihood │ Current Value │ Avg. Slope /call │
    // or
    // | Likelihood | Current value | Avg. Slope /call |
    ss << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_VLINE;
    for( int iCol = 0 ; iCol < int(_colTitleList_.size()) ; iCol++ ){
      ss << " " << GenericToolbox::padString(_colTitleList_[iCol], paveColList[iCol]);
      ss << " " << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_VLINE;
    }
    ss << std::endl;

    // ├───────────┼───────────────┼──────────────────┤
    // or
    // |-----------|---------------|------------------|
    ss << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TRIGHT;
    for( int iCol = 0 ; iCol < int(_colTitleList_.size())-1 ; iCol++ ){
      ss << GenericToolbox::repeatString(CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_HLINE, paveColList[iCol]+2);
      ss << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_CROSS;
    }
    ss << GenericToolbox::repeatString(CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_HLINE, paveColList.back()+2) << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TLEFT << std::endl;

    // │     Total │ 9.9296422e-13 │             nanP │
    // or
    // |     Total | 9.9296422e-13 |             nanP |
    if( not _tableContent_.empty() ){
      for( int iRow = 0 ; iRow < int(_tableContent_[0].size()) ; iRow++ ){
        ss << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_VLINE;
        for( int iCol = 0 ; iCol < int(_colTitleList_.size()) ; iCol++ ){
          ss << " " << GenericToolbox::padString(_tableContent_[iCol][iRow], paveColList[iCol]);
          ss << " " << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_VLINE;
        }
        ss << std::endl;
      }
    }


    // └───────────┴───────────────┴──────────────────┘
    // or
    // #----------------------------------------------#
    ss << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_LBCORN;
    for( int iCol = 0 ; iCol < int(_colTitleList_.size())-1 ; iCol++ ){
      ss << GenericToolbox::repeatString(CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_HLINE, paveColList[iCol]+2);
      ss << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_TBOT;
    }
    ss << GenericToolbox::repeatString(CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_HLINE, paveColList.back()+2) << CPP_GENERIC_TOOLBOX_TABLE_PRINTER_IMPL_H_RBCORN;

    return ss.str();
  }
  template<typename T> inline TablePrinter &TablePrinter::operator<<(const T &data){
    // just fills a buffer

    std::stringstream ss;
    ss << data;
    _currentEntryBuffer_ += ss.str();

    return *this;
  }
  inline TablePrinter &TablePrinter::operator<<(Action action_){

    if(action_ == Action::Reset ){
      this->reset();
      _currentRow_ = 0;
      _currentEntryBuffer_ = "";
      _currentLineBuffer_.clear();
    }
    if(
        action_ == Action::NextColumn or
        action_ == Action::NextLine
        ){
      if( _colTitleList_.empty() or _lineBuffer_.size() < _colTitleList_.size() ){
        // drop the string buffer to the vector buffer
        if( not _colorBuffer_.empty() ){
          _lineBuffer_.emplace_back( _colorBuffer_ + _currentEntryBuffer_ + GenericToolbox::ColorCodes::resetColor );
        }
        else{
          _lineBuffer_.emplace_back( _currentEntryBuffer_ );
        }
      }

      // clear the buffer
      _currentEntryBuffer_ = "";
      _currentRow_++;
    }
    if( action_ == Action::NextLine or ( not _colTitleList_.empty() and _lineBuffer_.size() == _colTitleList_.size() ) ){
      if( _colTitleList_.empty() ){ this->setColTitles(_lineBuffer_); }
      else                        {
        this->addTableLine(_lineBuffer_);
      }
      _lineBuffer_.clear();
      _colorBuffer_ = "";
      _currentRow_ = 0;
    }

    return *this;
  }
}

// VariableMonitor
namespace GenericToolbox{
  class VariableMonitor{

  public:
    inline VariableMonitor() = default; // for vector
    inline explicit VariableMonitor( std::string name_ ) : _name_{std::move(name_)}{ this->reset(); }

    [[nodiscard]] inline const std::string &getName() const { return _name_; }
    [[nodiscard]] inline double getTotalAccumulated() const { return _totalAccumulated_; }
    [[nodiscard]] inline double getLastValue() const;

    inline void reset();
    inline void addQuantity( const double &quantityToAdd_ );
    inline double evalTotalGrowthRate();

    [[nodiscard]] inline double evalCallGrowthRate() const;
    [[nodiscard]] inline double evalCallGrowthRatePerSecond() const;


  private:
    std::string _name_{};

    double _totalAccumulated_{0};
    double _lastTotalAccumulated_{0};
    std::chrono::high_resolution_clock::time_point _lastTotalRateEval_;

    size_t _currentHistorySize_{0};
    std::vector<double> _addToAccumulatorHistory_{};
    std::vector<std::chrono::high_resolution_clock::time_point> _addToAccumulatorTimeHistory_{};
    size_t _currentSlotIndex_{0};

  };

  inline void VariableMonitor::reset(){
    _totalAccumulated_ = 0;
    _currentHistorySize_ = 0;
    _addToAccumulatorHistory_.clear();
    _addToAccumulatorHistory_.resize(20, 0); // 20 slots
    _addToAccumulatorTimeHistory_.resize(20); // 20 slots
    _lastTotalRateEval_ = std::chrono::high_resolution_clock::now();
  }
  inline void VariableMonitor::addQuantity(const double& quantityToAdd_){
    _addToAccumulatorHistory_[_currentSlotIndex_] = quantityToAdd_;
    _addToAccumulatorTimeHistory_[_currentSlotIndex_] = std::chrono::high_resolution_clock::now();
    if( _currentHistorySize_ < _addToAccumulatorHistory_.size() ) _currentHistorySize_++;
    _currentSlotIndex_++;
    if( _currentSlotIndex_ == _addToAccumulatorHistory_.size() ) _currentSlotIndex_ = 0; // did a cycle
    _totalAccumulated_ += quantityToAdd_;
  }
  inline double VariableMonitor::getLastValue() const{
    if( _currentSlotIndex_ == 0 ) return _addToAccumulatorHistory_.back();
    else return _addToAccumulatorHistory_.at(_currentSlotIndex_-1);
  }
  inline double VariableMonitor::evalTotalGrowthRate(){
    double output = (_totalAccumulated_ - _lastTotalAccumulated_);
    output /= std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - _lastTotalRateEval_
    ).count();
    output *= 1000.;

    _lastTotalRateEval_ = std::chrono::high_resolution_clock::now();
    _lastTotalAccumulated_ = _totalAccumulated_;
    return output;
  }
  inline double VariableMonitor::evalCallGrowthRate() const{
    if( _currentHistorySize_ == 0 ) return 0;
    std::vector<double> orderedAddToAccumulatorHistory(_currentHistorySize_, 0);
    size_t latestIndex = _currentSlotIndex_;
    if( _currentHistorySize_ != _addToAccumulatorHistory_.size() ){
      latestIndex = 0;
    }
    size_t slotIndex = latestIndex;
    size_t orderedSlotIndex = 0;
    do{
      orderedAddToAccumulatorHistory.at(orderedSlotIndex) = _addToAccumulatorHistory_.at(slotIndex);
      slotIndex++;
      orderedSlotIndex++;
      if( slotIndex == _addToAccumulatorHistory_.size() ) slotIndex = 0;
    }
    while( orderedSlotIndex < orderedAddToAccumulatorHistory.size() );
    return GenericToolbox::getAveragedSlope(orderedAddToAccumulatorHistory);
  }
  inline double VariableMonitor::evalCallGrowthRatePerSecond() const{
    if( _currentHistorySize_ == 0 ) return 0;
    std::vector<double> orderedAddToAccumulatorHistory(_currentHistorySize_, 0);
    std::vector<size_t> deltaTimes(_currentHistorySize_, 0);
    size_t latestIndex = _currentSlotIndex_;
    if( _currentHistorySize_ != _addToAccumulatorHistory_.size() ){
      latestIndex = 0;
    }
    auto latestTp = _addToAccumulatorTimeHistory_.at(latestIndex);
    size_t slotIndex = latestIndex;
    size_t orderedSlotIndex = 0;
    do{
      orderedAddToAccumulatorHistory.at(orderedSlotIndex) = _addToAccumulatorHistory_.at(slotIndex);
      deltaTimes.at(orderedSlotIndex) = std::chrono::duration_cast<std::chrono::microseconds>(
          _addToAccumulatorTimeHistory_.at(slotIndex) - latestTp
      ).count();
      slotIndex++;
      orderedSlotIndex++;
      if( slotIndex == _addToAccumulatorHistory_.size() ) slotIndex = 0;
    }
    while( orderedSlotIndex < orderedAddToAccumulatorHistory.size() );
    return GenericToolbox::getAveragedSlope(orderedAddToAccumulatorHistory, deltaTimes);
  }
}

// QuantityMonitor
namespace GenericToolbox{
  struct QuantityMonitor{
    QuantityMonitor() = default;
    QuantityMonitor( std::string name_, std::string title_,
                     std::function<std::string( VariableMonitor & )> evalFunction_, bool isExpandablePadSize_ = true ) :
        name(std::move(name_)), title(std::move(title_)), isExpandablePadSize(isExpandablePadSize_),
        evalFunction(std::move(evalFunction_)){};
    std::string name{}; // kept for reference
    std::string title{};
    bool isExpandablePadSize{true};
    std::function<std::string( VariableMonitor & )> evalFunction{};
  };
}

// VariablesMonitor
namespace GenericToolbox{
  class VariablesMonitor{

  public:
    inline VariablesMonitor();
    inline virtual ~VariablesMonitor() = default;

    inline void setFooterString( const std::string &footerString ){ _footerString_ = footerString; }
    inline void setHeaderString( const std::string &headerString ){ _headerString_ = headerString; }
    inline void setMaxRefreshRateInMs( long long int maxRefreshRateInMs ){ _maxRefreshRateInMs_ = maxRefreshRateInMs; }

    inline long long int getMaxRefreshRateInMs() const;

    inline void addDisplayedQuantity( const std::string &quantityName_ );
    inline void defineNewQuantity( const QuantityMonitor &quantity_ );

    inline VariableMonitor &getVariable( const std::string &name_ );
    inline QuantityMonitor &getQuantity( const std::string &quantityName_ );

    inline bool isGenerateMonitorStringOk() const;

    inline void addVariable( const std::string &name_ );
    inline void clearDisplayQuantityList(){ _displayQuantityIndexList_.clear(); }

    inline std::string generateMonitorString( bool trailBackCursor_ = false, bool forceGenerate_ = false );

  private:
    long long int _maxRefreshRateInMs_{500}; // 1/0.033 = 30 fps, 500 = 1/0.500 = 2 fps
    std::chrono::high_resolution_clock::time_point _lastGeneratedMonitorStringTime_{};
    std::string _headerString_{};
    std::string _footerString_{};
    std::vector<VariableMonitor> _varMonitorList_{};
    std::vector<QuantityMonitor> _quantityMonitorList_{};
    std::vector<size_t> _displayQuantityIndexList_{};
    std::vector<size_t> _basedPaddingList_{};

    TablePrinter _tablePrinter_{};
  };

  inline VariablesMonitor::VariablesMonitor(){
    this->defineNewQuantity({ "VarName", "Variable",  [](VariableMonitor& v){ return v.getName(); } });
    this->defineNewQuantity({ "LastAddedValue", "Last Value", [](VariableMonitor& v){ return parseUnitPrefix(v.getLastValue(), 8); } });
    this->defineNewQuantity({ "Accumulated", "Accumulated", [](VariableMonitor& v){ return parseUnitPrefix(v.getTotalAccumulated(), 8); } });
    this->defineNewQuantity({ "AccumulationRate", "Acc. Rate /s", [](VariableMonitor& v){ return parseUnitPrefix(v.evalTotalGrowthRate(), 8); } });
    this->defineNewQuantity({ "SlopePerSecond", "Slope /s", [](VariableMonitor& v){ return parseUnitPrefix(v.evalCallGrowthRatePerSecond(),5); } });
    this->defineNewQuantity({ "SlopePerCall", "Slope /call", [](VariableMonitor& v){ return parseUnitPrefix(v.evalCallGrowthRate(), 5); } });
  }
  inline void VariablesMonitor::addVariable(const std::string& name_){
    for( const auto& v : _varMonitorList_ ){
      if( v.getName() == name_ ){
        throw std::logic_error("Variable name already added to the monitor");
      }
    }
    _varMonitorList_.emplace_back(name_);
  }
  inline void VariablesMonitor::addDisplayedQuantity(const std::string& quantityName_){
    int index = -1;
    for( size_t iQuantity = 0 ; iQuantity < _quantityMonitorList_.size() ; iQuantity++ ){
      if( _quantityMonitorList_.at(iQuantity).name == quantityName_ ){
        index = int(iQuantity);
        break;
      }
    }

    if( index == -1 ){
      throw std::logic_error("quantityName_ = " + quantityName_ + " not found.");
    }
    else{
      _displayQuantityIndexList_.emplace_back(index);
    }
  }
  inline void VariablesMonitor::defineNewQuantity(const QuantityMonitor& quantity_){
    int index = -1;
    for( size_t iQuantity = 0 ; iQuantity < _quantityMonitorList_.size() ; iQuantity++ ){
      if( _quantityMonitorList_.at(iQuantity).name == quantity_.name ){
        index = int(iQuantity);
        break;
      }
    }
    if( index == -1 ){
      _quantityMonitorList_.emplace_back(quantity_);
    }
    else{
      throw std::logic_error("quantity_: " + quantity_.name + " already exists.");
    }
  }
  inline long long int VariablesMonitor::getMaxRefreshRateInMs() const {
    return _maxRefreshRateInMs_;
  }
  inline VariableMonitor& VariablesMonitor::getVariable(const std::string& name_){
    for( auto& v : _varMonitorList_ ){
      if( v.getName() == name_ ){
        return v;
      }
    }
    throw std::logic_error("Variable with name " + name_ + " is not monitored");
  }
  inline QuantityMonitor& VariablesMonitor::getQuantity(const std::string& quantityName_){
    for( auto& q : _quantityMonitorList_ ){
      if( q.name == quantityName_ ){
        return q;
      }
    }
    throw std::logic_error("Quantity with name " + quantityName_ + " is not monitored");
  }
  inline bool VariablesMonitor::isGenerateMonitorStringOk() const {
    if( _maxRefreshRateInMs_ != -1 ){
      if( _lastGeneratedMonitorStringTime_.time_since_epoch().count() != 0
          and std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::high_resolution_clock::now() - _lastGeneratedMonitorStringTime_
      ).count() < _maxRefreshRateInMs_
          ){
        return false;
      }
    }
    return true;
  }
  inline std::string VariablesMonitor::generateMonitorString(bool trailBackCursor_, bool forceGenerate_) {

    if( not forceGenerate_ and trailBackCursor_ and not this->isGenerateMonitorStringOk() ) return {};
    _lastGeneratedMonitorStringTime_ = std::chrono::high_resolution_clock::now();

    std::vector<std::vector<std::string>> varElementsList(_varMonitorList_.size()+1, std::vector<std::string>(_displayQuantityIndexList_.size()));

    int iQuantity = 0;
    for( const auto& quantityIndex : _displayQuantityIndexList_ ){
      varElementsList[0][iQuantity++] = _quantityMonitorList_.at(quantityIndex).title;
    }

    for( size_t iVar = 0 ; iVar < _varMonitorList_.size() ; iVar++ ){ // line
      iQuantity = 0;
      for( const auto& quantityIndex : _displayQuantityIndexList_ ){ // col
        if( _quantityMonitorList_.at(quantityIndex).evalFunction ){
          varElementsList[iVar+1][iQuantity++] = _quantityMonitorList_.at(quantityIndex).evalFunction(_varMonitorList_.at(iVar));
        }
      }
    }
    _tablePrinter_.fillTable(varElementsList);

    std::stringstream ss;

    // Optional Header
    if( not _headerString_.empty() ){
      ss << _headerString_ << std::endl;
    }

    ss << _tablePrinter_.generateTableString() << std::endl;

    // Optional Footer
    if( not _footerString_.empty() ) ss << _footerString_ << std::endl;


    std::stringstream ssLineCleaner;
#ifdef CPP_GENERIC_TOOLBOX_BATCH
    #warning Variables Monitor in batch mode
    return ss.str();
#endif

    auto nLines = GenericToolbox::splitString(ss.str(), "\n").size();
    for( size_t iLine = 1 ; iLine < nLines ; iLine++ ){
      ssLineCleaner << static_cast<char>(27) << "[2K" << std::endl;
    }
    ssLineCleaner << static_cast<char>(27) << "[2K" << static_cast<char>(27) << "[" << nLines-1 << "F";
    ssLineCleaner << "\r" << ss.str(); // "\r" can be intercepted by loggers to know if a new line header can be printed

    if( trailBackCursor_ ){
      ssLineCleaner << static_cast<char>(27) << "[2K" << static_cast<char>(27) << "[" << nLines << "F" << std::endl;
      ssLineCleaner << static_cast<char>(27) << "[2K"; // un-flushed part: this clear line will only be displayed once a new line will try to override it
    }

    ssLineCleaner << std::flush;
    return ssLineCleaner.str();
  }
}

// AnyType
namespace GenericToolbox{  // Structs to decide if a stream function can be implemented
  template<typename S, typename T, typename = void> struct is_streamable : std::false_type {};
  template<typename S, typename T> struct is_streamable<S, T, decltype(std::declval<S&>() << std::declval<T&>(), void())> : std::true_type {};

  // Required by C++11 and C++14
  template <typename T, bool> class StreamerImpl {};
  template <typename T> class StreamerImpl<T,true> { public: static void implement(std::ostream& os, T const& t) { os << t; } };
  template <typename T> class StreamerImpl<T,false>{ public: static void implement(std::ostream& os, T const& t) { os << typeid(t).name(); } };
  template <typename T, bool> class DoubleCastImpl {};
  template <typename T> class DoubleCastImpl<T,true> { public: static double implement(T const& t) { return static_cast<double>(t); } };
  template <typename T> class DoubleCastImpl<T,false>{ public: static double implement(T const& t) { return std::nan(typeid(t).name()); } };

  // PlaceHolder is used in AnyType as a pointer member
  struct PlaceHolder{
    virtual ~PlaceHolder() = default;
    [[nodiscard]] virtual const std::type_info& getType() const = 0;
    [[nodiscard]] virtual PlaceHolder* clone() const = 0;
    virtual void writeToStream(std::ostream& o) const = 0;
    [[nodiscard]] virtual double getVariableAsDouble() const = 0;
    [[nodiscard]] virtual size_t getVariableSize() const = 0;
    [[nodiscard]] virtual const void* getVariableAddress() const = 0;
    virtual void* getVariableAddress() = 0;
  };

  // VariableHolder is the specialized PlaceHolder containing the _variable_ of interest
  template<typename VariableType> struct VariableHolder: public PlaceHolder{
    explicit VariableHolder(VariableType value_) : _nBytes_(sizeof(value_)), _variable_(std::move(value_)){  }
    [[nodiscard]] const std::type_info & getType() const override { return typeid(VariableType); }
    [[nodiscard]] PlaceHolder* clone() const override { return new VariableHolder(_variable_); }
    void writeToStream(std::ostream& o) const override {
#if HAS_CPP_17
      if constexpr (is_streamable<std::ostream,VariableType>::value) { o << _variable_; }
#else
      StreamerImpl<VariableType, is_streamable<std::ostream,VariableType>::value>::implement(o, _variable_);
#endif
    }
    [[nodiscard]] double getVariableAsDouble() const override {
      return DoubleCastImpl<VariableType, std::is_convertible<VariableType, double>::value>::implement(_variable_);
    }
    [[nodiscard]] size_t getVariableSize() const override{
      return _nBytes_;
    }
    [[nodiscard]] const void* getVariableAddress() const override{
      return static_cast<const void*>(&_variable_);
    }
    void* getVariableAddress() override{
      return static_cast<void*>(&_variable_);
    }

    size_t _nBytes_;
    VariableType _variable_;
  };


  // The Class:
  class AnyType{

  public:

    inline AnyType() = default;
    inline AnyType(const AnyType& other_);
    template<typename ValueType> inline explicit AnyType(const ValueType& value_);
    inline virtual ~AnyType() = default;

    // Operators
    template<typename ValueType> inline AnyType& operator=(const ValueType & rhs);
    inline AnyType& operator=(const AnyType& rhs);

    inline bool empty();
    inline PlaceHolder* getPlaceHolderPtr();
    [[nodiscard]] inline size_t getStoredSize() const;
    [[nodiscard]] inline const std::type_info& getType() const;
    [[nodiscard]] inline const PlaceHolder* getPlaceHolderPtr() const;

    template<typename ValueType> inline void setValue(const ValueType& value_);
    template<typename ValueType> inline ValueType& getValue();
    template<typename ValueType> inline const ValueType& getValue() const;
    [[nodiscard]] inline double getValueAsDouble() const;

    inline friend std::ostream& operator <<( std::ostream& o, const AnyType& v );


  protected:
    inline AnyType& swap(AnyType& rhs);


  private:
    std::unique_ptr<PlaceHolder> _varPtr_{};

  };

  inline AnyType::AnyType(const AnyType& other_){
    this->_varPtr_ = std::unique_ptr<PlaceHolder>(other_._varPtr_->clone());
  }
  template<typename ValueType> inline AnyType::AnyType(const ValueType& value_){
    this->template setValue(value_);
  }

  template<typename ValueType> inline AnyType& AnyType::operator=(const ValueType & rhs) {
    AnyType(rhs).swap(*this);
    return *this;
  }
  inline AnyType& AnyType::operator=(const AnyType& rhs){
    AnyType(rhs).swap(*this);
    return *this;
  }

  inline bool AnyType::empty(){
    return (_varPtr_ == nullptr);
  }
  inline const std::type_info& AnyType::getType() const{
    return _varPtr_ != nullptr ? _varPtr_->getType() : typeid(void);
  }
  inline const PlaceHolder* AnyType::getPlaceHolderPtr() const {
    return _varPtr_.get();
  }
  inline PlaceHolder* AnyType::getPlaceHolderPtr() {
    return _varPtr_.get();
  }
  inline size_t AnyType::getStoredSize() const{
    return _varPtr_->getVariableSize();
  }

  template<typename ValueType> inline void AnyType::setValue(const ValueType& value_){
    _varPtr_ = std::unique_ptr<VariableHolder<ValueType>>(new VariableHolder<ValueType>(value_));
  }
  template<typename ValueType> inline ValueType& AnyType::getValue() {
    if ( _varPtr_ == nullptr ){ throw std::runtime_error("AnyType value not set."); }
    if ( getType() != typeid(ValueType) ) { throw std::runtime_error("AnyType value type mismatch."); }
    return static_cast<VariableHolder<ValueType> *>(_varPtr_.get())->_variable_;
  }
  template<typename ValueType> inline const ValueType& AnyType::getValue() const{
    if ( _varPtr_ == nullptr ){ throw std::runtime_error("AnyType value not set."); }
    if ( getType() != typeid(ValueType) ) { throw std::runtime_error("AnyType value type mismatch."); }
    return static_cast<const VariableHolder<const ValueType> *>(_varPtr_.get())->_variable_;
  }
  inline double AnyType::getValueAsDouble() const{
    return _varPtr_->getVariableAsDouble();
  }

  inline std::ostream& operator<<( std::ostream& o, const AnyType& v ) {
    if( v._varPtr_ != nullptr ) v._varPtr_->writeToStream(o);
    return o;
  }

  // Protected
  inline AnyType& AnyType::swap(AnyType& rhs) {
    std::swap(_varPtr_, rhs._varPtr_);
    return *this;
  }
}


#endif // CPP_GENERIC_TOOLBOX_UTILS_H
