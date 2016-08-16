#include "base/nebu_debug_memory.h"

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>

void nebu_Log(const char* format, ...)
{
	char buf[4096];
	va_list ap;
	va_start(ap, format);
	vsprintf(buf, format, ap);
	va_end(ap);
	OutputDebugStringA(buf);
}


