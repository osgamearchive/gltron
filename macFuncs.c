#ifdef macintosh

/* we want GLTron to run on locked volumes (CD-ROMs etc),
   so we must save settings to the prefs folder */

#include <Folders.h>
#include "FullPath.h" /* MoreFiles Library */
#include <stdlib.h>
#include <string.h>
#include "SDL.h"

void setupHomeEnvironment () {

    short  volume_ref_number;
    long   directory_id;
    short  len;
    Handle path;

    if ( noErr != FindFolder ( kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder,
                               &volume_ref_number, &directory_id) )
        exit (-1);

    

    if ( noErr == GetFullPath (volume_ref_number, directory_id, "\p", &len, &path) ) {

        char *homeEnv = (char*) malloc ( sizeof(*homeEnv) * len + 6);
        strcpy (homeEnv, "HOME=");
        strncat (homeEnv, *path, len-1); /* we want to remove the terminating ':' */        
        SDL_putenv (homeEnv);        
        free (homeEnv);
    }
}
#endif
