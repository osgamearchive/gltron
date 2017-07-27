#include "base/nebu_debug_memory.h"

#include "filesystem/path.h"
#include "filesystem/dirsetup.h"
#include "game/engine.h"
#include "game/init.h"
#include "game/gltron.h"
#include "game/game.h"
#include "game/resource.h"
#include "base/nebu_resource.h"
#include "input/input.h"
#include "base/util.h"
#include "scripting/scripting.h"
#include "base/nebu_system.h"
#include "audio/audio.h"
#include "audio/nebu_audio_system.h"
#include "configuration/settings.h"
#include "configuration/configuration.h"
#include "video/nebu_video_system.h"
#include "filesystem/nebu_filesystem.h"
#include "scripting/nebu_scripting.h"
#include "input/nebu_input_system.h"
#include "video/video.h"

#include <stdlib.h>

#include "base/nebu_assert.h"

void initFilesystem(int argc, const char *argv[]);
void initGUIs(void);

void exitSubsystems(void)
{
	Sound_shutdown();

	freeVideoData();

	if(gWorld)
		video_FreeLevel(gWorld);
	gWorld = NULL;

	if(game)
		game_FreeGame(game);
	game = NULL;

	if(game2)
		game_FreeGame2(game2);
	game2 = NULL;

	scripting_Quit();
	nebu_FS_ClearAllPaths();
	resource_FreeAll();
	resource_Shutdown();
}

void initSubsystems(int argc, const char *argv[]) {
	nebu_Init();

	resource_Init();

	initFilesystem(argc, argv);
	initScripting();

	initConfiguration(argc, argv);
	initArtpacks(); // stores the artpack directory names in a lua table, so we can display it in the menu later on

	initGUIs();
	initVideo();
	initAudio();
	initInput();

	nebu_Log("[status] done loading level...\n");
}

void initFilesystem(int argc, const char *argv[]) {
	dirSetup(argv[0]);
	// argc is not 1 for Cocoa apps
	// nebu_assert(argc == 1);
}

void initScripting(void) {
	// scripting_Init(NEBU_SCRIPTING_DEBUG);
	scripting_Init(0);
	init_c_interface();

  /* load basic scripting services */
	runScript(PATH_SCRIPTS, "basics.lua");
	runScript(PATH_SCRIPTS, "joystick.lua");
	runScript(PATH_SCRIPTS, "path.lua");

	runScript(PATH_SCRIPTS, "video.lua");

	runScript(PATH_SCRIPTS, "console.lua");
}

void initConfiguration(int argc, const char *argv[])
{
	/* load some more defaults from config file */
	runScript(PATH_SCRIPTS, "config.lua");
	runScript(PATH_SCRIPTS, "artpack.lua");
	
#ifndef __IPHONEOS__
	/* go for .gltronrc (or whatever is defined in RC_NAME) */
	{
		char *path;
		path = getPossiblePath(PATH_PREFERENCES, RC_NAME);
		if (path != NULL) {
		if (nebu_FS_Test(path)) {
			nebu_Log("[status] loading settings from %s\n", path);
			scripting_RunFile(path);
		} else {
			nebu_Log("[error] cannot load config file %s from %s\n", RC_NAME, path);
		}
			free(path);
		}
		else {
			nebu_Log("[fatal] can't get valid pref path for %s\n", RC_NAME);
			nebu_assert(0); exit(1); // something is seriously wrong
		}
	}
	
	// check if the config file is from the same version
	// if not, override using defaults
	{
		float ini_version = 0, app_version;
		if(isSetting("version"))
			ini_version = getSettingf("version");
		scripting_GetGlobal("app_version", NULL);
		scripting_GetFloatResult(&app_version);
		if(ini_version < app_version)
		{
			/* load some more defaults from config file */
			runScript(PATH_SCRIPTS, "config.lua");
			runScript(PATH_SCRIPTS, "artpack.lua");
			nebu_Log("[warning] old config file version %f found, app version is %f, overriding using defaults\n",
				ini_version, app_version);
			setSettingf("version", app_version);
		}
	}
	// check if config is valid
	{
		int isValid = 1;
		scripting_GetGlobal("save_completed", NULL);
		if(scripting_IsNil()) {
            nebu_Log("[warning] defunct config file (save_completed missing)\n");
			isValid = 0;
		}
		scripting_Pop();
		scripting_GetGlobal("settings", "keys", NULL);
		if(!scripting_IsTable())
		{
            nebu_Log("[warning] defunct config file (keys table missing)\n");
			isValid = 0;
		}
		scripting_Pop();
		if(!isValid)
		{
			nebu_Log("[warning] defunct config file found, overriding using defaults\n");
			runScript(PATH_SCRIPTS, "config.lua");
			runScript(PATH_SCRIPTS, "artpack.lua");

		}
	}
#endif

	/* parse any comandline switches overrinding the loaded settings */
	// TODO: fix for mac version
    // parse_args(argc, argv);

	/* sanity check some settings */
	checkSettings();

	scripting_Run("setupArtpackPaths()");
	scripting_Run("setupLevels()");
		
	/* intialize the settings cache, remember to do that everytime you
	   change something */
	updateSettingsCache();
}

void initVideo(void) {
	nebu_Video_Init();
	// this requuires the player data
    gScreen = (Visual*) malloc(sizeof(Visual));

	setupDisplay();
    initVideoData();

	loadArt();
	loadModels();
}

void initAudio(void) {
	nebu_Audio_Init();
	runScript(PATH_SCRIPTS, "audio.lua");
	/* probe for artpacks & songs */
	Sound_initTracks();
	Sound_setup();
}
	
void initGUIs(void)
{
	// menu
	runScript(PATH_SCRIPTS, "menu_functions.lua");
	runScript(PATH_SCRIPTS, "menu.lua");

	// hud stuff
	runScript(PATH_SCRIPTS, "hud-config.lua");
	runScript(PATH_SCRIPTS, "hud.lua");
	runScript(PATH_SCRIPTS, "gauge.lua");
}

void initInput(void) {
	nebu_Input_Init();

	gInput.mouse1 = 0;
	gInput.mouse2 = 0;
}
