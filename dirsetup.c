#include "gltron.h"

void dirSetup(const char *executable) {
#ifdef LOCAL_DATA
	goto_installpath(executable);
#endif
  initDirectories();
}
