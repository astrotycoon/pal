#ifndef __LIBPAL_H__
#define __LIBPAL_H__

/* TODO:
Entire library:
  Switch to copy-and-swap: http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom and http://www.gmannickg.com/?p=95
  Make palAllocator into a virtual interface, all containers take one as a constructor.
  
palSimd:
  Test and/or implement true SIMD versions of Sin, Cos, Tan, Acos, Asin, Atan, Atan2, Pow, Log and Exp.

palObjectIDManager:
  Template for type safety
  An ID -> object tracker
  Look at Terrance Cohen's component system and http://altdevblogaday.com/managing-decoupling

palJSONEncoder:
  Simple, stateful, no allocations, etc.

palAtomic:
  Finish palAtomicAddress and palAtomicInt64
  Add Acquire and Release operations

palAllocators:
  palStackAllocator:
    Adapt to palAllocator interface
  ! Make sure all allocators take a string name in the constructor
  ! Make a fixed array allocator, plug it into palArray.

Random number generator:
  Create new github project SFMTOO, import unmodified SFMT, port it to a class
  Move all random number generation into a class, look at c# for reference
  Expose "extern palRandom g_palRandomNumberGenerator" for code that doesn't care

Containers:
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

palLinearScopedAllocator:
  From DICE presentation. Linear allocator with support for calling destructors.

ObjectModel:
  palObject:
    explicit reference counting base object

Threading:
  TLS (dynamic and static)
*/

#include "libpal/pal_platform.h"
#include "libpal/pal_types.h"
#include "libpal/pal_scalar.h"
#include "libpal/pal_simd.h"
#include "libpal/pal_timer.h"
#include "libpal/pal_atomic.h"
#include "libpal/pal_align.h"
#include "libpal/pal_debug.h"
#include "libpal/pal_string.h"
#include "libpal/pal_random.h"
#include "libpal/pal_thread.h"
#include "libpal/pal_file.h"
#include "libpal/pal_endian.h"
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
#include "libpal/pal_delegate.h"
#include "libpal/pal_event.h"
#include "libpal/pal_time_line.h"
#include "libpal/pal_json.h"
#include "libpal/pal_object_id_table.h"
#include "libpal/pal_socket.h"
#include "libpal/pal_tcp_client.h"
#include "libpal/pal_tcp_listener.h"
#include "libpal/pal_web_socket_server.h"
#include "libpal/pal_md5.h"

#endif
