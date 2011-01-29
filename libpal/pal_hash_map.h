/*
	Copyright (c) 2009 John McCutchan <john@johnmccutchan.com>

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

#ifndef LIBPAL_PAL_HASH_MAP_H__
#define LIBPAL_PAL_HASH_MAP_H__

#include "libpal/pal_debug.h"
#include "libpal/pal_random.h"
#include "libpal/pal_array.h"
#include "libpal/pal_hash_functions.h"
#include "libpal/pal_hash_constants.h"

template <class Key, class Value, class HashFunction = palHashFunction<Key>, class KeyEqual = palHashEqual<Key>, uint32_t KeyAlignment = PAL_ALIGNOF(Key), uint32_t ValueAlignment = PAL_ALIGNOF(Value), typename Allocator = palAllocator>
class palHashMap
{
public:
  /* Types and constants */
  typedef palHashMap<Key, Value, HashFunction, KeyEqual, KeyAlignment, ValueAlignment, Allocator> this_type;
  typedef Key key_type;
  typedef Value value_type;
  typedef Allocator allocator_type;
  static const uint32_t key_size = sizeof(Key);
  static const uint32_t value_size = sizeof(Value);
  static const uint32_t key_alignment = KeyAlignment;
  static const uint32_t value_alignment = ValueAlignment;
protected:

  /* A chaining hash table. There are four arrays used in the implementation:
   * hash_bucket_list_head_ holds the head of each hash index list. If nothing is hashed at
   * hash_bucket_list_head_[hash_index] than it contains pal_object_hash_null. 
   * 
   * pal_object_hash_null marks the end of a list/chain.
   *
   * chain_next_ 
   * key_array_
   * value_array_
   *
   * These are three separate arrays that are indexed together. That is:
   *
   * chain_next_[3], key_array_[3] and value_array_[3] all refer to the same object.
   *
   * chain_next_ holds the next pointer for the hash chain (pal_object_hash_null if the end of chain)
   * key_array_ holds the key and value_array_ holds the value.
   *
   * Conceptually it's a singly linked list starting at hash_table_[hash_index] terminated with pal_object_hash_null.
   * But, by using arrays of integers we allocate list nodes in large quantities and close together.
   *
   */
  int hash_size_configuration_;

	palArray<int, 4, Allocator>	hash_bucket_list_head_;


  /* These three could be thought of as:
   *
   * struct hashed_item {
   *     struct hashed_item* next; (chain_next_)
   *     Key key;
   *     Value value;
   * }
   */
	palArray<int, PAL_ALIGNOF(int), Allocator>	chain_next_;
	palArray<Key, KeyAlignment, Allocator>   key_array_;
	palArray<Value, ValueAlignment, Allocator>	value_array_;

  HashFunction hash_function_;
  KeyEqual key_equal_function_;

  bool ResizeTable() {
    int size = key_array_.GetSize();

    int high = kPalHashTableSizeConfigurations[hash_size_configuration_].high_water_mark;
    int low = high >> 2;

    if (size >= low && size <= high) {
      // don't need to resize table
      return false;
    }

    if (size > high) {
      // table needs to grow
      hash_size_configuration_++;
      if (hash_size_configuration_ == kPalNumHashTableSizeConfigurations) {
        // we have reached the maximum hash table size. Please see pal_hash_constants.cpp
        palAssert(0);
      }
    } else {
      // table needs to shrink
      if (hash_size_configuration_ == 0) {
        // can't shrink
        return false;
      }
      hash_size_configuration_--;
    }

    int new_bucket_size = kPalHashTableSizeConfigurations[hash_size_configuration_].prime_size;

    // resize to new hash size configuration size
    hash_bucket_list_head_.Reset();
    hash_bucket_list_head_.Resize(new_bucket_size);

    for (int i = 0; i < new_bucket_size; i++) {
      // set all list heads to NULL
      hash_bucket_list_head_[i] = kPalHashNULL;
    }

    // rehash
    for(int i = 0; i < size; i++) {
      const Key& key = key_array_[i];
      int bucket = hash_function_(key) % new_bucket_size;

      /* Each item hashed is put at the head of the list */
      /* chain_next_[i] points to the previous head of the list */
      /* hash_table_[hash_index] points to chain_next_[i] */
      chain_next_[i] = hash_bucket_list_head_[bucket];
      hash_bucket_list_head_[bucket] = i;
    }

    return true;
  }

  void InternalTest() {
    int key_size = key_array_.GetSize();
    int value_size = value_array_.GetSize();
    int chain_size = chain_next_.GetSize();
    palAssert(key_size == value_size);
    palAssert(value_size == chain_size);
  }

  int BucketListLength(int bucket_index) {
    if (bucket_index < 0 || bucket_index >= hash_bucket_list_head_.GetSize()) {
      return 0;
    }
    int length = 0;
    int index = hash_bucket_list_head_[bucket_index];
    // while not at end of list and object is not the one we are looking for
    while (index != kPalHashNULL) {
      // move to next item
      index = chain_next_[index];
      length++;
    }
    return length;
  }
public:
	palHashMap () : hash_size_configuration_(0), hash_bucket_list_head_(), chain_next_(), key_array_(), value_array_(), hash_function_(HashFunction()), key_equal_function_(KeyEqual()) {
    int new_bucket_size = kPalHashTableSizeConfigurations[hash_size_configuration_].prime_size;
    hash_bucket_list_head_.Resize(new_bucket_size);
    for (int i = 0; i < new_bucket_size; i++) {
      // set all list heads to NULL
      hash_bucket_list_head_[i] = kPalHashNULL;
    }
	}

	/* Assignment operator */
	palHashMap<Key, Value, HashFunction, KeyEqual>& operator= (const palHashMap<Key, Value, HashFunction, KeyEqual>& map) {
		if (this == &map) return *this; // self assignment

		hash_bucket_list_head_ = map.hash_bucket_list_head_;
		chain_next_ = map.chain_next_;
		key_array_ = map.key_array_;
    hash_size_configuration_ = map.hash_size_configuration_;
		value_array_ = map.value_array_;
    key_equal_function_ = map.key_equal_function_;
    hash_function_ = map.hash_function_;
    return *this;
	}

	/* Copy constructor */
	palHashMap (const palHashMap<Key, Value, HashFunction, KeyEqual>& map) : hash_size_configuration_(hash_size_configuration_), hash_bucket_list_head_(map.hash_bucket_list_head_), chain_next_(map.chain_next_), key_array_(map.key_array_), value_array_(map.value_array_), hash_function_(map.hash_function_), key_equal_function_(map.key_equal_function_) {
	}

	bool Insert(const Key& key, const Value& value) {
    /* First try and find the key in the hash table */
    int index = FindIndex(key);
		if (index != kPalHashNULL) {
			// key is already present, so just update the value
			value_array_[index] = value;
			return true;
		}

    // push
    value_array_.push_back(value);
    key_array_.push_back(key);
    chain_next_.push_back(kPalHashNULL);
    int insert_index = chain_next_.GetSize()-1;

    if (ResizeTable()) {
      // the table was resized and the above was hashed in the process
      return true;
    }

    /* Determine which bucket this key should be chained in */
    int bucket = hash_function_(key) % hash_bucket_list_head_.GetSize();
    /* Insert this item as head of list */
		chain_next_[insert_index] = hash_bucket_list_head_[bucket];
    hash_bucket_list_head_[bucket] = insert_index;
		return true;
	}

	bool Remove(const Key& key) {
    int item_index = FindIndex(key);
		if (item_index == kPalHashNULL) {
      // not present, no need to remove
			return false;
		}

    int bucket = hash_function_(key) % hash_bucket_list_head_.GetSize();
    palAssert(hash_bucket_list_head_[bucket] != kPalHashNULL);

    // find the previous node in the bucket's list
		int index = hash_bucket_list_head_[bucket];
		int previous = kPalHashNULL;
		while (index != item_index) {
			previous = index;
			index = chain_next_[index];
		}

    // delete item's node from list
		if (previous != kPalHashNULL) {
			palAssert(chain_next_[previous] == item_index);
      // interior node - remove from list
			chain_next_[previous] = chain_next_[item_index];
		} else {
      palAssert(hash_bucket_list_head_[bucket] == item_index);
      // first node - remove from list
			hash_bucket_list_head_[bucket] = chain_next_[item_index];
		}

    // Since we store all keys, values and next pointers tightly together in
    // an array, we need to move the last item inserted into this items 
    // slot in those arrays.
    int last_item_index = key_array_.GetSize() - 1;

		// If the removed item is the last item, we are done.
		if (last_item_index == item_index) {
			value_array_.pop_back();
			key_array_.pop_back();
      chain_next_.pop_back();
      InternalTest();
			return true;
		}

		// Remove the last pair from the hash table.
		const Key& last_item_key = key_array_[last_item_index];
		int last_item_bucket = hash_function_(last_item_key) % hash_bucket_list_head_.GetSize();
    palAssert(hash_bucket_list_head_[last_item_bucket] != kPalHashNULL);

		index = hash_bucket_list_head_[last_item_bucket];
		previous = kPalHashNULL;
		while (index != last_item_index) {
			previous = index;
			index = chain_next_[index];
		}

		if (previous != kPalHashNULL) {
			palAssert(chain_next_[previous] == last_item_index);
			chain_next_[previous] = chain_next_[last_item_index];
		} else {
      palAssert(hash_bucket_list_head_[last_item_bucket] == last_item_index);
			hash_bucket_list_head_[last_item_bucket] = chain_next_[last_item_index];
		}

		// Copy the last pair into the removed item's slot
		value_array_[item_index] = value_array_[last_item_index];
		key_array_[item_index] = key_array_[last_item_index];

		// Insert the last pair into the hash table
		chain_next_[item_index] = hash_bucket_list_head_[last_item_bucket];
		hash_bucket_list_head_[last_item_bucket] = item_index;

		value_array_.pop_back();
		key_array_.pop_back();
    chain_next_.pop_back();

		return true;
	}

	void EraseRandomEntry () {
		if (IsEmpty())
			return;
		int index = palGenerateRandom() % key_array_.GetSize();
    const Key& key = key_array_[index];
    Remove(key);
	}

	int GetSize() const
	{
		return key_array_.GetSize();
	}

	const Value* GetValueAtIndex(int index) const {
		palAssert(index < key_array_.GetSize());
		return &value_array_[index];
	}

	Value* GetValueAtIndex(int index) {
		palAssert(index < key_array_.GetSize());
		return &value_array_[index];
	}

  const Key* GetKeyAtIndex(int index) const {
		palAssert(index < key_array_.GetSize());
    return &key_array_[index];
  }

  Key* GetKeyAtIndex(int index) {
    palAssert(index < key_array_.GetSize());
    return &key_array_[index];
  }

	bool IsEmpty() const {
		return key_array_.IsEmpty();
	}

	const Value* Find(const Key& key) const {
		int index = FindIndex(key);
		if (index == kPalHashNULL) {
			return NULL;
		}
		return GetValueAtIndex(index);
	}

	Value* Find(const Key& key)
	{
		int index = FindIndex(key);
		if (index == kPalHashNULL) {
			return NULL;
		}
		return GetValueAtIndex(index);
	}

	int	FindIndex(const Key& key) const {
		if (IsEmpty())
			return kPalHashNULL;

    /* Find bucket */
		int bucket = hash_function_(key) % hash_bucket_list_head_.GetSize();

		int index = hash_bucket_list_head_[bucket];
    // while not at end of list and object is not the one we are looking for
		while ((index != kPalHashNULL) && key_equal_function_(key, key_array_[index]) == false) {
      // move to next item
			index = chain_next_[index];
		}
		return index;
	}

	void Clear() {
		hash_bucket_list_head_.Clear();
		chain_next_.Clear();
		value_array_.Clear();
		key_array_.Clear();
	}

  void Reset() {
    hash_bucket_list_head_.Reset();
    chain_next_.Reset();
    value_array_.Reset();
    key_array_.Reset();
  }

	palArray<Value>& GetValueArrayReference() {
		return value_array_;
	}

	palArray<Key>& GetKeyArrayReference() {
		return key_array_;
	}

  float LoadFactor() {
    float item_size = static_cast<float>(key_array_.GetSize());
    float bucket_size = static_cast<float>(hash_bucket_list_head_.GetSize());
    return item_size/bucket_size;
  }

  float MeanLength() {
    float sum_length = 0.0f;
    float population_size = static_cast<float>(hash_bucket_list_head_.GetSize());
    for (int i = 0; i < hash_bucket_list_head_.GetSize(); i++) {
      sum_length += static_cast<float>(BucketListLength(i));
    }

    return sum_length / population_size;
  }

  float StandardDeviationLength() {
    float mean_length = MeanLength();
    float population_size = static_cast<float>(hash_bucket_list_head_.GetSize());
    float sum_diff_squared = 0.0f;
    for (int i = 0; i < hash_bucket_list_head_.GetSize(); i++) {
      float length = static_cast<float>(BucketListLength(i));
      float diff = length - mean_length;
      sum_diff_squared += (diff*diff);
    }
    return sum_diff_squared/population_size;
  }

  void dumpBucketLength(const char* file) {
    FILE* fp = fopen(file, "wb");
    if (!fp) {
      palBreakHere();
    }
    for (int i = 0; i < hash_bucket_list_head_.GetSize(); i++) {
      int length = BucketListLength(i);
      fprintf(fp, "%d %d\n", i, length);
    }
    fclose(fp);
  }
};

#endif  // LIBPAL_PAL_HASH_MAP_H__

