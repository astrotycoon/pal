#include <cstdio>
#include <cmath>
#include "libpal/libpal.h"
#include "pal_container_test.h"

bool palArrayTest() {
  palArray<int> testArray;
  testArray.SetAllocator(g_DefaultHeapAllocator);

  if (testArray.IsEmpty() == false) {
    palBreakHere();
  }

  if (testArray.GetSize() != 0) {
    palBreakHere();
  }

  testArray.push_back(3);
  palAssertBreak(testArray.GetSize() == 1);
  testArray.push_back(5);
  palAssertBreak(testArray.GetSize() == 2);
  testArray.push_back(5);
  palAssertBreak(testArray.GetSize() == 3);
  testArray.push_back(8);
  palAssertBreak(testArray.GetSize() == 4);
  testArray.push_back(3);
  palAssertBreak(testArray.GetSize() == 5);
  testArray.push_back(11);
  palAssertBreak(testArray.GetSize() == 6);

  palArray<int> ta2;
  ta2.SetAllocator(g_DefaultHeapAllocator);
  ta2.push_back(51);
  ta2.push_back(52);
  ta2.push_front(53);

  int* stolen_buffer = ta2.StealBuffer();

  ta2 = testArray;
  palAssertBreak(ta2.GetSize() == 6);

  palAssertBreak(stolen_buffer[0] == 53);
  palAssertBreak(stolen_buffer[1] == 51);
  palAssertBreak(stolen_buffer[2] == 52);

  g_DefaultHeapAllocator->Deallocate(stolen_buffer);

  {
    testArray.EraseAllStable(3);
    int results[] = { 5, 5, 8, 11};
    for (int i = 0; i < testArray.GetSize(); i++) {
      palAssertBreak(testArray[i] == results[i]);
    }
  }

  {
    testArray.EraseAll(5);
    int results[] = { 11, 8};
    for (int i = 0; i < testArray.GetSize(); i++) {
      palAssertBreak(testArray[i] == results[i]);
    }
  }

  {
    int saved_capacity = testArray.GetCapacity();
    testArray.Clear();
    palAssertBreak(saved_capacity == testArray.GetCapacity());
    palAssertBreak(testArray.GetSize() == 0);
    testArray.Reset();
    palAssertBreak(testArray.GetCapacity() == 0);
  }

  {
    palArray<int> orderedArray;
    orderedArray.SetAllocator(g_DefaultHeapAllocator);
    palArray<int> out_of_order_array;
    out_of_order_array.SetAllocator(g_DefaultHeapAllocator);
    palCompareFuncLessThan<int> CF = palCompareFuncLessThan<int>();
    out_of_order_array.push_back(5);
    out_of_order_array.push_back(-199);
    out_of_order_array.push_back(253);
    out_of_order_array.push_back(-4);
    out_of_order_array.push_back(335);
    out_of_order_array.push_back(85);
    out_of_order_array.push_back(257);
    out_of_order_array.HeapSort(CF);

    orderedArray.OrderedInsert(5, CF);
    orderedArray.OrderedInsert(-199, CF);
    orderedArray.OrderedInsert(253, CF);
    orderedArray.OrderedInsert(-4, CF);
    orderedArray.OrderedInsert(335, CF);
    orderedArray.OrderedInsert(85, CF);
    orderedArray.OrderedInsert(257, CF);
    int correct_order[] = { -199, -4, 5, 85, 253, 257, 335};
    for (int i = 0; i < orderedArray.GetSize(); i++) {
      palAssertBreak(orderedArray[i] == correct_order[i]);
      palAssertBreak(orderedArray[i] == out_of_order_array[i]);
    }
  }
  
  return true;
}

bool palPalHashMapCacheTest() {
  palHashMapCache<const char*, int> cache1(12);
  cache1.SetAllocator(g_DefaultHeapAllocator);
  cache1.Cache("January", 1);
  cache1.Cache("March", 3);
  cache1.Cache("May", 5);
  cache1.Cache("June", 6);
  cache1.Cache("July", 7);

  int index;
  
  index = cache1.GetIndex("February");
  palAssertBreak(cache1.IsValidIndex(index) == false);
  index = cache1.GetIndex("May");
  palAssertBreak(cache1.IsValidIndex(index) == true);

  {
    int value;
    index = cache1.GetIndex("January");
    value = *cache1.GetValueAtIndex(index);
    palAssertBreak(value == 1);

    index = cache1.GetIndex("March");
    value = *cache1.GetValueAtIndex(index);
    palAssertBreak(value == 3);

    index = cache1.GetIndex("May");
    value = *cache1.GetValueAtIndex(index);
    palAssertBreak(value == 5);

    index = cache1.GetIndex("June");
    value = *cache1.GetValueAtIndex(index);
    palAssertBreak(value == 6);

    index = cache1.GetIndex("July");
    value = *cache1.GetValueAtIndex(index);
    palAssertBreak(value == 7);
  }
  cache1.Clear();
  cache1.Reset();

  return true;
}

bool palHashMapTest3() {
  palHashMap<const char*, int> intMap;
  intMap.SetAllocator(g_DefaultHeapAllocator);
  const int data_set_size = 1*1024*1024;
  int* keys = (int*)g_StdProxyAllocator->Allocate(sizeof(int)*data_set_size);
  char** keys_as_strings = (char**)g_StdProxyAllocator->Allocate((sizeof(char*)*data_set_size));
  int* values = (int*)g_StdProxyAllocator->Allocate(sizeof(int)*data_set_size);

  palSeedRandom(3);
  for (int i = 0; i < data_set_size; i++) {
    keys[i] = palGenerateRandom();
    keys_as_strings[i] = palStringAllocatingPrintf("%d", keys[i]);
    values[i] = palGenerateRandom();
  }

  int insert_count = 0;
  palTimer timer;
  timer.Start();
  for (int i = 0; i < data_set_size; i++) {
    int index = intMap.FindIndex(keys_as_strings[i]);
    int map_size = intMap.GetSize();
    if (index == kPalHashNULL) {
      // insert into list
      intMap.Insert(keys_as_strings[i], values[i]);
      index = intMap.FindIndex(keys_as_strings[i]);
      palAssertBreak(index != kPalHashNULL);
      insert_count++;
      palAssertBreak(map_size+1 == intMap.GetSize());
    } else {
      // remove from list
      intMap.Remove(keys_as_strings[i]);
      palAssertBreak(map_size-1 == intMap.GetSize());
      insert_count--;
    }
  }
  timer.Stop();
  printf("Inserting/Deleting %d items takes %f\n", data_set_size, timer.GetDeltaSeconds());
  float mean_length = intMap.MeanLength();
  float sd = intMap.StandardDeviationLength();
  printf("mean_length = %f\nSD = %f\n", mean_length, sd);
  timer.Start();
  while (intMap.GetSize() > 0) {
    int size = intMap.GetSize();
    intMap.EraseRandomEntry();
    palAssertBreak(size-1 == intMap.GetSize());
  }
  timer.Stop();
  printf("Emptying map by removing 1 random item at a time takes %f seconds\n", timer.GetDeltaSeconds());

  printf("Insert count = %d\n", insert_count);

  g_StdProxyAllocator->Deallocate(keys);
  g_StdProxyAllocator->Deallocate(values);
  for (int i = 0; i < data_set_size; i++) {
    palStringAllocatingPrintfDeallocate(keys_as_strings[i]);
  }
  g_StdProxyAllocator->Deallocate(keys_as_strings);
  return true;
}

bool palHashMapTest2() {
  palHashMap<int, int> intMap;
  intMap.SetAllocator(g_DefaultHeapAllocator);
  const int data_set_size = 256*1024;
  int* keys = (int*)g_DefaultHeapAllocator->Allocate(sizeof(int)*data_set_size, 4);
  int* values = (int*)g_DefaultHeapAllocator->Allocate(sizeof(int)*data_set_size, 4);

  palSeedRandom(3);
  for (int i = 0; i < data_set_size; i++) {
    keys[i] = palGenerateRandom();
    values[i] = palGenerateRandom();
  }

  int insert_count = 0;
  palTimer timer;
  timer.Start();
  for (int i = 0; i < data_set_size; i++) {
    int index = intMap.FindIndex(keys[i]);
    int map_size = intMap.GetSize();
    if (index == kPalHashNULL) {
      // insert into list
      intMap.Insert(keys[i], values[i]);
      index = intMap.FindIndex(keys[i]);
      palAssertBreak(index != kPalHashNULL);
      int key_at_index = *intMap.GetKeyAtIndex(index);
      int value_at_index = *intMap.GetValueAtIndex(index);
      palAssertBreak(keys[i] == key_at_index);
      palAssertBreak(values[i] == value_at_index);
      insert_count++;
      palAssertBreak(map_size+1 == intMap.GetSize());
    } else {
      // remove from list
      int key_at_index = *intMap.GetKeyAtIndex(index);
      int value_at_index = *intMap.GetValueAtIndex(index);
      palAssertBreak(keys[i] == key_at_index);
      intMap.Remove(keys[i]);
      palAssertBreak(map_size-1 == intMap.GetSize());
      insert_count--;
    }    
  }

  g_DefaultHeapAllocator->Deallocate(keys);
  g_DefaultHeapAllocator->Deallocate(values);
  return true;
  timer.Stop();
  printf("Inserting/Deleting %d items takes %f\n", data_set_size, timer.GetDeltaSeconds());
  float mean_length = intMap.MeanLength();
  float sd = intMap.StandardDeviationLength();
  printf("mean_length = %f\nSD = %f\n", mean_length, sd);

  //intMap.dumpBucketLength("c:/temp/dump.txt");
  timer.Start();
  while (intMap.GetSize() > 0) {
    int size = intMap.GetSize();
    intMap.EraseRandomEntry();
    palAssertBreak(size-1 == intMap.GetSize());
  }
  timer.Stop();
  printf("Emptying map by removing 1 random item at a time takes %f seconds\n", timer.GetDeltaSeconds());

  printf("Insert count = %d\n", insert_count);

  g_DefaultHeapAllocator->Deallocate(keys);
  g_DefaultHeapAllocator->Deallocate(values);
  return true;
}

bool palHashMapTest() {
  palHashMap<int, int> intMap;
  intMap.SetAllocator(g_DefaultHeapAllocator);
  intMap.Insert(3, 99);
  intMap.Insert(4, 100);

  palHashMap<int, int> copiedIntMap(intMap, g_DefaultHeapAllocator);
  palHashMap<int, int> assignedIntMap;
  assignedIntMap.SetAllocator(g_DefaultHeapAllocator);
  palAssertBreak(assignedIntMap.GetSize() == 0);
  assignedIntMap = copiedIntMap;

  int* r = NULL;
  r = intMap.Find(3);
  palAssertBreak(*r == 99);
  r = intMap.Find(4);
  palAssertBreak(*r == 100);


  for (int i = 0; i < intMap.GetSize(); i++) {
    const int* key = intMap.GetKeyAtIndex(i);
    const int* value = intMap.GetValueAtIndex(i);

    if (*key == 3) {
      palAssertBreak(*value == 99);
    }

    if (*key == 4) {
      palAssertBreak(*value == 100);
    }
  }

  palAssertBreak(intMap.GetSize() == 2);
  intMap.Remove(3);
  palAssertBreak(intMap.GetSize() == 1);
  r = intMap.Find(3);
  palAssertBreak(r == NULL);
  intMap.Remove(4);
  r = intMap.Find(4);
  palAssertBreak(r == NULL);
  palAssertBreak(intMap.GetSize() == 0);

  r = copiedIntMap.Find(3);
  palAssertBreak(*r == 99);
  r = copiedIntMap.Find(4);
  palAssertBreak(*r == 100);


  for (int i = 0; i < copiedIntMap.GetSize(); i++) {
    const int* key = copiedIntMap.GetKeyAtIndex(i);
    const int* value = copiedIntMap.GetValueAtIndex(i);

    if (*key == 3) {
      palAssertBreak(*value == 99);
    }

    if (*key == 4) {
      palAssertBreak(*value == 100);
    }
  }

  copiedIntMap.Remove(3);
  r = copiedIntMap.Find(3);
  palAssertBreak(r == NULL);
  copiedIntMap.Remove(4);
  r = copiedIntMap.Find(4);
  palAssertBreak(r == NULL);


  r = assignedIntMap.Find(3);
  palAssertBreak(*r == 99);
  r = assignedIntMap.Find(4);
  palAssertBreak(*r == 100);


  for (int i = 0; i < assignedIntMap.GetSize(); i++) {
    const int* key = assignedIntMap.GetKeyAtIndex(i);
    const int* value = assignedIntMap.GetValueAtIndex(i);

    if (*key == 3) {
      palAssertBreak(*value == 99);
    }

    if (*key == 4) {
      palAssertBreak(*value == 100);
    }
  }

  assignedIntMap.Remove(3);
  r = assignedIntMap.Find(3);
  palAssertBreak(r == NULL);
  assignedIntMap.Remove(4);
  r = assignedIntMap.Find(4);
  palAssertBreak(r == NULL);

  return true;
}

bool palMinHeapTest() {
  palMinHeap<int> mh;
  mh.SetAllocator(g_DefaultHeapAllocator);
  mh.Insert(3);
  mh.Insert(999912);
  mh.Insert(66);
  mh.Insert(-3);
  mh.Insert(27);
  mh.Insert(0);
  mh.Insert(1);
  mh.Insert(1000000);

  int correct_order[] = {-3, 0, 1, 3, 27, 66, 999912, 1000000};
  int answer_index = 0;
  while (mh.IsEmpty() == false) {
    int answer = mh.FindMin();
    if (correct_order[answer_index] != answer) {
      palBreakHere();
    }
    mh.DeleteMin();
    answer_index++;
  }

  mh.Insert(3);
  mh.Insert(999912);
  mh.Insert(66);
  mh.Insert(-3);
  mh.Insert(27);
  mh.Insert(0);
  mh.Insert(1);
  mh.Insert(1000000);

  palMinHeap<int> mh2(mh, g_DefaultHeapAllocator);
  answer_index = 0;
  while (mh2.IsEmpty() == false) {
    int answer = mh2.FindMin();
    if (correct_order[answer_index] != answer) {
      palBreakHere();
    }
    mh2.DeleteMin();
    answer_index++;
  }
  palMinHeap<int> mh3;
  mh3.SetAllocator(g_DefaultHeapAllocator);
  mh3 = mh;

  answer_index = 0;
  while (mh3.IsEmpty() == false) {
    int answer = mh3.FindMin();
    if (correct_order[answer_index] != answer) {
      palBreakHere();
    }
    mh3.DeleteMin();
    answer_index++;
  }

  answer_index = 0;
  while (mh.IsEmpty() == false) {
    int answer = mh.FindMin();
    if (correct_order[answer_index] != answer) {
      palBreakHere();
    }
    mh.DeleteMin();
    answer_index++;
  }

  return true;
}

void dumpListForward (const palList<int>& l)
{
  palListNode<int>* current;

  if (l.IsEmpty())
    return;

  current = l.GetFirst();
  do 
  {
    printf("%d\n", current->data);
    if (l.IsLast(current))
      break;
    current = current->next;
  } while (true);
  printf("--\n");
}

void dumpListBackward (const palList<int>& l)
{
  palListNode<int>* current;

  if (l.IsEmpty())
    return;

  current = l.GetLast();
  do 
  {
    printf("%d\n", current->data);
    if (l.IsFirst(current))
      break;
    current = current->prev;
  } while (true);
  printf("--\n");
}

void dumpList (const palList<int>& l)
{
  dumpListForward(l);
  dumpListBackward(l);
}

int CompareIntNode(palListNode<int>* a, palListNode<int>* b) {
  if (a->data < b->data) {
    return -1;
  } else if (a->data == b->data) {
    return 0;
  } else if (a->data > b->data) {
    return 1;
  }
  return 0;
}

bool verifyListSort(const palList<int>& il) {
  palListNode<int>* node = il.GetFirst();
  palListNode<int>* next = node->next;
  while (il.IsLast(node) == false && il.IsLast(next) == false) {
    if (next->data < node->data) {
      return false;
    }
    node = next;
    next = next->next;
  }
  return true;
}

int64_t runListSortBenchmark(int benchmark_size) {
  palList<int> il;
  il.SetAllocator(g_DefaultHeapAllocator);
  for (int i = 0; i < benchmark_size; i++) {
    il.PushFront(palGenerateRandom());
  }
  
  palTimer t;
  t.Start();
  il.Sort(CompareIntNode);
  t.Stop();

  if (verifyListSort(il) == false) {
    dumpListForward(il);
  }
  
  return t.GetDeltaTime();
}

bool palListSortBenchmark() {
  int64_t dt;
  
  int benchmark_size = 1; 
  
  palSeedRandom(7);
  
  for (int i = 0; i < 17; i++) {
    dt = runListSortBenchmark(benchmark_size);
    printf("%d %lld\n", benchmark_size, dt);
    benchmark_size *= 2;
  }
  return true;
}

bool palListSortTest() {
  palList<int> il;
  il.SetAllocator(g_DefaultHeapAllocator);
  il.PushFront(4);
  il.PushFront(6);
  il.PushFront(99);
  il.PushFront(21);
  il.PushFront(5);
  il.PushFront(9);
  il.PushFront(13);
  
  //dumpListForward(il);
  il.Sort(CompareIntNode);

  //dumpListForward(il);
  return true;
}

/* intrusive list */
struct int_container
{
  int z;
  palIListNodeDeclare(int_container, list_node);
};

int int_container_compare(const struct int_container* a, const struct int_container* b) {
  if (a->z < b->z) {
    return -1;
  } else if (a->z > b->z) {
    return 1;
  } else {
    return 0;
  }
}

bool palIListSortTest() {
  int_container a,b,c,d,e,f,g;

  a.z = palGenerateRandom();
  b.z = palGenerateRandom();
  c.z = palGenerateRandom();
  d.z = palGenerateRandom();
  e.z = palGenerateRandom();
  f.z = palGenerateRandom();
  g.z = palGenerateRandom();


  palIList ilist_head;

  ilist_head.AddHead(&a.list_node);
  ilist_head.AddHead(&b.list_node);
  ilist_head.AddHead(&c.list_node);
  ilist_head.AddHead(&d.list_node);
  ilist_head.AddHead(&e.list_node);
  ilist_head.AddHead(&f.list_node);
  ilist_head.AddHead(&g.list_node);

  palIListSorterDeclare(int_container, list_node) sorter(&ilist_head);
  palIListForeachDeclare(int_container, list_node) fe(&ilist_head);

  printf("Unsorted\n");
  while (fe.Finished() == false)
  {
    int_container* list_entry = fe.GetListEntry();
    printf("%d\n", list_entry->z);
    fe.Next();
  }

  sorter.Sort(int_container_compare);

  fe.First();
  
  printf("Sorted\n");
  while (fe.Finished() == false)
  {
    int_container* list_entry = fe.GetListEntry();
    printf("%d\n", list_entry->z);
    fe.Next();
  }

  return true;
}

bool palListTest()
{
  palList<int> il;
  il.SetAllocator(g_DefaultHeapAllocator);
  dumpList(il);

  il.PushFront(4);
  dumpList(il);
  il.PushFront(5);
  dumpList(il);
  il.PushFront(6);
  dumpList(il);
  il.PushFront(9);
  dumpList(il);
  il.PushFront(13);
  dumpList(il);
  il.PopFront();
  dumpList(il);
  il.PopBack();
  dumpList(il);
  il.Remove(6);
  dumpList(il);

#if 0
  palList<int> il_spliced_head;
  il_spliced_head.SetAllocator(g_DefaultHeapAllocator);
  il_spliced_head.PushFront(9);
  il_spliced_head.PushBack(21);
  palList<int> il_spliced_tail;
  il_spliced_tail.SetAllocator(g_DefaultHeapAllocator);
  il_spliced_tail.PushFront(99);
  il_spliced_tail.PushBack(13);

  printf("head:\n");
  dumpList(il_spliced_head);
  printf("tail:\n");
  dumpList(il_spliced_tail);

  il.SpliceHead(&il_spliced_head);
  dumpList(il);
  il.SpliceTail(&il_spliced_tail);
  dumpList(il);
#endif
#if 0
  palList<int> sublist;

  il.MakeSublist(&sublist, p_21, p_99);

  printf("original:\n");
  dumpList(il);
  printf("sublist:\n");
  dumpList(sublist);
#endif
  return true;
}


bool palIListTest()
{
	int_container i1,i2,i3,i4;

	i1.z = 1;
	i2.z = 2;
	i3.z = 3;
	i4.z = 4;

	palIList ilist_head;

	ilist_head.AddHead(&i1.list_node);
	ilist_head.AddHead(&i2.list_node);
	ilist_head.AddHead(&i3.list_node);
	ilist_head.AddHead(&i4.list_node);

	palIListForeachDeclare(int_container, list_node) fe(&ilist_head);

	while (fe.Finished() == false)
	{
		int_container* list_entry = fe.GetListEntry();
		printf("%d\n", list_entry->z);
		fe.Next();
	}

	return true;
}

bool PalContainerTest() {
  return true;
  //palHashMapTest2();
  
  palArrayTest();
  palHashMapTest();
  palListTest();
  palListSortTest();
  palIListTest();
  palListSortBenchmark();
  palIListSortTest();
  palMinHeapTest();
  palPalHashMapCacheTest();
  
  //palHashMapTest3();
  return true;
}