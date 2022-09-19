#include "heap.h"

#include <stdio.h>

#include <windows.h>

static void homework1_test();

int main(int argc, const char* argv[])
{
	homework1_test();
	heap_t* heap = heap_create(2 * 1024 * 1024);

	heap_destroy(heap);
	return 0;
}

static void* homework1_allocate_1(heap_t* heap)
{
	return heap_alloc(heap, 16 * 1024, 8);
}

static void* homework1_allocate_2(heap_t* heap)
{
	return heap_alloc(heap, 256, 8);
}

static void* homework1_allocate_3(heap_t* heap)
{
	return heap_alloc(heap, 32 * 1024, 8);
}

static void homework1_test()
{
	heap_t* heap1 = heap_create(4096);
	/*leaked*/ void* leak = heap_alloc(heap1, 100, 8);
	void* not_leak = heap_alloc(heap1, 200, 8);
	heap_free(heap1, not_leak);
	heap_destroy(heap1);

	heap_t* heap2 = heap_create(4096);
	void* block1 = homework1_allocate_1(heap2);
	/*leaked*/ homework1_allocate_2(heap2);
	/*leaked*/ homework1_allocate_3(heap2);
	heap_free(heap2, block1);
	heap_destroy(heap2);
}