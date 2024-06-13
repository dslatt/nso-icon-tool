//
// Created by Nadrino on 25/06/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_THREAD_H
#define CPP_GENERIC_TOOLBOX_THREAD_H


#include "GenericToolbox.Wrappers.h"
#include "GenericToolbox.Vector.h"
#include "GenericToolbox.Time.h"
#include "GenericToolbox.Macro.h"

#include <condition_variable>
#include <functional>
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <future>
#include <mutex>
#include <queue>
#include <map>


// WorkerEntry
namespace GenericToolbox{
  struct WorkerEntry{
    int index{-1};
    std::shared_ptr<std::future<void>> thread{nullptr};

    // signal handler
    std::function<void(int)>* fctToRunPtr{nullptr};
    GenericToolbox::Atomic<bool> isEngaged{false};
    GenericToolbox::Atomic<bool> isRunning{false};
  };
}

// JobEntry
namespace GenericToolbox{
  struct JobEntry{
    std::string name{};
    std::function<void(int)> function{};
    std::function<void()> functionPreParallel{};
    std::function<void()> functionPostParallel{};

    JobEntry() = delete;
    explicit JobEntry(std::string  name_) : name(std::move(name_)) {}
  };
}

// ParallelWorker
namespace GenericToolbox{

  class ParallelWorker {

  public:
    template<typename T> struct ThreadBounds{ T beginIndex{}; T endIndex{}; };
    template<typename T> static ThreadBounds<T> getThreadBoundIndices(int iThread_, int nThreads_, T nTotal_);

  public:
    inline ParallelWorker() = default;
    inline virtual ~ParallelWorker(){ if( not _workerList_.empty() ){ this->stopThreads(); } };

    inline void setIsVerbose(bool isVerbose_){ _isVerbose_ = isVerbose_; }
    inline void setNThreads(int nThreads_);
    inline void setCpuTimeSaverIsEnabled(bool cpuTimeSaverIsEnabled_);

    // const getters
    [[nodiscard]] inline int getNbThreads() const{ return _nbThreads_; }
    [[nodiscard]] inline int getJobIdx(const std::string& name_) const;
    [[nodiscard]] inline const JobEntry* getJobPtr(const std::string& name_) const;
    [[nodiscard]] inline const GenericToolbox::Time::Timer& getLastJobTimer() const { return _lastJobTimer_; }

    // getters
    inline JobEntry* getJobPtr(const std::string& name_);

    // core
    inline void addJob(const std::string& jobName_, const std::function<void(int)>& function_); // int arg is supposed to be the thread id
    inline void setPostParallelJob(const std::string& jobName_, const std::function<void()>& function_);
    inline void setPreParallelJob(const std::string& jobName_, const std::function<void()>& function_);
    inline void runJob(const std::function<void(int)>& function_);
    inline void runJob(const std::string& jobName_);
    inline void removeJob(const std::string& jobName_);

  protected:
    inline void startThreads();
    inline void stopThreads();

    inline void threadWorker(int iThread_);

  private:
    // Parameters
    bool _isVerbose_{false};
    bool _cpuTimeSaverIsEnabled_{false};
    int _nbThreads_{1};

    // Internals
    bool _stopThreads_{false};

    std::vector<WorkerEntry> _workerList_{};
    std::vector<JobEntry> _jobEntryList_{};

    // Monitoring
    Time::Timer _lastJobTimer_{};

  };

  // statics
  template<typename T> inline ParallelWorker::ThreadBounds<T> ParallelWorker::getThreadBoundIndices(int iThread_, int nThreads_, T nTotal_){
    // first index, last index: for( int i = out.first ; i < out.second ; i++){}
    ParallelWorker::ThreadBounds<T> out{0, nTotal_};

    if( iThread_ == -1 or nThreads_ == 1 ){ return out; }

    int nEventsPerThread = nTotal_ / nThreads_;
    int nExtraEvents = nTotal_ % nThreads_;

    // make sure we get the right shift event if nTotal_ < nThreads_
    out.beginIndex =
        std::min(iThread_, nExtraEvents) * (nEventsPerThread + 1) +
        std::max(0, iThread_ - nExtraEvents) * nEventsPerThread;

    // adjust such the first threads are sharing the numExtraEvents
    out.endIndex =
        out.beginIndex +
        ((iThread_ < nExtraEvents) ? nEventsPerThread + 1 : nEventsPerThread );

    // OLD METHOD:
//    out.first = T(iThread_)*( nTotal_ / T(nThreads_) );
//    if( iThread_+1 != nThreads_ ){
//      out.second = (T(iThread_) + 1) * (nTotal_ / T(nThreads_));
//    }

    return out;
  }

  // setters
  inline void ParallelWorker::setNThreads(int nThreads_){
    if( not _workerList_.empty() ){ throw std::logic_error("Can't " + __METHOD_NAME__ + " while workers are running."); }
    _nbThreads_ = nThreads_;
  }
  inline void ParallelWorker::setCpuTimeSaverIsEnabled(bool cpuTimeSaverIsEnabled_){
    if( not _workerList_.empty() ){ throw std::logic_error("Can't " + __METHOD_NAME__ + " while workers are running."); }
    _cpuTimeSaverIsEnabled_ = cpuTimeSaverIsEnabled_;
  }

  // const getters
  inline int ParallelWorker::getJobIdx(const std::string& name_) const{
    return GenericToolbox::findElementIndex(name_, _jobEntryList_, [](const JobEntry& job){ return job.name; });
  }
  inline const JobEntry* ParallelWorker::getJobPtr(const std::string& name_) const {
    auto idx = this->getJobIdx( name_ );
    if( idx == -1 ){ return nullptr; }
    return &_jobEntryList_[idx];
  }

  // getters
  inline JobEntry* ParallelWorker::getJobPtr(const std::string& name_){
    return const_cast<JobEntry*>(const_cast<const ParallelWorker*>(this)->getJobPtr(name_));
  }

  inline void ParallelWorker::addJob(const std::string &jobName_, const std::function<void(int)> &function_) {
    // is it callable?
    if( not function_ ){ throw std::logic_error("the provided function is not callable"); }

    // existing job?
    auto* jobPtr{ this->getJobPtr(jobName_) };
    if( jobPtr != nullptr ){ throw std::logic_error("A job with the same name has already been added: " + jobName_); }

    // now emplace_back()
    _jobEntryList_.emplace_back( jobName_ );
    jobPtr = this->getJobPtr( jobName_ );
    jobPtr->function = function_;
  }
  inline void ParallelWorker::setPostParallelJob(const std::string& jobName_, const std::function<void()>& function_){
    // is it callable?
    if( not function_ ){ throw std::logic_error("the provided function is not callable"); }

    // existing job?
    auto* jobPtr{ this->getJobPtr(jobName_) };
    if( jobPtr == nullptr ){ throw std::logic_error(jobName_ + ": is not in the available jobsList"); }

    jobPtr->functionPostParallel = function_;
  }
  inline void ParallelWorker::setPreParallelJob(const std::string& jobName_, const std::function<void()>& function_){
    // is it callable?
    if( not function_ ){ throw std::logic_error("the provided function is not callable"); }

    // existing job?
    auto* jobPtr{ this->getJobPtr(jobName_) };
    if( jobPtr == nullptr ){ throw std::logic_error(jobName_ + ": is not in the available jobsList"); }

    jobPtr->functionPreParallel = function_;
  }
  inline void ParallelWorker::runJob(const std::function<void(int)>& function_){
    std::string tempName{std::to_string((unsigned long long)(void**)this)};
    this->addJob(tempName, function_);
    this->runJob(tempName);
    this->removeJob(tempName);
  }
  inline void ParallelWorker::runJob(const std::string &jobName_) {
    _lastJobTimer_.start();
    if( _isVerbose_ ){ std::cout << "Running \"" << jobName_ << "\" on " << _nbThreads_ << " parallel threads..." << std::endl; }

    // existing job?
    auto* jobPtr{ this->getJobPtr(jobName_) };
    if( jobPtr == nullptr ){ throw std::logic_error(jobName_ + ": is not in the available jobsList"); }

    // runs the pre-job if set
    if( jobPtr->functionPreParallel ){ jobPtr->functionPreParallel(); }

    // launch threads if not already
    if( _workerList_.empty() ){ this->startThreads(); }

    // request the jobs
    for( auto& thread : _workerList_ ){
      thread.fctToRunPtr = &jobPtr->function;
      thread.isEngaged.setValue(true );
    }

    // do the last job in the main thread
    jobPtr->function(_nbThreads_ - 1);

    for( auto& worker : _workerList_ ){
      if( _isVerbose_ ) std::cout << "Waiting for worker #" << worker.index << " to be finish the job..." << std::endl;
      worker.isEngaged.waitUntilEqual( false );
    }

    // CPU time saver stops the parallel threads when no job is requested
    if( _cpuTimeSaverIsEnabled_ ){ this->stopThreads(); }

    // runs the post-job if set
    if( jobPtr->functionPostParallel ){ jobPtr->functionPostParallel(); }

    _lastJobTimer_.stop();
  }
  inline void ParallelWorker::removeJob(const std::string& jobName_){
    // existing job?
    auto jobIdx{ this->getJobIdx(jobName_) };
    if( jobIdx == -1 ){ throw std::logic_error(jobName_ + ": is not in the available jobsList"); }

    _jobEntryList_.erase(_jobEntryList_.begin() + jobIdx);

    // stop the parallel threads if no job is in the pool
    if( _jobEntryList_.empty() ){ this->stopThreads(); }
  }

  inline void ParallelWorker::startThreads(){
    if( _isVerbose_ ){ std::cout << __METHOD_NAME__ << std::endl; }

    _stopThreads_ = false;

    _workerList_.clear();
    _workerList_.resize(_nbThreads_ - 1 );
    int iThread{-1};
    for( auto& worker : _workerList_ ){
      iThread++;
      worker.index = iThread;
      worker.thread = std::make_shared<std::future<void>>(
          std::async( std::launch::async, [this, iThread]{ this->threadWorker( iThread ); } )
      );
    }

    // waiting for all of them to start
    for( auto& worker : _workerList_ ){
      worker.isRunning.waitUntilEqual( true );
    }
  }
  inline void ParallelWorker::stopThreads(){
    if( _isVerbose_ ){ std::cout << __METHOD_NAME__ << std::endl; }

    // set stop signal
    _stopThreads_ = true;

    // stop waiting for the signal
    for( auto& worker : _workerList_ ){ worker.isEngaged.setValue( true ); }

    // wait for all to close
    for( auto& worker : _workerList_ ){ worker.thread.get(); }

    // an empty worker list is saying the threads are not running. So clearing it
    _workerList_.clear();
  }


  inline void ParallelWorker::threadWorker(int iThread_){

    auto* thisWorker{&_workerList_[iThread_]};
    thisWorker->isRunning.setValue( true );

    while( not _stopThreads_ ){

      // release with signal is set
      thisWorker->isEngaged.waitUntilEqual( true );

      if( _stopThreads_ ){ break; } // if stop requested while in pause

      // run job
      (*thisWorker->fctToRunPtr)(iThread_);

      // reset
      thisWorker->isEngaged.setValue(false );

    } // not stop

  }
}

// OrderedLock
namespace GenericToolbox{
  class OrderedLock {

  public:
    inline OrderedLock() = default;

    [[nodiscard]] inline bool isLocked() const { return _isLocked_; }

    inline void lock();
    inline void unlock();

  private:
    bool _isLocked_{false};
    NoCopyWrapper<std::mutex> _lock_{};
    std::queue<std::condition_variable *> _conditionVariable_{};
  };

  void OrderedLock::lock() {
    std::unique_lock<std::mutex> acquire(_lock_);
    if (_isLocked_) {
      std::condition_variable signal{};
      _conditionVariable_.emplace(&signal);
      signal.wait(acquire);
    }
    else {
      _isLocked_ = true;
    }
  }
  void OrderedLock::unlock() {
    std::unique_lock<std::mutex> acquire(_lock_);
    if (_conditionVariable_.empty()) {
      _isLocked_ = false;
    }
    else {
      _conditionVariable_.front()->notify_one();
      _conditionVariable_.pop();
    }
  }
}



#endif //CPP_GENERIC_TOOLBOX_THREAD_H
