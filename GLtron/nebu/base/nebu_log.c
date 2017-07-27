#include "base/nebu_debug_memory.h"

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>

FILE* nebu_debug = NULL;

void nebu_Log(const char* format, ...)
{
	char buf[4096];
	va_list ap;
	va_start(ap, format);
	vsprintf(buf, format, ap);
	va_end(ap);
	OutputDebugStringA(buf);

	if (!nebu_debug)
	{
		nebu_debug = fopen("nebu_debug.txt", "w");
	}
	if(nebu_debug)
		fprintf(nebu_debug, buf);
}


