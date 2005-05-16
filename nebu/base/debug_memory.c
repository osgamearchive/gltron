#include "base/nebu_debug_memory.h"

void nebu_debug_memory_CheckLeaksOnExit(void)
{
#ifdef _DEBUG
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
}
