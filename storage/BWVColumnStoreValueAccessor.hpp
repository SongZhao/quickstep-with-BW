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

#ifndef QUICKSTEP_STORAGE_BWV_COLUMN_STORE_VALUE_ACCESSOR_HPP_
#define QUICKSTEP_STORAGE_BWV_COLUMN_STORE_VALUE_ACCESSOR_HPP_

#include <vector>

#include "catalog/CatalogRelationSchema.hpp"
#include "catalog/CatalogTypedefs.hpp"
#include "storage/StorageBlockInfo.hpp"
#include "storage/ValueAccessor.hpp"
#include "types/Type.hpp"
#include "types/TypedValue.hpp"
#include "utility/BitVector.hpp"
#include "utility/Macros.hpp"
#include "utility/PtrVector.hpp"
#include "storage/BWVColumnStoreTupleStorageSubBlock.hpp"
namespace quickstep {

class BWVColumnStoreTupleStorageSubBlock;

class BWVColumnStoreValueAccessorHelper {
 public:
  BWVColumnStoreValueAccessorHelper(const CatalogRelationSchema &relation,
                                      const tuple_id num_tuples,
                                      const std::vector<void*> &column_stripes,
                                      const PtrVector<BitVector<false>, true> &column_null_bitmaps)
      : relation_(relation),
        num_tuples_(num_tuples),
        column_stripes_(column_stripes),
        column_null_bitmaps_(column_null_bitmaps) {
  }

  inline tuple_id numPackedTuples() const {
    return num_tuples_;
  }

  /**
   * @brief Returns whether this accessor has a fast strided ColumnAccessor available
   *        that can be used to optimize memory access in a tight loop iteration
   *        over the underlying storage block.
   *
   * @return true if fast ColumnAccessor is supported, otherwise false.
   */
  inline bool isColumnAccessorSupported() const {
    return true;
  }

  /**
   * @brief Get a pointer to a ColumnAccessor object that provides a fast strided memory
   *        access on the underlying storage block.
   * @note The ownership of the returned object lies with the caller.
   * @warning This method should only be called if isColumnAccessorSupported() method
   *          returned true. If ColumnAccessor is not supported this method will return a nullptr.
   *
   * @param current_tuple_position A constant reference to the tuple position in the containing
   *        ValueAccessor. This reference value is shared between the containing ValueAccessor &
   *        a ColumnAccessor. However, a ColumnAccessor *CANNOT* modify this tuple position.
   * @param attr_id The attribute id on which this ColumnAccessor will be created.
   *
   * @return A pointer to a ColumnAccessor object with specific properties set that can be used
   *         in a tight loop iterations over the underlying storage block.
   **/
  template <bool check_null = true>
  inline const ColumnAccessor<check_null>* getColumnAccessor(const tuple_id &current_tuple_position,
                                                             const attribute_id attr_id) const {
    std::cout << "Try to get a column accessor" << std::endl;
    DCHECK(relation_.hasAttributeWithId(attr_id));
    const void* base_location = static_cast<const char*>(column_stripes_[attr_id]);
    const std::size_t stride = relation_.getAttributeById(attr_id)->getType().maximumByteLength();
    std::unique_ptr<ColumnAccessor<check_null>> column_accessor;
    if (check_null) {
      // The nullable_base might get initialized to -1 if column_null_bitmaps returns false for
      // the given attribute. Setting the nullable_base to -1 will mean that
      // column accessor will always evaluate null check to false.
      const int nullable_base = (!column_null_bitmaps_.elementIsNull(attr_id)) ? 0 : -1;
      const unsigned nullable_stride = 1;
      column_accessor.reset(new ColumnAccessor<check_null>(current_tuple_position,
                                                           num_tuples_,
                                                           base_location,
                                                           stride,
                                                           &(column_null_bitmaps_[attr_id]),
                                                           nullable_base,
                                                           nullable_stride));
    } else {
      column_accessor.reset(new ColumnAccessor<check_null>(current_tuple_position,
                                                           num_tuples_,
                                                           base_location,
                                                           stride));
    }
    return column_accessor.release();
  }

  template <bool check_null>
  inline const void* getAttributeValue(const tuple_id tuple,
                                       const attribute_id attr) const {
    std::cout << "----- to get an attribute value with type through BWVColumnStoreValueAccessorHelper" << std::endl;
    DEBUG_ASSERT(tuple < num_tuples_);
    DEBUG_ASSERT(relation_.hasAttributeWithId(attr));
    if (check_null
        && (!column_null_bitmaps_.elementIsNull(attr))
        && column_null_bitmaps_[attr].getBit(tuple)) {
      return nullptr;
    } 
  //num_codes_per_word_.resize(relation_.getMaxAttributeId());
 // num_codes_per_segment_.resize(relation_.getMaxAttributeId());
 // num_words_per_segment_.resize(relation_.getMaxAttributeId());
 // num_words_per_code_.resize(relation_.getMaxAttributeId());
  //    int  num  = (sizeof(WordUnit)<<3)/((relation_.getAttributeById(attr)->getType().maximumByteLength()<<3)+1);
      int num_w_p_c, n_c_p_s, n_t_i_c_s, n_s, col, row, n_w_p_s;
      int  num  = 64 / ((relation_.getAttributeById(attr)->getType().maximumByteLength()<<3)+1);
 	if(num  == 0)
	{
       	num_w_p_c = ((relation_.getAttributeById(attr)->getType().maximumByteLength()<<3)+1)/64 + 1;
   	n_c_p_s =  ((relation_.getAttributeById(attr)->getType().maximumByteLength()<<3));		
	}
	else
	{
     		num_w_p_c  = 1;
     		n_c_p_s = num  * ((relation_.getAttributeById(attr)->getType().maximumByteLength()<<3)); 
	}
    n_w_p_s = num_w_p_c * (relation_.getAttributeById(attr)->getType().maximumByteLength()<<3);
    n_t_i_c_s  = tuple % n_c_p_s;
    n_s = tuple / n_c_p_s;
    std::cout << "tuple # is " << tuple << std::endl;
    std::cout << "in accessor, n_s is "<< n_s << "item number in CS is " << n_t_i_c_s << "num of word/code is " << n_w_p_s << std::endl;
   if(num != 0)
		{
		 int row_size = ((relation_.getAttributeById(attr)->getType().maximumByteLength()<<3)*num_w_p_c);
	 	 std::cout << "row size for this attr's segment is " << row_size << std::endl;
           	 col = n_t_i_c_s / row_size;
           	 row = n_t_i_c_s % row_size;
		}
	  else
		{
	         col = 0;
		 row = n_t_i_c_s;
		}
    return static_cast<const char*>(column_stripes_[attr]
           +  row * (sizeof(std::size_t)<<3)  * num_w_p_c
                        + n_s * n_w_p_s * (sizeof(std::size_t)<<3));
  }

  inline TypedValue getAttributeValueTyped(const tuple_id tuple,
                                           const attribute_id attr) const {
    //Kan: test
    std::cout << "----- to get an attributeValueTyped with type through BWVColumnStoreValueAccessorHelper" << std::endl;

    const Type &attr_type = relation_.getAttributeById(attr)->getType();
    const void *untyped_value = getAttributeValue<true>(tuple, attr);
    return (untyped_value == nullptr)
        ? attr_type.makeNullValue()
        : attr_type.makeValue(untyped_value, attr_type.maximumByteLength());
  }

 private:
  const CatalogRelationSchema &relation_;
  const tuple_id num_tuples_;
  const std::vector<void*> &column_stripes_;
  const PtrVector<BitVector<false>, true> &column_null_bitmaps_;

  DISALLOW_COPY_AND_ASSIGN(BWVColumnStoreValueAccessorHelper);
};

typedef PackedTupleStorageSubBlockValueAccessor<
    BWVColumnStoreTupleStorageSubBlock,
    BWVColumnStoreValueAccessorHelper,
    ValueAccessor::Implementation::kBWVColumnStore>
        BWVColumnStoreValueAccessor;

}  // namespace quickstep

#endif  // QUICKSTEP_STORAGE_BWV_COLUMN_STORE_VALUE_ACCESSOR_HPP_
