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

#pragma once

#include "libpal/pal_hash_map.h"
#include "libpal/pal_atom.h"
#include "libpal/pal_thread.h"
#include "libpal/pal_allocator.h"

// 32kb at a time
#define PAL_ATOM_PAGE_SIZE (32768)

static palMutex _atom_table_mutex;

struct palAtomMemoryManager {
  char* page;
  uint32_t offset;
  uint32_t total_allocated_memory;
  uint32_t total_wasted_memory;
  uint32_t total_large_strings;
  void Init();
  char* CopyString(const char* str);
};

void palAtomMemoryManager::Init() {
  page = NULL;
  offset = 0;
  total_allocated_memory = 0;
  total_wasted_memory = 0;
  total_large_strings = 0;
}

char* palAtomMemoryManager::CopyString(const char* str) {
  uint32_t length = palStringLength(str) + 1;

  if (length >= PAL_ATOM_PAGE_SIZE) {
    // very large string, won't fit in page
    total_large_strings++;
    total_allocated_memory += length;
    return palStringDuplicate(str);
  }
  const uint32_t page_size = ((palPageAllocator*)g_PageAllocator)->GetPageSize();
  if (page == NULL) {
    // initial run
    page = (char*)g_PageAllocator->Allocate(PAL_ATOM_PAGE_SIZE, page_size);
    offset = 0;
    total_allocated_memory += PAL_ATOM_PAGE_SIZE;
  }
  if (offset + length >= PAL_ATOM_PAGE_SIZE) {
    total_wasted_memory += (PAL_ATOM_PAGE_SIZE - offset);
    total_allocated_memory += PAL_ATOM_PAGE_SIZE;
    page = (char*)g_PageAllocator->Allocate(PAL_ATOM_PAGE_SIZE, page_size);
    offset = 0;
  }
  char* copy = page+offset;
  palMemoryCopyBytes(page+offset, str, length);
  offset += length;
  return copy;
}

static palAtomMemoryManager buffer;
static palArray<const char*> _atom_strings;
static palHashMap<const char*, palAtom> _string_to_atom_map;
static palAtom atom_counter = 0;

static palAtom palAtomAddString(const char* str) {
  palAtom atom = atom_counter++;
  int r = _atom_strings.push_back(str);
  palAssert(atom == r);
  _string_to_atom_map.Insert(str, atom);
  return atom;
}

void palAtomInitialize() {
  buffer.Init();
  _atom_strings.SetAllocator(g_DefaultHeapAllocator);
  _string_to_atom_map.SetAllocator(g_DefaultHeapAllocator);
  palMutexDescription md;
  md.initial_ownership = false;
  md.name = "palAtomTableMutex";
  md.recursion_policy = kPalThreadRecursionPolicyAllowed;
  int r = _atom_table_mutex.Create(md);
  palAssert(r == 0);
  // atom 0 is the NULL string.
  _atom_strings.push_back(NULL);
  atom_counter++;
}

void palAtomShutdown() {
  _atom_strings.Reset();
  _string_to_atom_map.Reset();
}

void palAtomGetStats(uint32_t* total_memory, uint32_t* wasted_memory, uint32_t* large_strings) {
  palScopedMutex m(&_atom_table_mutex);
  *total_memory = buffer.total_allocated_memory;
  *wasted_memory = buffer.total_wasted_memory;
  *large_strings = buffer.total_large_strings;
}

palAtom palAtomTryString(const char* str) {
  palScopedMutex m(&_atom_table_mutex);
  palAtom* result = _string_to_atom_map.Find(str);
  if (!result) {
    return 0;
  }
  return *result;
}

palAtom palAtomFromStaticString(const char* str) {
  palScopedMutex m(&_atom_table_mutex);
  palAtom* result = _string_to_atom_map.Find(str);
  if (!result) {
    return palAtomAddString(str);
  } else {
    return *result;
  }
}

palAtom palAtomFromString(const char* str) {
  palScopedMutex m(&_atom_table_mutex);
  palAtom* result = _string_to_atom_map.Find(str);
  if (!result) {
    return palAtomAddString(buffer.CopyString(str));
  } else {
    return *result;
  }
}

const char* palAtomToString(palAtom atom) {
  if (atom > 0 && atom < atom_counter) {
    return _atom_strings[atom];
  }
  return NULL;
}