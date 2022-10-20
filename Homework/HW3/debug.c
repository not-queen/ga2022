#include "debug.h"

#include <stdarg.h>
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <DbgHelp.h>

static uint32_t s_mask = 0xffffffff;

void debug_set_print_mask(uint32_t mask)
{
	s_mask = mask;
}

void debug_print(uint32_t type, _Printf_format_string_ const char* format, ...)
{
	if ((s_mask & type) == 0)
	{
		return;
	}

	va_list args;
	va_start(args, format);
	char buffer[256];
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	OutputDebugStringA(buffer);

	DWORD bytes = (DWORD)strlen(buffer);
	DWORD written = 0;
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsoleA(out, buffer, bytes, &written, NULL);
}

int debug_backtrace(void** stack, int stack_capacity)
{
	return CaptureStackBackTrace(1, stack_capacity, stack, NULL);
}
