#include "filesystem/path.h"
#include "Nebu_scripting.h"

#include <stdlib.h>

void runScript(int ePath, const char *name) {
        char *s;
        s = getPath(ePath, name);
        scripting_RunFile(s);
        free(s);
}

