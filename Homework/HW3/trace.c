#include "trace.h"
#include "fs.h"
#include "timer.h"
#include "heap.h"
#include "atomic.h"
#include "debug.h"

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
	fs_t* fs;
	int capacity;
	bool is_cap;

} trace_t;

trace_t* trace_create(heap_t* heap, int event_capacity)
{
	trace_t* trace = heap_alloc(heap, sizeof(trace_t), 8);
	trace->heap = heap;
	trace->capacity = event_capacity;
	trace->is_cap = false;
	
	fs_t* fs = fs_create(trace->heap, event_capacity);
	trace->fs = fs;
	return trace;
}

void trace_destroy(trace_t* trace)
{
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
	trace->path = heap_alloc(trace->heap, strlen(path) + 1, 8);
	strcpy(trace->path, path);
	char* tmp = "\n\t\"displayTimeUnit\": \"ns\", \"traceEvents\": [\n\0";
	strncat(trace->buffer, tmp, strlen(tmp));
	trace->is_cap = true;
}

void trace_capture_stop(trace_t* trace)
{
	trace->is_cap = false;
	char* tmp = "\t\n\0";
	strncat(trace->buffer, tmp, strlen(tmp));
	fs_write(trace->fs, trace->path, trace->buffer, strlen(trace->buffer), false);
}
