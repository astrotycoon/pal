/*
	Copyright (c) 2010 John McCutchan <john@johnmccutchan.com>

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

#ifndef __PAL_POOL_ALLOCATOR_H
#define __PAL_POOL_ALLOCATOR_H

#include "libpal/pal_platform.h"
#include "libpal/pal_align.h"
#include "libpal/pal_spinlock.h"

template <int POOL_ALIGNMENT = 16>
class palPoolAllocator
{
protected:
  palSpinlock spinlock_;
	uint32_t pool_element_size_;
	uint32_t num_pool_elements_;
	uint32_t num_free_pool_elements_;

	unsigned char* pool_base_ptr_;
	unsigned char* free_ptr_;

	bool PointerFromPool (void* ptr)
	{
		return ptr >= pool_base_ptr_ && ptr <= (pool_base_ptr_ + (pool_element_size_ * num_pool_elements_-1));
	}
public:

  palPoolAllocator() : pool_element_size_(0), num_pool_elements_(0), num_free_pool_elements_(0), pool_base_ptr_(0), free_ptr_(0) {
    palSpinlockInit(&spinlock_);
  }
  
  palPoolAllocator(uint32_t element_size, uint32_t num_elements, void* pool_memory) {
    palSpinlockInit(&spinlock_);
    Create(element_size, num_elements, pool_memory);
  }

  void Create(uint32_t element_size, uint32_t num_elements, void* pool_memory) {  
		pool_element_size_ = element_size;
		num_pool_elements_ = num_elements;
		num_free_pool_elements_ = num_elements;
    pool_base_ptr_ = static_cast<unsigned char*>(pool_memory);

		free_ptr_ = pool_base_ptr_;
		/* Inside each "element" we store a pointer to the next free item */
		unsigned char* current_element = pool_base_ptr_;
		unsigned char* next_free = pool_base_ptr_+element_size;
		for (uint32_t i = 0; i < num_pool_elements_-1; i++)
		{
			*(unsigned char**)current_element = next_free;
			next_free += element_size;
			current_element += element_size;
		}
	}

	~palPoolAllocator ()
	{
	}

	uint32_t GetNumFree()
	{
		return num_free_pool_elements_;
	}

	void* Malloc()
	{
    palSpinlockTake(&spinlock_);
		if (num_free_pool_elements_ == 0)
		{
			return NULL;
		}
		num_free_pool_elements_--;
		unsigned char* new_element = free_ptr_;
		free_ptr_ = *(unsigned char**)new_element;
    palSpinlockRelease(&spinlock_);
		return new_element;
	}

	void Free (void* p)
	{
		if (!p)
			return;

		palAssert (PointerFromPool (p));
    palSpinlockTake(&spinlock_);
		num_free_pool_elements_++;
		*(unsigned char**)p = free_ptr_;
		free_ptr_ = (unsigned char*)p;
    palSpinlockRelease(&spinlock_);
	}
  
  void FreeAll()
  {
    palSpinlockTake(&spinlock_);
    free_ptr_ = pool_base_ptr_;
		/* Inside each "element" we store a pointer to the next free item */
		unsigned char* current_element = pool_base_ptr_;
		unsigned char* next_free = pool_base_ptr_+pool_element_size_;
		for (uint32_t i = 0; i < num_pool_elements_-1; i++)
		{
			*(unsigned char**)current_element = next_free;
			next_free += pool_element_size_;
			current_element += pool_element_size_;
		}
    num_free_pool_elements_ = num_pool_elements_;
    palSpinlockRelease(&spinlock_);
  }
};

#endif