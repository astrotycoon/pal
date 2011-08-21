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

#include "libpal/pal_algorithms.h"
#include "libpal/pal_memory.h"
#include "libpal/pal_allocator.h"

#define BUFFER_SIZE 64*1024

palAllocatorInterface* g_PageAllocator = NULL;
palAllocatorInterface* g_StaticHeapAllocator = NULL;
palAllocatorInterface* g_DefaultHeapAllocator = NULL;
palAllocatorInterface* g_StdProxyAllocator = NULL;
palAllocatorInterface* g_StringProxyAllocator = NULL;
palAllocatorInterface* g_AllocatorTrackerProxyAllocator = NULL;
palAllocatorInterface* g_TrackingAllocator = NULL;
palAllocatorInterface* g_FileProxyAllocator = NULL;
palAllocatorInterface* g_OpticsAllocator = NULL;
palAllocatorInterface* g_FontAllocator = NULL;
palAllocatorTracker* g_AllocatorTracker = NULL;

static char buffer[BUFFER_SIZE];

int palAllocatorInit() {
  char* p = &buffer[0];
  g_StaticHeapAllocator = new (p) palHeapAllocator("Static Heap");
  p += sizeof(*g_StaticHeapAllocator);
  static uint32_t static_heap_size = BUFFER_SIZE-sizeof(*g_StaticHeapAllocator);
  ((palHeapAllocator*)g_StaticHeapAllocator)->Create(p, static_heap_size);

  g_PageAllocator = g_StaticHeapAllocator->Construct<palPageAllocator>();
  g_DefaultHeapAllocator = g_StaticHeapAllocator->Construct<palHeapAllocator>("Default Heap");
  ((palHeapAllocator*)g_DefaultHeapAllocator)->Create((palPageAllocator*)g_PageAllocator);
  g_TrackingAllocator = g_StaticHeapAllocator->Construct<palTrackingAllocator>("Default Heap Tracker", g_DefaultHeapAllocator);

  // We swap tracking and default allocators here, enabling tracking by default.
  palSwap(g_TrackingAllocator, g_DefaultHeapAllocator);


  g_StdProxyAllocator = g_StaticHeapAllocator->Construct<palProxyAllocator>("STD Proxy", g_DefaultHeapAllocator);
  g_AllocatorTrackerProxyAllocator = g_StaticHeapAllocator->Construct<palProxyAllocator>("AllocatorTracker", g_DefaultHeapAllocator);
  g_StringProxyAllocator = g_StaticHeapAllocator->Construct<palProxyAllocator>("Strings", g_DefaultHeapAllocator);
  g_FileProxyAllocator = g_StaticHeapAllocator->Construct<palProxyAllocator>("Files", g_DefaultHeapAllocator);
  g_OpticsAllocator = g_StaticHeapAllocator->Construct<palProxyAllocator>("Optics Device", g_DefaultHeapAllocator);
  g_FontAllocator = g_StaticHeapAllocator->Construct<palProxyAllocator>("Font Rasterization", g_DefaultHeapAllocator);

  g_AllocatorTracker = g_StaticHeapAllocator->Construct<palAllocatorTracker>();
  g_AllocatorTracker->SetAllocator(g_StaticHeapAllocator);
  g_AllocatorTracker->RegisterAllocator(g_StaticHeapAllocator, NULL);
  g_AllocatorTracker->RegisterAllocator(g_PageAllocator, NULL);
  g_AllocatorTracker->RegisterAllocator(g_TrackingAllocator, g_PageAllocator);
  g_AllocatorTracker->RegisterAllocator(g_DefaultHeapAllocator, g_TrackingAllocator);
  g_AllocatorTracker->RegisterAllocator(g_StdProxyAllocator, g_DefaultHeapAllocator);
  g_AllocatorTracker->RegisterAllocator(g_StringProxyAllocator, g_DefaultHeapAllocator);
  g_AllocatorTracker->RegisterAllocator(g_AllocatorTrackerProxyAllocator, g_DefaultHeapAllocator);
  g_AllocatorTracker->RegisterAllocator(g_FileProxyAllocator, g_DefaultHeapAllocator);
  g_AllocatorTracker->RegisterAllocator(g_OpticsAllocator, g_DefaultHeapAllocator);
  g_AllocatorTracker->RegisterAllocator(g_FontAllocator, g_DefaultHeapAllocator);

  return 0;
}

int palAllocatorShutdown() {
  // Unswap tracking and default heap allocator, undoing the swap in palAllocatorInit

  palSwap(g_TrackingAllocator, g_DefaultHeapAllocator);
  ((palTrackingAllocator*)g_TrackingAllocator)->ConsoleDump();
  g_AllocatorTracker->ConsoleDump();
  g_StaticHeapAllocator->Destruct(g_AllocatorTracker);

  g_StaticHeapAllocator->Destruct(g_StdProxyAllocator);
  g_StaticHeapAllocator->Destruct(g_OpticsAllocator);
  g_StaticHeapAllocator->Destruct(g_FontAllocator);
  g_StaticHeapAllocator->Destruct(g_FileProxyAllocator);
  g_StaticHeapAllocator->Destruct(g_StringProxyAllocator);
  g_StaticHeapAllocator->Destruct(g_AllocatorTrackerProxyAllocator);
  g_StaticHeapAllocator->Destruct(g_TrackingAllocator);
  ((palHeapAllocator*)g_DefaultHeapAllocator)->Destroy();
  g_StaticHeapAllocator->Destruct(g_DefaultHeapAllocator);
  g_StaticHeapAllocator->Destruct(g_PageAllocator);
  ((palHeapAllocator*)g_StaticHeapAllocator)->Destroy();
  ((palHeapAllocator*)g_StaticHeapAllocator)->~palHeapAllocator();
  return 0;
}

palAllocatorTracker::palAllocatorTracker() : _allocator(NULL), _root() {
}

palAllocatorTracker::~palAllocatorTracker() {
}

void palAllocatorTracker::SetAllocator(palAllocatorInterface* allocator) {
  _allocator = allocator;
  _root.children.SetAllocator(_allocator);
}

void palAllocatorTracker::RegisterAllocator(palAllocatorInterface* allocator, palAllocatorInterface* parent_allocator) {
  palTrackedAllocator* tracked_parent = NULL;
  if (parent_allocator) {
    tracked_parent = LocateParentAllocator(&_root, parent_allocator);
    if (tracked_parent == NULL) {
      tracked_parent = AddAllocator(&_root, parent_allocator);
    }
  } else {
    tracked_parent = &_root;
  }
  AddAllocator(tracked_parent, allocator);
}

void palAllocatorTracker::ConsoleDump() {
  palPrintf("Dumping Allocator State\n");
  ConsoleDump(0, &_root);
}

static void print_n_spaces(int n) {
  while (n >= 0) {
    n--;
    palPrintf(" ");
  }
}

void palAllocatorTracker::ConsoleDump(int level, palTrackedAllocator* root) {
  const int num_children = root->children.GetSize();
  if (root->allocator == NULL) {
    for (int i = 0; i < num_children; i++) {
      ConsoleDump(level, &root->children[i]);
    }
    return;
  }

  const uint64_t bytes_allocated = root->allocator->GetMemoryAllocated();
  const uint64_t allocations = root->allocator->GetNumberOfAllocations();
  float kb = (float)bytes_allocated/1024.0f;
  print_n_spaces(level*2);
  palPrintf("%s [%lld] [%f KB]\n", root->allocator->GetName(), allocations, kb);
  for (int i = 0; i < num_children; i++) {
    ConsoleDump(level+1, &root->children[i]);
  }
}

palTrackedAllocator* palAllocatorTracker::LocateParentAllocator(palTrackedAllocator* root, palAllocatorInterface* allocator) {
  // check self
  if (root->allocator == allocator) {
    return root;
  }
  {
    // check children
    const int num_children = root->children.GetSize();
    for (int i = 0; i < num_children; i++) {
      if (root->children[i].allocator == allocator) {
        return &root->children[i];
      }
    }
  }
  {
    // recurse
    const int num_children = root->children.GetSize();
    palTrackedAllocator* found = NULL;
    for (int i = 0; i < num_children; i++) {
      found = LocateParentAllocator(&root->children[i], allocator);
      if (found) {
        return found;
      }
    }
  } 
  return NULL;
}

palTrackedAllocator* palAllocatorTracker::AddAllocator(palTrackedAllocator* root, palAllocatorInterface* allocator) {
  palTrackedAllocator tracked_allocator;

  tracked_allocator.children.SetAllocator(_allocator);
  tracked_allocator.allocator = allocator;
  tracked_allocator.parent = NULL;

  int i = root->children.push_back(tracked_allocator);
  return &root->children[i];
}
