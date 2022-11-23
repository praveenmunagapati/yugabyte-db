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

#include "yb/common/ql_bfunc.h"

#include "yb/bfpg/bfpg.h"

#include "yb/bfql/bfql.h"

#include "yb/common/ql_value.h"

namespace yb {

using std::shared_ptr;
using std::vector;

//--------------------------------------------------------------------------------------------------
// CQL support

Status ExecBfunc(
    bfql::BFOpcode opcode, std::vector<QLValuePB>* params, QLValuePB* result) {
  return bfql::BFExecApi<QLValuePB, QLValuePB>::ExecQLOpcode(opcode, params, result);
}

//--------------------------------------------------------------------------------------------------
// PGSQL support

Status ExecBfunc(
    bfpg::BFOpcode opcode, std::vector<QLValuePB>* params, QLValuePB *result) {
  return bfpg::BFExecApi<QLValuePB, QLValuePB>::ExecPgsqlOpcode(opcode, params, result);
}

Status ExecBfunc(
    bfpg::BFOpcode opcode, std::vector<LWQLValuePB*>* params, LWQLValuePB *result) {
  return bfpg::BFExecApi<LWQLValuePB, LWQLValuePB>::ExecPgsqlOpcode(opcode, *params, result);
}

} // namespace yb
