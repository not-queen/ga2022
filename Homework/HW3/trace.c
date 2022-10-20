#include "trace.h"
#include "fs.h"
#include "timer.h"
#include "heap.h"

#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct duration_event_t {
	char* name;
	DWORD tid;
	uint64_t time_start;
	uint64_t time_end;
} duration_event_t;

typedef struct trace_t {
	heap_t* heap;
	char* buffer;
	char* path;
	int capacity;
	bool is_cap;

} trace_t;

trace_t* trace_create(heap_t* heap, int event_capacity)
{
	trace_t* trace = heap_alloc(heap, sizeof(trace_t), 8);
	trace->heap = heap;
	trace->capacity = event_capacity;
	trace->is_cap = false;
	
	return trace;
}

void trace_destroy(trace_t* trace)
{
	free(trace->buffer);
	heap_free(trace->heap, trace);
}

void trace_duration_push(trace_t* trace, const char* name)
{
	if (trace->is_cap) {

	}
}

void trace_duration_pop(trace_t* trace)
{

}

void trace_capture_start(trace_t* trace, const char* path)
{

}

void trace_capture_stop(trace_t* trace)
{

}
