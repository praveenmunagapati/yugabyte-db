//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under the BSD-style license found in the
//  LICENSE file in the root directory of this source tree. An additional grant
//  of patent rights can be found in the PATENTS file in the same directory.
//
// The following only applies to changes made to this file as part of YugaByte development.
//
// Portions Copyright (c) YugaByte, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied.  See the License for the specific language governing permissions and limitations
// under the License.
//
#pragma once


namespace rocksdb {

// XFUNC is never defined. If we enable it, the code does not compile. We should get rid of it.

#ifndef XFUNC
#define XFUNC_TEST(condition, location, lfname, fname, ...)
#else

void GetXFTestOptions(Options* options, int skip_policy);
void xf_manage_options(ReadOptions* read_options);
void xf_transaction_set_memtable_history(
    int32_t* max_write_buffer_number_to_maintain);
void xf_transaction_clear_memtable_history(
    int32_t* max_write_buffer_number_to_maintain);

// This class provides the facility to run custom code to test a specific
// feature typically with all existing unit tests.
// A developer could specify cross functional test points in the codebase
// via XFUNC_TEST.
// Each xfunc test represents a position in the execution stream of a thread.
// Whenever that particular piece of code is called, the given cross-functional
// test point is executed.
// eg. on DBOpen, a particular option can be set.
// on Get, a particular option can be set, or a specific check can be invoked.
// XFUNC_TEST(TestName, location, lfname, FunctionName, Args)
// Turn on a specific cross functional test by setting the environment variable
// ROCKSDB_XFUNC_TEST

class XFuncPoint {
 public:
  // call once at the beginning of a test to get the test name
  static void Init() {
    char* s = getenv("ROCKSDB_XFUNC_TEST");
    if (s == nullptr) {
      xfunc_test_ = "";
      enabled_ = false;
    } else {
      xfunc_test_ = s;
      enabled_ = true;
    }
    initialized_ = true;
  }

  static bool Initialized() { return initialized_; }

  static bool Check(std::string test) {
    return (enabled_ &&
            ((test.compare("") == 0) || (test.compare(xfunc_test_) == 0)));
  }

  static void SetSkip(int skip) { skip_policy_ = skip; }
  static int GetSkip(void) { return skip_policy_; }

 private:
  static std::string xfunc_test_;
  static bool initialized_;
  static bool enabled_;
  static int skip_policy_;
};

// Use XFUNC_TEST to specify cross functional test points inside the code base.
// By setting ROCKSDB_XFUNC_TEST, all XFUNC_TEST having that
// value in the condition field will be executed.
// The second argument specifies a string representing the calling location
// The third argument, lfname, is the name of the function which will be created
// and called.
// The fourth argument fname represents the function to be called
// The arguments following that are the arguments to fname
// See Options::Options in options.h for an example use case.
// XFUNC_TEST is no op in release build.
#define XFUNC_TEST(condition, location, lfname, fname, ...)         \
  {                                                                 \
    if (!XFuncPoint::Initialized()) {                               \
      XFuncPoint::Init();                                           \
    }                                                               \
    if (XFuncPoint::Check(condition)) {                             \
      std::function<void()> lfname = std::bind(fname, __VA_ARGS__); \
      lfname();                                                     \
    }                                                               \
  }

#endif  // XFUNC

enum SkipPolicy { kSkipNone = 0, kSkipNoSnapshot = 1, kSkipNoPrefix = 2 };
}  // namespace rocksdb
