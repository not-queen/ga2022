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
#include <sys/types.h>


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct duration_event_t {
	char* name;
	heap_t* heap;
	DWORD pid;
	DWORD tid;
	uint64_t time_start;
	uint64_t time_end;
} duration_event_t;

typedef struct trace_t {
	heap_t* heap;
	char* buffer;
	char* path;
	int capacity;
	uint64_t num;
	bool is_cap;
} trace_t;

trace_t* trace_create(heap_t* heap, int event_capacity)
{
	trace_t* trace = heap_alloc(heap, sizeof(trace_t), 8);
	trace->heap = heap;
	trace->capacity = event_capacity;
	trace->is_cap = false;
	trace->num = 0;
	return trace;
}

void trace_destroy(trace_t* trace)
{
	//heap_free(trace->path, trace);
	//heap_free(trace->buffer, trace);
	//heap_free(trace->heap, trace);
	return;
}

void trace_duration_push(trace_t* trace, const char* name)
{
	if (trace->is_cap) {
		uint64_t tmp_num = atomic_increment(&(trace->num));
		if (tmp_num > trace->capacity) {
			printf("trace_duration_push failed due to exceed the capacity.\n");
			return;
		} 

		heap_t* heap = heap_create(4096);
		duration_event_t* event_duration = heap_alloc(heap, sizeof(duration_event_t), 8);
		event_duration->heap = heap;
		event_duration->name = heap_alloc(heap, strlen(name) + 1, 8);
		strncpy_s(event_duration->name, strlen(name) + 1, name, strlen(name));
		event_duration->tid = GetCurrentThreadId();
		event_duration->pid = GetCurrentProcessId();
		event_duration->time_start = timer_ticks_to_us(timer_get_ticks());

		char tmp[4096];
		sprintf_s(tmp, 4096, "\t\t{ \"name\":\"%s\", \"ph\" : \"B\", \"pid\" : %lu, \"tid\" : \"%lu\", \"ts\" : \"%lu\" },\n", event_duration->name, event_duration->pid, event_duration->tid, event_duration->time_start);
		strncat_s(trace->buffer, strlen(trace->buffer) + strlen(tmp) + 1, tmp, strlen(tmp));
	}
	return;
}

void trace_duration_pop(trace_t* trace, const char* name)
{
	if (trace->is_cap) {
		if (trace->num <= 0) {
			printf("trace_duration_pop failed due to noting in the queue.\n");
			return;
		}

		heap_t* heap = heap_create(4096);
		duration_event_t* event_duration = heap_alloc(heap, sizeof(duration_event_t), 8);
		event_duration->heap = heap;
		event_duration->name = heap_alloc(heap, strlen(name) + 1, 8);
		strncpy_s(event_duration->name, strlen(name) + 1, name, strlen(name));
		event_duration->tid = GetCurrentThreadId();
		event_duration->pid = GetCurrentProcessId();
		event_duration->time_end = timer_ticks_to_us(timer_get_ticks());

		char tmp[4096];
		sprintf_s(tmp, 4096, "\t\t{ \"name\":\"%s\", \"ph\" : \"E\", \"pid\" : %lu, \"tid\" : \"%lu\", \"ts\" : \"%lu\" },\n", event_duration->name, event_duration->pid, event_duration->tid, event_duration->time_end);
		strncat_s(trace->buffer, strlen(trace->buffer) + strlen(tmp) + 1, tmp, strlen(tmp));
	}
	return;
}

void trace_capture_start(trace_t* trace, const char* path)
{
	trace->path = heap_alloc(trace->heap, strlen(path) + 1, 8);
	strncpy_s(trace->path, strlen(path)+1, path, strlen(path));
	
	char* tmp = "{\n\t\"displayTimeUnit\": \"ns\", \"traceEvents\": [\n\0";
	strncat_s(trace->buffer, strlen(trace->buffer) + strlen(tmp) + 1, tmp, strlen(tmp));

	trace->is_cap = true;
}

void trace_capture_stop(trace_t* trace)
{
	trace->is_cap = false;
	char* tmp = "\t\n}\0";
	strncat_s(trace->buffer, strlen(trace->buffer) + strlen(tmp) + 1, tmp, strlen(tmp));

	heap_t* heap = heap_create(4096);
	fs_t* fs = fs_create(heap, trace->capacity);
	fs_work_t* fs_work = fs_write(fs, trace->path, trace->buffer, strlen(trace->buffer), false);
	fs_work_wait(fs_work);
}
