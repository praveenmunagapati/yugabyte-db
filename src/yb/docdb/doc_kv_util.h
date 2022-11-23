// Copyright (c) YugaByte, Inc.
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

// Utilities for encoding and decoding key/value pairs that are used in the DocDB code.

#pragma once

#include <string>

#include "yb/common/doc_hybrid_time.h"
#include "yb/common/hybrid_time.h"

#include "yb/gutil/endian.h"

#include "yb/util/status_fwd.h"
#include "yb/util/kv_util.h"
#include "yb/util/monotime.h"
#include "yb/util/slice.h"

namespace yb {
namespace docdb {

constexpr int kEncodedKeyStrTerminatorSize = 2;

// Checks whether the given RocksDB key belongs to a document identified by the given encoded
// document key (a key that has already had zero characters escaped). This is done simply by
// checking that the key starts with the encoded document key followed by two zero characters.
// This is only used in unit tests as of 08/02/2016.
bool KeyBelongsToDocKeyInTest(const Slice &key, const std::string &encoded_doc_key);

// Given a DocDB key stored in RocksDB, validate the DocHybridTime size stored as the
// last few bits of the final byte of the key, and ensure that the ValueType byte preceding that
// encoded DocHybridTime is ValueType::kHybridTime.
Result<size_t> CheckHybridTimeSizeAndValueType(const Slice& key);

template <class Buffer>
void AppendUInt16ToKey(uint16_t val, Buffer* dest) {
  char buf[sizeof(uint16_t)];
  BigEndian::Store16(buf, val);
  dest->append(buf, sizeof(buf));
}

template <class Buffer>
void AppendUInt32ToKey(uint32_t val, Buffer* dest) {
  char buf[sizeof(uint32_t)];
  BigEndian::Store32(buf, val);
  dest->append(buf, sizeof(buf));
}

template <class Buffer>
void AppendUInt64ToKey(uint64_t val, Buffer* dest) {
  char buf[sizeof(uint64_t)];
  BigEndian::Store64(buf, val);
  dest->append(buf, sizeof(buf));
}

// Encodes the given string by replacing '\x00' with "\x00\x01" and appends it to the given
// destination string.
void AppendZeroEncodedStrToKey(const std::string &s, KeyBuffer *dest);

// Encodes the given string by replacing '\xff' with "\xff\xfe" and appends it to the given
// destination string.
void AppendComplementZeroEncodedStrToKey(const std::string &s, KeyBuffer *dest);

// Appends two zero characters to the given string. We don't add final end-of-string characters in
// this function.
void TerminateZeroEncodedKeyStr(KeyBuffer *dest);

// Appends two '\0xff' characters to the given string. We don't add final end-of-string characters
// in this function.
void TerminateComplementZeroEncodedKeyStr(KeyBuffer *dest);

inline void ZeroEncodeAndAppendStrToKey(const std::string &s, KeyBuffer *dest) {
  AppendZeroEncodedStrToKey(s, dest);
  TerminateZeroEncodedKeyStr(dest);
}

inline void ComplementZeroEncodeAndAppendStrToKey(const std::string &s, KeyBuffer* dest) {
  AppendComplementZeroEncodedStrToKey(s, dest);
  TerminateComplementZeroEncodedKeyStr(dest);
}

inline std::string ZeroEncodeStr(const std::string& s) {
  KeyBuffer result;
  ZeroEncodeAndAppendStrToKey(s, &result);
  return result.ToStringBuffer();
}

// Reverses the encoding we use for string fields in a RocksDB key where a zero is represented as
// \0x00\0x01 and the string is terminated with \x00\x00.
// Input/output:
//   slice - a slice containing an encoded string, optionally terminated by \x00\x00. A prefix of
//           this slice is consumed.
// Output (undefined in case of an error):
//   result - the resulting decoded string
yb::Status DecodeZeroEncodedStr(rocksdb::Slice* slice, std::string* result);

// A version of the above function that ensures the encoding is correct and all characters are
// consumed.
std::string DecodeZeroEncodedStr(std::string encoded_str);

// Reverses the encoding for a string that was encoded with ComplementZeroEncodeAndAppendStrToKey.
// In this representation the string termination changes from \x00\x00 to
// \xFF\xFF.
// Input/output:
//   slice - a slice containing an encoded string, optionally terminated by \xFF\xFF. A prefix of
//           this slice is consumed.
// Output (undefined in case of an error):
//   result - the resulting decoded string
yb::Status DecodeComplementZeroEncodedStr(rocksdb::Slice* slice, std::string* result);

// We try to use up to this number of characters when converting raw bytes to strings for debug
// purposes.
constexpr int kShortDebugStringLength = 40;

// Produces a debug-friendly representation of a sequence of bytes that may contain non-printable
// characters.
// @return A human-readable representation of the given slice, capped at a fixed short length.
std::string ToShortDebugStr(rocksdb::Slice slice);

inline std::string ToShortDebugStr(const std::string& raw_str) {
  return ToShortDebugStr(rocksdb::Slice(raw_str));
}

Result<DocHybridTime> DecodeInvertedDocHt(Slice key_slice);

constexpr size_t kMaxWordsPerEncodedHybridTimeWithValueType =
    ((kMaxBytesPerEncodedHybridTime + 1) + sizeof(size_t) - 1) / sizeof(size_t);

// Puts inverted encoded doc hybrid time specified by input to buffer.
// And returns slice to it.
using DocHybridTimeWordBuffer = std::array<size_t, kMaxWordsPerEncodedHybridTimeWithValueType>;
Slice InvertEncodedDocHT(const Slice& input, DocHybridTimeWordBuffer* buffer);

}  // namespace docdb
}  // namespace yb
