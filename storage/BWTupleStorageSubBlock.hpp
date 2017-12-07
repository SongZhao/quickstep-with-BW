/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 **/

#ifndef QUICKSTEP_STORAGE_BW_TUPLE_STORAGE_SUB_BLOCK_HPP_
#define QUICKSTEP_STORAGE_BW_TUPLE_STORAGE_SUB_BLOCK_HPP_

#include <unordered_map>
#include <vector>

#include "catalog/CatalogTypedefs.hpp"
#include "expressions/predicate/PredicateCost.hpp"
#include "storage/SubBlockTypeRegistryMacros.hpp"
#include "storage/TupleStorageSubBlock.hpp"
#include "types/TypedValue.hpp"
#include "utility/BitVector.hpp"
#include "utility/Macros.hpp"
#include "utility/PtrVector.hpp"


#include <cstddef>
#include <cstdint>
#include <memory>

#include "catalog/CatalogRelationSchema.hpp"
#include "compression/CompressionDictionary.hpp"
#include "storage/IndexSubBlock.hpp"
#include "storage/StorageBlockInfo.hpp"
#include "storage/StorageBlockLayout.pb.h"
#include "storage/TupleIdSequence.hpp"
#include "types/operations/comparisons/ComparisonID.hpp"

#include "glog/logging.h"

namespace quickstep {

/** \addtogroup Storage
 *  @{
 */

/**
 * @brief An IndexSubBlock which implements a base class for BitWeaving methods.
 *        It automatically selects most efficient coding for the compressed
 *        column (dictionary coding with external or internal dictionary, or
 *        truncation).
 **/
class BWTupleStorageSubBlock : public TupleStorageSubBlock {
 public: 
  BWTupleStorageSubBlock(const CatalogRelationSchema &relation,
                       const TupleStorageSubBlockDescription &description,
                       const bool new_block,
                       void *sub_block_memory,
                       const std::size_t sub_block_memory_size)
                       : TupleStorageSubBlock(relation,
                           description,
                           new_block,
                           sub_block_memory,
                           sub_block_memory_size) {
  }


  DISALLOW_COPY_AND_ASSIGN(BWTupleStorageSubBlock);
};

/** @} */

}  // namespace quickstep

#endif  // QUICKSTEP_STORAGE_BW_TUPLE_STORAGE_SUB_BLOCK_HPP_

