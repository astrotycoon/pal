#ifndef __LIBPAL_H__
#define __LIBPAL_H__

/* TODO:
palSimd:
  Finish rest of libm functions -> pow, inverse trig functions
  More unit tests
    Test libm functions (sin, cos, etc)

palAtomic:
  Finish palAtomicAddress and palAtomicInt64

palAllocators:
  Ideas from EA-STL:
    Add support for alignment_offset to all allocators

    Provide wrapper macros for new, new[], delete and delete[]. Implement them on top of a palAllocator concept.
      Add a "default system allocator" interface?
        Have the above macros go through it if no allocator passed in

  palPoolAllocator:
    Adapt to palAllocator interface

  palStackAllocator:
    Adapt to palAllocator interface

  palHeapAllocator:
    Adapt to palAllocator interface

  palLinearScopedAllocator:
    From DICE presentation. Linear allocator with support for calling destructors.
    Adapt to palAllocator interface

  Random number generator:
    Move all random number generation into a class
    Expose "extern palRandom g_palRandomNumberGenerator" for people who don't care

Containers:
  Add alignment template parameter to all container types
  Add allocator template parameter to all container types

  Export alignment, node_type, node_size and allocator from all container types


  Modify palListNodePool to become an allocator that is passed as a template argument
  palArray:  
  palArray<T> CopySlice(int start, int end);
  palArray<T> CutSlice(int start, int end);
  void AppendArray(const palArray<T>& array, int source_start_index, int dest_start_index);
  void AppendArray(const palArray<T>& array, int source_start_index, int dest_start_index, int source_count);
  void AppendItem(const T& item, int copies);

  Add single linked list
    Intrusive and Non-intrusive versions

  Support high water marks for each container

  Add palFixedSize* container types
    Support statically allocate memory inside container and being passed a buffer
    Support optional growing  
    Add palStaticArray<T,N> where N is the number of entries statically allocated internally
      Break common parts of palArray and palStaticArray into palArrayImpl
  
  Unit Tests:
    Switch to proper unit testing framework
    palHashSet
    palHashMapCache  


  Benchmark for Array, HashMap container

Algorithms:
  Finish binarySearch <,>,=,<=,>= API
  Unit tests

ObjectModel:
  palObject:
    explicit reference counting base object

Threading:
  TLS (dynamic and static)
*/

#include "libpal/pal_platform.h"
#include "libpal/pal_types.h"
#include "libpal/pal_simd.h"
#include "libpal/pal_timer.h"
#include "libpal/pal_atomic.h"
#include "libpal/pal_align.h"
#include "libpal/pal_debug.h"
#include "libpal/pal_string.h"
#include "libpal/pal_random.h"
#include "libpal/pal_thread.h"
#include "libpal/pal_file.h"
#include "libpal/pal_algorithms.h"
#include "libpal/pal_pool_allocator.h"
#include "libpal/pal_heap_allocator.h"
#include "libpal/pal_stack_allocator.h"
#include "libpal/pal_array.h"
#include "libpal/pal_min_heap.h"
#include "libpal/pal_image.h"
#include "libpal/pal_hash_map.h"
#include "libpal/pal_hash_map_cache.h"
#include "libpal/pal_hash_set.h"
#include "libpal/pal_hash_functions.h"
#include "libpal/pal_list.h"
#include "libpal/pal_ilist.h"
#include "libpal/pal_tokenizer.h"
#include "libpal/pal_compacting_allocator.h"
#include "libpal/pal_allocator.h"
#include "libpal/pal_font_rasterizer_stb.h"
#include "libpal/pal_font_rasterizer_freetype.h"
#include "libpal/pal_utf8.h"
#include "libpal/pal_profiler.h"

#endif
