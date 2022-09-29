#include "atomic.h"
#include "debug.h"
#include "fs.h"
#include "heap.h"
#include "mutex.h"
#include "thread.h"
#include "wm.h"

#include <assert.h>
#include <stdio.h>

#include <windows.h>

static void homework1_test();
static void homework2_test();

int main(int argc, const char* argv[])
{
	debug_install_exception_handler();
	homework2_test();

	return 0;
}

static void homework2_test_internal(heap_t* heap, fs_t* fs, bool use_compression)
{
	const char* write_data = "Hello world!";
	fs_work_t* write_work = fs_write(fs, "foo.bar", write_data, strlen(write_data), use_compression);

	fs_work_wait(write_work);

	fs_work_t* read_work = fs_read(fs, "foo.bar", heap, true, use_compression);

	assert(fs_work_get_result(write_work) == 0);
	assert(fs_work_get_size(write_work) == 12);

	char* read_data = fs_work_get_buffer(read_work);
	assert(read_data && strcmp(read_data, "Hello world!") == 0);
	assert(fs_work_get_result(read_work) == 0);
	assert(fs_work_get_size(read_work) == 12);

	fs_work_destroy(read_work);
	fs_work_destroy(write_work);

	heap_free(heap, read_data);
}

static void homework2_test()
{
	heap_t* heap = heap_create(4096);
	fs_t* fs = fs_create(heap, 16);

	const bool disable_compression = false;
	homework2_test_internal(heap, fs, disable_compression);

	// HOMEWORK 2: Set enable_compression to true when implemented!
	const bool enable_compression = false;
	homework2_test_internal(heap, fs, enable_compression);

	fs_destroy(fs);
	heap_destroy(heap);
}
