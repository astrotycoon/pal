/*
Copyright (c) 2011 John McCutchan <john@johnmccutchan.com>

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#ifndef LIBPAL_PAL_EVENT_H_
#define LIBPAL_PAL_EVENT_H_

#include "libpal/pal_array.h"
#include "libpal/pal_delegate.h"

template<typename Signature>
class palEvent;

template<typename R>
class palEvent<R ( ) > {
public:
  typedef palDelegate<R ()> DelegateType;
protected:
  palArray<DelegateType> delegates_;
public:

  void Register(DelegateType del) {
    delegates_.push_back(del);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)()) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)() const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)()) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    delegates_.Erase(del);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)()) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)() const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)()) {
    Unregister(DelegateType(func));
  }

  void Fire() const {
    for (int i = 0; i < delegates_.GetSize(); i++) {
      const DelegateType& del = delegates_[i];
      del();
    }
  }
};

template<typename R, typename Param1>
class palEvent<R ( Param1 ) > {
public:
  typedef palDelegate<R (Param1)> DelegateType;
protected:
  palArray<DelegateType> delegates_;
public:

  void Register(DelegateType del) {
    delegates_.push_back(del);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1)) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1) const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)(Param1 p1)) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    delegates_.Erase(del);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1)) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1) const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)(Param1 p1)) {
    Unregister(DelegateType(func));
  }

  void Fire(Param1 p1) const {
    for (int i = 0; i < delegates_.GetSize(); i++) {
      const DelegateType& del = delegates_[i];
      del(p1);
    }
  }
};

template<typename R, typename Param1, typename Param2>
class palEvent<R ( Param1, Param2 ) > {
public:
  typedef palDelegate<R (Param1, Param2)> DelegateType;
protected:
  palArray<DelegateType> delegates_;
public:

  void Register(DelegateType del) {
    delegates_.push_back(del);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2)) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2) const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)(Param1 p1, Param2 p2)) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    delegates_.Erase(del);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2)) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2) const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)(Param1 p1, Param2 p2)) {
    Unregister(DelegateType(func));
  }

  void Fire(Param1 p1, Param2 p2) const {
    for (int i = 0; i < delegates_.GetSize(); i++) {
      const DelegateType& del = delegates_[i];
      del(p1, p2);
    }
  }
};

template<typename R, typename Param1, typename Param2, typename Param3>
class palEvent<R ( Param1, Param2, Param3 ) > {
public:
  typedef palDelegate<R (Param1, Param2, Param3)> DelegateType;
protected:
  palArray<DelegateType> delegates_;
public:

  void Register(DelegateType del) {
    delegates_.push_back(del);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3)) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3) const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)(Param1 p1, Param2 p2, Param3 p3)) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    delegates_.Erase(del);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3)) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3) const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)(Param1 p1, Param2 p2, Param3 p3)) {
    Unregister(DelegateType(func));
  }

  void Fire(Param1 p1, Param2 p2, Param3 p3) const {
    for (int i = 0; i < delegates_.GetSize(); i++) {
      const DelegateType& del = delegates_[i];
      del(p1, p2, p3);
    }
  }
};

template<typename R, typename Param1, typename Param2, typename Param3, typename Param4>
class palEvent<R ( Param1, Param2, Param3, Param4 ) > {
public:
  typedef palDelegate<R (Param1, Param2, Param3, Param4)> DelegateType;
protected:
  palArray<DelegateType> delegates_;
public:

  void Register(DelegateType del) {
    delegates_.push_back(del);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4)) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4) const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4)) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    delegates_.Erase(del);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4)) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4) const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4)) {
    Unregister(DelegateType(func));
  }

  void Fire(Param1 p1, Param2 p2, Param3 p3, Param4 p4) const {
    for (int i = 0; i < delegates_.GetSize(); i++) {
      const DelegateType& del = delegates_[i];
      del(p1, p2, p3, p4);
    }
  }
};

template<typename R, typename Param1, typename Param2, typename Param3, typename Param4, typename Param5>
class palEvent<R ( Param1, Param2, Param3, Param4, Param5 ) > {
public:
  typedef palDelegate<R (Param1, Param2, Param3, Param4, Param5)> DelegateType;
protected:
  palArray<DelegateType> delegates_;
public:

  void Register(DelegateType del) {
    delegates_.push_back(del);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5)) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5)) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    delegates_.Erase(del);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5)) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5)) {
    Unregister(DelegateType(func));
  }

  void Fire(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const {
    for (int i = 0; i < delegates_.GetSize(); i++) {
      const DelegateType& del = delegates_[i];
      del(p1, p2, p3, p4, p5);
    }
  }
};

template<typename R, typename Param1, typename Param2, typename Param3, typename Param4, typename Param5, typename Param6>
class palEvent<R ( Param1, Param2, Param3, Param4, Param5, Param6 ) > {
public:
  typedef palDelegate<R (Param1, Param2, Param3, Param4, Param5, Param6)> DelegateType;
protected:
  palArray<DelegateType> delegates_;
public:

  void Register(DelegateType del) {
    delegates_.push_back(del);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6)) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6)) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    delegates_.Erase(del);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6)) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6)) {
    Unregister(DelegateType(func));
  }

  void Fire(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const {
    for (int i = 0; i < delegates_.GetSize(); i++) {
      const DelegateType& del = delegates_[i];
      del(p1, p2, p3, p4, p5, p6);
    }
  }
};

template<typename R, typename Param1, typename Param2, typename Param3, typename Param4, typename Param5, typename Param6, typename Param7>
class palEvent<R ( Param1, Param2, Param3, Param4, Param5, Param6, Param7 ) > {
public:
  typedef palDelegate<R (Param1, Param2, Param3, Param4, Param5, Param6, Param7)> DelegateType;
protected:
  palArray<DelegateType> delegates_;
public:

  void Register(DelegateType del) {
    delegates_.push_back(del);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7)) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7) const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7)) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    delegates_.Erase(del);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7)) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7) const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7)) {
    Unregister(DelegateType(func));
  }

  void Fire(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7) const {
    for (int i = 0; i < delegates_.GetSize(); i++) {
      const DelegateType& del = delegates_[i];
      del(p1, p2, p3, p4, p5, p6, p7);
    }
  }
};

template<typename R, typename Param1, typename Param2, typename Param3, typename Param4, typename Param5, typename Param6, typename Param7, typename Param8>
class palEvent<R ( Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8 ) > {
public:
  typedef palDelegate<R (Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)> DelegateType;
protected:
  palArray<DelegateType> delegates_;
public:

  void Register(DelegateType del) {
    delegates_.push_back(del);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8)) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8) const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8)) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    delegates_.Erase(del);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8)) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8) const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8)) {
    Unregister(DelegateType(func));
  }

  void Fire(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8) const {
    for (int i = 0; i < delegates_.GetSize(); i++) {
      const DelegateType& del = delegates_[i];
      del(p1, p2, p3, p4, p5, p6, p7, p8);
    }
  }
};

template<typename R, typename Param1, typename Param2, typename Param3, typename Param4, typename Param5, typename Param6, typename Param7, typename Param8, typename Param9>
class palEvent<R ( Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8, Param9 ) > {
public:
  typedef palDelegate<R (Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8, Param9)> DelegateType;
protected:
  palArray<DelegateType> delegates_;
public:

  void Register(DelegateType del) {
    delegates_.push_back(del);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8, Param9 p9)) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8, Param9 p9) const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8, Param9 p9)) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    delegates_.Erase(del);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8, Param9 p9)) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8, Param9 p9) const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8, Param9 p9)) {
    Unregister(DelegateType(func));
  }

  void Fire(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8, Param9 p9) const {
    for (int i = 0; i < delegates_.GetSize(); i++) {
      const DelegateType& del = delegates_[i];
      del(p1, p2, p3, p4, p5, p6, p7, p8, p9);
    }
  }
};

#endif  // LIBPAL_PAL_EVENT_H_
