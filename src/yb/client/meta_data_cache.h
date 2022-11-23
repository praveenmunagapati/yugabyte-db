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

#pragma once

#include <mutex>
#include <unordered_map>

#include <boost/container_hash/hash.hpp>

#include "yb/client/client_fwd.h"
#include "yb/client/yb_table_name.h"

#include "yb/common/common_fwd.h"
#include "yb/common/common_types.pb.h"

#include "yb/yql/cql/ql/ptree/pt_option.h"

namespace yb {
namespace client {

enum class CacheCheckMode {
  NO_RETRY,
  RETRY,
};

class YBMetaDataCache {
 public:
  YBMetaDataCache(client::YBClient* client,
                  bool create_roles_permissions_cache = false);
  ~YBMetaDataCache();

  // Opens the table with the given name or id. If the table has been opened before, returns the
  // previously opened table from cached_tables_. If the table has not been opened before
  // in this client, this will do an RPC to ensure that the table exists and look up its schema.
  Status GetTable(const YBTableName& table_name,
                  std::shared_ptr<YBTable>* table,
                  bool* cache_used);
  Status GetTable(const TableId& table_id,
                  std::shared_ptr<YBTable>* table,
                  bool* cache_used);

  // Remove the table from cached_tables_ if it is in the cache.
  void RemoveCachedTable(const YBTableName& table_name);
  void RemoveCachedTable(const TableId& table_id);

  // Opens the type with the given name. If the type has been opened before, returns the
  // previously opened type from cached_types_. If the type has not been opened before
  // in this client, this will do an RPC to ensure that the type exists and look up its info.
  Status GetUDType(const std::string &keyspace_name,
                   const std::string &type_name,
                   std::shared_ptr<QLType> *ql_type,
                   bool *cache_used);

  // Remove the type from cached_types_ if it is in the cache.
  void RemoveCachedUDType(const std::string& keyspace_name, const std::string& type_name);

  // Used to determine if the role has the specified permission on the canonical resource.
  // Arguments keyspace and table can be empty strings and are only used to generate the error
  // message.
  // object_type can be ObjectType::SCHEMA, ObjectType::TABLE, or
  // ObjectType::ROLE.
  // If the permission is not found, and check_mode is RETRY, this method will refresh the
  // permissions cache and retry.
  Status HasResourcePermission(const std::string &canonical_resource,
                               const ql::ObjectType &object_type,
                               const RoleName &role_name,
                               const PermissionType &permission,
                               const NamespaceName &keyspace,
                               const TableName &table,
                               const CacheCheckMode check_mode);

  Status WaitForPermissionCache();
  Result<bool> RoleCanLogin(const RoleName& role_name);
  Result<std::string> RoleSaltedHash(const RoleName& role_name);

  // Convenience method to check if role has the specified permission on the given keyspace or
  // table.
  // If the role has not the permission on neither the keyspace nor the table, and check_mode is
  // RETRY, this method will cause the permissions cache to be refreshed before retrying the check.
  Status HasTablePermission(const NamespaceName &keyspace_name,
      const TableName &table_name,
      const RoleName &role_name,
      const PermissionType permission,
      const CacheCheckMode check_mode =  CacheCheckMode::RETRY);

 private:
  client::YBClient* const client_;

  // Map from table-name to YBTable instances.
  typedef std::unordered_map<YBTableName,
                             std::shared_ptr<YBTable>,
                             boost::hash<YBTableName>> YBTableByNameMap;
  YBTableByNameMap cached_tables_by_name_;

  // Map from table-id to YBTable instances.
  typedef std::unordered_map<TableId,
                             std::shared_ptr<YBTable>,
                             boost::hash<TableId>> YBTableByIdMap;
  YBTableByIdMap cached_tables_by_id_;

  std::mutex cached_tables_mutex_;

  std::shared_ptr<client::internal::PermissionsCache> permissions_cache_;

  // Map from type-name to QLType instances.
  typedef std::unordered_map<std::pair<std::string, std::string>,
                             std::shared_ptr<QLType>,
                             boost::hash<std::pair<std::string, std::string>>> YBTypeMap;
  YBTypeMap cached_types_;
  std::mutex cached_types_mutex_;
};

} // namespace client
} // namespace yb
