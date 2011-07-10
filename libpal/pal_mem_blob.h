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

struct palMemBlob {
protected:
  void* buffer;
  int buffer_size; // in bytes
  bool own_buffer;
public:
  palMemBlob() {
    buffer_size = 0;
    buffer = NULL;
    own_buffer = false;
  }

  ~palMemBlob() {
    if (buffer && own_buffer) {
      palFree(buffer);
    }
    buffer = NULL;
    buffer_size = 0;
    own_buffer = false;
  }

  palMemBlob(void* buffer, int buffer_len, bool own_buffer) {
    this->buffer = buffer;
    this->buffer_size = buffer_len;
    this->own_buffer = own_buffer;
  }

  void* GetPtr(uint32_t offset = 0) const {
    uintptr_t addr = reinterpret_cast<uintptr_t>(buffer);
    return reinterpret_cast<void*>(addr+offset);
  }

  int GetBufferSize() const {
    return buffer_size;
  }

  void* StealBuffer() {
    own_buffer = false;
    return buffer;
  }

  void TakeOwnerShip() {
    own_buffer = true;
  }

  void Reset() {
    if (own_buffer && buffer) {
      palFree(buffer);
    }
    own_buffer = false;
    buffer_size = 0;
    buffer = NULL;
  }
};

template<typename T>
struct palTypeBlob {
  T* elements;
  int size;

  palTypeBlob<T>() {
    elements = NULL;
    size = 0;
  }

  palTypeBlob<T>(T* e, int num_e) {
    elements = e;
    size = num_e;
  }
};

struct palGrowingMemoryBlob {
  void* buffer;
  uint32_t buffer_capacity;
  uint32_t buffer_size;

  palGrowingMemoryBlob() {
    buffer = NULL;
    buffer_capacity = 0;
    buffer_size = 0;
  }

  palGrowingMemoryBlob(uint32_t initial_capacity) {
    buffer = NULL;
    buffer_capacity = 0;
    buffer_size = 0;
  }

  ~palGrowingMemoryBlob() {
    palFree(buffer);
  }

  void IncreaseCapacity(uint32_t new_capacity) {
    if (buffer_capacity == 0) {
      // initial growth
      buffer = palMalloc(new_capacity);
      buffer_capacity = new_capacity;
    } else if (new_capacity > buffer_capacity) {
      // create new buffer
      void* new_buffer = palMalloc(new_capacity);
      // copy old buffer into new buffer
      palMemoryCopyBytes(new_buffer, buffer, buffer_size);
      // free old buffer
      palFree(buffer);

      buffer = new_buffer;
      buffer_capacity = new_capacity;
    }
  }

  uint32_t Append(void* data, uint32_t data_size) {
    if (buffer_size + data_size >= buffer_capacity) {
      // need more room in buffer
      uint32_t new_capacity = 2*buffer_capacity;
      if (new_capacity == 0) {
        new_capacity = 64;
      }
      while (new_capacity <= (buffer_size+data_size)) {
        new_capacity *= 2;
      }
      IncreaseCapacity(new_capacity);
    }
    uintptr_t target_address = reinterpret_cast<uintptr_t>(buffer) + buffer_size;
    palMemoryCopyBytes((void*)target_address, data, data_size);
    buffer_size += data_size;
    uintptr_t base_address = reinterpret_cast<uintptr_t>(buffer);
    return target_address - base_address;
  }

  void Clear() {
    buffer_size = 0;
  }

  void Reset() {
    if (buffer) {
      palFree(buffer);
    }
    buffer = NULL;
    buffer_capacity = 0;
    buffer_size = 0;
  }

  void* GetPtr(uint32_t offset) {
    uintptr_t byte_address = reinterpret_cast<uintptr_t>(buffer);
    return reinterpret_cast<void*>(byte_address+offset);
  }

  uint32_t GetSize() {
    return buffer_size;
  }

  void* GetPtr() {
    return buffer;
  }
};