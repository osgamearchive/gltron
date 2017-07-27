#include "filesystem/path.h"
#include "Nebu_scripting.h"

#include <stdio.h>

#include "base/nebu_debug_memory.h"

void runScript(int ePath, const char *name) {
    char *s;
    s = getPath(ePath, name);
	// nebu_Log("[script] start running script '%s' from '%s'\n", name, s);
    scripting_RunFile(s);
	// nebu_Log("[script] finished running script '%s'\n", name);
    free(s);
}

