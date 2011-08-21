#pragma once

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

#include "libpal/pal_memory.h"
#include "libpal/pal_allocator.h"

struct palMemBlob {
protected:
  void* buffer;
  uint64_t buffer_size; // in bytes
public:
  palMemBlob() {
    buffer_size = 0;
    buffer = NULL;
  }

  ~palMemBlob() {
    buffer = NULL;
    buffer_size = 0;
  }

  palMemBlob(void* buffer, uint64_t buffer_len) {
    this->buffer = buffer;
    this->buffer_size = buffer_len;
  }

  template<typename T>
  T* GetPtr(uint64_t byte_offset = 0) const {
    uintptr_t addr = reinterpret_cast<uintptr_t>(buffer);
    return reinterpret_cast<T*>(addr+byte_offset);
  }

  void* GetPtr(uint64_t byte_offset = 0) const {
    uintptr_t addr = reinterpret_cast<uintptr_t>(buffer);
    return reinterpret_cast<void*>(addr+byte_offset);
  }

  uint64_t GetBufferSize() const {
    return buffer_size;
  }

  void Reset() {
    buffer_size = 0;
    buffer = NULL;
  }
};

template<typename T>
struct palTypeBlob {
  T* elements;
  uint64_t size;

  palTypeBlob<T>() {
    elements = NULL;
    size = 0;
  }

  palTypeBlob<T>(T* e, uint64_t num_e) {
    elements = e;
    size = num_e;
  }
};

class palGrowingMemoryBlob {
  void* buffer;
  uint64_t buffer_capacity;
  uint64_t buffer_size;
  palAllocatorInterface* allocator_;
public:
  palGrowingMemoryBlob() {
    buffer = NULL;
    buffer_size = 0;
    buffer_capacity = 0;
    allocator_ = NULL;
  }
  palGrowingMemoryBlob(palAllocatorInterface* allocator) {
    buffer = NULL;
    buffer_capacity = 0;
    buffer_size = 0;
    allocator_ = allocator;
  }

  void SetAllocator(palAllocatorInterface* allocator) {
    allocator_ = allocator;
  }

  ~palGrowingMemoryBlob() {
    if (allocator_ != NULL && buffer != NULL) {
      allocator_->Deallocate(buffer);
    } 
  }

  void IncreaseCapacity(uint64_t new_capacity) {
    if (buffer_capacity == 0) {
      // initial growth
      buffer = allocator_->Allocate(new_capacity);
      buffer_capacity = new_capacity;
    } else if (new_capacity > buffer_capacity) {
      // create new buffer
      void* new_buffer = allocator_->Allocate(new_capacity);
      // copy old buffer into new buffer
      palMemoryCopyBytes(new_buffer, buffer, buffer_size);
      // free old buffer
      allocator_->Deallocate(buffer);

      buffer = new_buffer;
      buffer_capacity = new_capacity;
    }
  }

  void SetSize(uint64_t new_size) {
    if (new_size > buffer_capacity) {
      IncreaseCapacity(new_size);
    }
    buffer_size = new_size;
  }

  uint64_t Append(void* data, uint64_t data_size) {
    if (buffer_size + data_size >= buffer_capacity) {
      // need more room in buffer
      uint64_t new_capacity = 2*buffer_capacity;
      if (new_capacity == 0) {
        new_capacity = 64;
      }
      while (new_capacity <= (buffer_size+data_size)) {
        new_capacity *= 2;
      }
      IncreaseCapacity(new_capacity);
    }
    uintptr_t target_address = reinterpret_cast<uintptr_t>(buffer) + (uintptr_t)buffer_size;
    palMemoryCopyBytes((void*)target_address, data, data_size);
    buffer_size += data_size;
    uintptr_t base_address = reinterpret_cast<uintptr_t>(buffer);
    return target_address - base_address;
  }

  void Clear() {
    buffer_size = 0;
  }

  void Reset(bool deallocate_buffer = true) {
    if (allocator_ != NULL && buffer != NULL && deallocate_buffer == false) {
      allocator_->Deallocate(buffer);
    }
    buffer = NULL;
    buffer_capacity = 0;
    buffer_size = 0;
  }

  void* GetPtr(uint64_t offset) {
    uintptr_t byte_address = reinterpret_cast<uintptr_t>(buffer);
    return reinterpret_cast<void*>(byte_address+offset);
  }

  uint64_t GetBufferSize() const {
    return buffer_size;
  }

  uint64_t GetBufferCapacity() const {
    return buffer_capacity;
  }

  void* GetPtr() {
    return buffer;
  }

  void GetBlob(palMemBlob* blob) const {
    *blob = palMemBlob(buffer, buffer_size);
  }
};