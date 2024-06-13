//
// Created by Nadrino on 03/12/2021.
//

#ifndef CPP_GENERIC_TOOLBOX_WRAPPERS_H
#define CPP_GENERIC_TOOLBOX_WRAPPERS_H



#include <condition_variable>
#include <type_traits>
#include <atomic>
#include <memory>
#include <mutex>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"


// Declaration
namespace GenericToolbox{

  template<typename T> class NoCopyWrapper: public T {
  public:
    NoCopyWrapper() = default;
    NoCopyWrapper(const NoCopyWrapper&){}
    NoCopyWrapper& operator=(const NoCopyWrapper&){ return *this; }
  };

  template<typename T> class CopiableAtomic : public std::atomic<T> {
  public:
    CopiableAtomic() = default;
    constexpr explicit CopiableAtomic(T desired) : std::atomic<T>(desired) {}
    constexpr CopiableAtomic(const CopiableAtomic<T>& other) : CopiableAtomic(other.load(std::memory_order_relaxed)){}
    CopiableAtomic& operator=(const CopiableAtomic<T>& other);
    CopiableAtomic& operator=(const T& other);
  };

  template<typename T> class PolymorphicObjectWrapper{

  public:
    PolymorphicObjectWrapper() = default;

    // Handling copy
    PolymorphicObjectWrapper(const PolymorphicObjectWrapper& src_): dialPtr{src_.dialPtr->clone()} {  }
    PolymorphicObjectWrapper& operator=(const PolymorphicObjectWrapper& other);
    PolymorphicObjectWrapper(PolymorphicObjectWrapper&&)  noexcept = default;
    PolymorphicObjectWrapper& operator=(PolymorphicObjectWrapper&&)  noexcept = default;

#if HAS_CPP_14
    template<typename DerivedT, std::enable_if_t<std::is_base_of<T, DerivedT>::value, int> = 0>
    explicit PolymorphicObjectWrapper(const DerivedT& src_): dialPtr{src_.clone()} {  }
#else
    template <typename DerivedT>
    explicit PolymorphicObjectWrapper(const DerivedT& src_): dialPtr{src_.clone()} {
      static_assert(std::is_base_of<T, DerivedT>::value, "Class doesn't inherit from Base!");
    }
#endif


#if HAS_CPP_14
    template<typename DerivedT, std::enable_if_t<std::is_base_of<T, DerivedT>::value, int> = 0>
    explicit PolymorphicObjectWrapper(std::unique_ptr<DerivedT> def): dialPtr{std::move(def)} {};
#else
    template <typename DerivedT>
    explicit PolymorphicObjectWrapper(std::unique_ptr<DerivedT> def): dialPtr{std::move(def)} {
      static_assert(std::is_base_of<T, DerivedT>::value, "Class doesn't inherit from Base!");
    }
#endif

    T& operator*() const { return *dialPtr; }
    T* operator->() const { return dialPtr.get(); }
    [[nodiscard]] T* get() const{ return dialPtr.get(); }

    std::unique_ptr<T> dialPtr;

  };

  template<typename T> class Atomic{

  public:
    Atomic() = default;
    explicit Atomic(const T& val_): _variable_(val_) {}

    // setter
    void setValue(const T& val_);

    // getter
    const T& getValue() const;

    // core
    void waitUntilEqual(const T& val_) const;

    // Define operator++ (post-increment) using SFINAE
    // Using operator++(int) and not operator++(T) to stay consistent with C++ conventions */
    template<typename U = T>
    typename std::enable_if<std::is_integral<U>::value, T>::type operator++(int);

  private:
    T _variable_{};
    mutable GenericToolbox::NoCopyWrapper<std::mutex> _mutex_{};
    mutable GenericToolbox::NoCopyWrapper<std::condition_variable> _notifier_{};
  };

}

// Implementation
namespace GenericToolbox{

  template<typename T> CopiableAtomic<T>& CopiableAtomic<T>::operator=(const CopiableAtomic<T>& other) {
    this->store(other.load(std::memory_order_acquire), std::memory_order_release);
    return *this;
  }
  template<typename T> CopiableAtomic<T>& CopiableAtomic<T>::operator=(const T& other) {
    this->store(other, std::memory_order_release);
    return *this;
  }

  template<typename T> PolymorphicObjectWrapper<T>& PolymorphicObjectWrapper<T>::operator=(const PolymorphicObjectWrapper& other){
    if (this != &other) { dialPtr = other.dialPtr->clone(); } return *this;
  }


  template<typename T> void Atomic<T>::setValue(const T& val_){
    // meant to be executed in the main thread
    {
      // scope lock
      std::unique_lock<std::mutex> lock(_mutex_);
      _variable_ = val_;
    }
    // tell the parallel thread to check the "engageThread" variable
    _notifier_.notify_one();
  }
  template<typename T> const T& Atomic<T>::getValue() const {
    // scope lock
    std::unique_lock<std::mutex> lock(_mutex_);
    return _variable_;
  }
  template<typename T> void Atomic<T>::waitUntilEqual(const T& val_) const {
    std::unique_lock<std::mutex> lock(_mutex_);
    _notifier_.wait(lock, [&]{ return _variable_ == val_; });
  }

  template<typename T> template<typename U>
  typename std::enable_if<std::is_integral<U>::value, T>::type Atomic<T>::operator++(int) {
    std::unique_lock<std::mutex> lock(_mutex_);
    T out(_variable_++);
    return out;
  }

}

#endif //CPP_GENERIC_TOOLBOX_WRAPPERS_H
