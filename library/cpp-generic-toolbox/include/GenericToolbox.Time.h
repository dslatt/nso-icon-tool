//
// Created by Nadrino on 23/12/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_TIME_H
#define CPP_GENERIC_TOOLBOX_TIME_H

// ***************************
//! Time related tools
// ***************************

#include <sstream>
#include <iomanip>
#include <utility>
#include <string>
#include <chrono>
#include <vector>
#include <map>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"


// Declaration section
namespace GenericToolbox{

  static std::string parseTimeUnit(double nbMicroSec_, int maxPadSize_=-1);
  static std::string getElapsedTimeSinceLastCallStr(const std::string& key_);
  static std::string getElapsedTimeSinceLastCallStr(int instance_ = -1);
  static long long getElapsedTimeSinceLastCallInMicroSeconds(const std::string& key_);
  static long long getElapsedTimeSinceLastCallInMicroSeconds(int instance = -1);
  static std::string getNowDateString(const std::string& dateFormat_="%Y_%m_%d-%H_%M_%S");

  namespace Time{

    class StopWatch {

    public:
      StopWatch() = default;

      void reset(){ _startTime_ = std::chrono::high_resolution_clock::now(); }
      [[nodiscard]] std::chrono::duration<double> get() const { return std::chrono::high_resolution_clock::now()-_startTime_; }

      [[nodiscard]] inline std::string toString() const {
        // .count() returns a double expressing the time in seconds
        return GenericToolbox::parseTimeUnit( this->get().count()*1E6 );
      }
      inline friend std::ostream& operator<< (std::ostream& stream, const StopWatch& stopWatch_) {
        stream << stopWatch_.toString(); return stream;
      }

    private:
      std::chrono::high_resolution_clock::time_point _startTime_{std::chrono::high_resolution_clock::now()};

    };

    class TimerBase{

    public:
      inline virtual void stop() = 0;
      inline virtual void stop( size_t nbCycles_ ) = 0; // handles what to do with the StopWatch clock
      [[nodiscard]] inline virtual std::chrono::duration<double> eval() const = 0; // eval returns a number of seconds

      inline void start(){ _stopWatch_.reset(); _isStarted_ = true; }
      inline void cycle( size_t nbCycles_=1 ){ this->stop(nbCycles_); this->start(); }
      inline void count( size_t nbCumulated_=1 ){
        if( nbCumulated_ > _nbCumulated_ ){ this->cycle(nbCumulated_ - _nbCumulated_);  }
        else{ _nbCumulated_ = nbCumulated_; }
      }

      // eval in a given units T = std::chrono::milliseconds for instance
      template<typename T> [[nodiscard]] inline ssize_t eval() const{ return std::chrono::duration_cast<T>( this->eval() ).count(); }
      [[nodiscard]] inline double evalTickSpeed() const{ return 1./this->eval().count(); } // per second
      template<typename T> [[nodiscard]] inline double evalTickSpeed() const{ return 1./this->evalTickSpeed<T>(); }

      [[nodiscard]] inline std::string toString() const { return GenericToolbox::parseTimeUnit( this->eval().count()*1E6 ); }
      inline friend std::ostream& operator<< (std::ostream& stream, const TimerBase& aTimer_) { stream << aTimer_.toString(); return stream; }

    protected:
      StopWatch _stopWatch_{};

      bool _isStarted_{false};
      size_t _nbCumulated_{0};

    };

    class Timer : public TimerBase {

    public:
      inline void stop( size_t nbCycles_ ) override {
        if( not _isStarted_ or nbCycles_ == 0 ){ return; }
        _nbCumulated_ += nbCycles_;
        _buffer_ = _stopWatch_.get()/nbCycles_;
        _isStarted_ = false;
      }
      inline void stop() override { this->stop(1); }
      [[nodiscard]] inline std::chrono::duration<double> eval() const override { return _buffer_; }

    private:
      std::chrono::duration<double> _buffer_{};
    };

    template <size_t N> class AveragedTimer : public TimerBase {

    public:
      inline void stop( size_t nbCycles_ ) override {
        if( not _isStarted_ or nbCycles_ == 0 ){ return; }
        _nbCumulated_ += nbCycles_;
        _durationsBuffer_[_cursorIndex_++] = _stopWatch_.get()/nbCycles_;
        if( _cursorIndex_ >= _durationsBuffer_.size() ){ _cursorIndex_ = 0; _isBufferFilled_ = true; }
        _isStarted_ = false;
      }
      inline void stop() override { this->stop(1); }
      [[nodiscard]] inline std::chrono::duration<double> eval() const override {
        std::chrono::duration<double> out{0};
        if( not _isBufferFilled_ ){
          for( int iSlot = 0 ; iSlot < _cursorIndex_+1 ; iSlot++ ){ out += _durationsBuffer_[iSlot]; }
          return out/(_cursorIndex_+1);
        }

        for( auto& duration : _durationsBuffer_ ){ out += duration; }

        // in seconds
        return out/_durationsBuffer_.size();
      }

    private:
      bool _isBufferFilled_{false};
      int _cursorIndex_{0};
      std::array<std::chrono::duration<double>, N> _durationsBuffer_{};
    };
  }

}


// Implementation section
namespace GenericToolbox{

  namespace Time{
    namespace Internals{
      static std::map<int, std::chrono::high_resolution_clock::time_point> lastTimePointMap{};
      static std::map<std::string, std::chrono::high_resolution_clock::time_point> lastTimePointMapStr{};
    }
  }



  static std::string parseTimeUnit(double nbMicroSec_, int maxPadSize_){

    std::stringstream ss;

    if( nbMicroSec_ < 0 ){
      ss << "-";
      nbMicroSec_ = -nbMicroSec_;
      maxPadSize_--;
    }

    if(maxPadSize_ > -1){
      ss << std::setprecision(maxPadSize_-1);
    }

    auto reducedVal = size_t(std::abs(nbMicroSec_));
    if     ( (reducedVal = (reducedVal / 1000)) < 9 ){ // print in ms?
      ss << nbMicroSec_ << "us";                       // <- no
    }
    else if( (reducedVal = (reducedVal / 1000)) < 3 ){ // print in s?
      ss << nbMicroSec_/1E3 << "ms";
    }
    else if( (reducedVal = (reducedVal / 60)) < 2 ){ // print in min?
      ss << nbMicroSec_/1E6 << "s";
    }
    else if( (reducedVal = (reducedVal / 60)) < 2 ){ // print in h?
      ss << nbMicroSec_/1E6/60. << "min";
    }
    else if( (reducedVal = (reducedVal / 24)) < 2 ){ // print in d?
      ss << nbMicroSec_/1E6/3600. << "h";
    }
    else if( (reducedVal = (reducedVal / 24)) < 2 ){ // print in y?
      ss << nbMicroSec_/1E6/3600./24. << "d";
    }
    else {
      ss << nbMicroSec_/1E6/3600./24./365.25 << "y";
    }
    return ss.str();
  }
  static std::string getElapsedTimeSinceLastCallStr( const std::string& key_ ) {
    return GenericToolbox::parseTimeUnit(double(GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(key_)));
  }
  static std::string getElapsedTimeSinceLastCallStr(int instance_){
    return GenericToolbox::parseTimeUnit(double(getElapsedTimeSinceLastCallInMicroSeconds(instance_)));
  }
  static long long getElapsedTimeSinceLastCallInMicroSeconds( const std::string& key_ ) {
    auto newTimePoint = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
        newTimePoint - Time::Internals::lastTimePointMapStr[key_]
    );
    Time::Internals::lastTimePointMapStr[key_] = newTimePoint;
    return microseconds.count();
  }
  static long long getElapsedTimeSinceLastCallInMicroSeconds(int instance_){
    auto newTimePoint = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
        newTimePoint - Time::Internals::lastTimePointMap[instance_]
    );
    Time::Internals::lastTimePointMap[instance_] = newTimePoint;
    return microseconds.count();
  }
  static std::string getNowDateString(const std::string& dateFormat_){
    std::stringstream ss;
#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ <= 4)
    std::time_t now = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&now);

    char buffer[128];
    std::strftime(buffer, sizeof(buffer), dateFormat_.c_str(), timeinfo);

    ss << buffer;
#else
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    ss << std::put_time(std::localtime(&in_time_t), dateFormat_.c_str());
#endif
    return ss.str();
  }



}


#endif // CPP_GENERIC_TOOLBOX_TIME_H
