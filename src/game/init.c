#include "filesystem/path.h"
#include "filesystem/dirsetup.h"
#include "game/init.h"
#include "game/gltron.h"
#include "game/game.h"
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

#include <assert.h>

#define INI_VERSION 0.7110f

void initFilesystem(int argc, const char *argv[]);

void initSubsystems(int argc, const char *argv[]) {
	nebu_Init();

	initFilesystem(argc, argv);
	scripting_Init();
	init_c_interface();
	initScripting();

	initConfiguration(argc, argv);
	
	loadLevel();

	initGame();
	initVideo();
	initAudio();
	initInput();
}

void initFilesystem(int argc, const char *argv[]) {
	dirSetup(argv[0]);
	assert(argc == 1);
}

void initScripting(void) {
  /* load basic scripting services */
	runScript(PATH_SCRIPTS, "basics.lua");
	runScript(PATH_SCRIPTS, "joystick.lua");
	runScript(PATH_SCRIPTS, "path.lua");

	runScript(PATH_SCRIPTS, "video.lua");

	/* not so basic scripting stuff */
	runScript(PATH_SCRIPTS, "console.lua");
	runScript(PATH_SCRIPTS, "hud-config.lua");
	runScript(PATH_SCRIPTS, "hud.lua");
	runScript(PATH_SCRIPTS, "gauge.lua");
}

void initConfiguration(int argc, const char *argv[])
{
	/* load some more defaults from config file */
	runScript(PATH_SCRIPTS, "config.lua");
	runScript(PATH_SCRIPTS, "artpack.lua");
	
	/* go for .gltronrc (or whatever is defined in RC_NAME) */
	{
		char *path;
		path = getPossiblePath(PATH_PREFERENCES, RC_NAME);
		if (path != NULL) {
		if (nebu_FS_Test(path)) {
			printf("[status] loading settings from %s\n", path);
			scripting_RunFile(path);
		} else {
			printf("[error] cannot load %s from %s\n", RC_NAME, path);
		}
			free(path);
		}
		else {
			printf("[fatal] can't get valid pref path for %s\n", RC_NAME);
			exit(1); // something is seriously wrong
		}
	}
	
	if(!isSetting("version") || getSettingf("version") < INI_VERSION) {
		/* load some more defaults from config file */
		runScript(PATH_SCRIPTS, "config.lua");
		runScript(PATH_SCRIPTS, "artpack.lua");
		printf("[warning] old config file found, overriding using defaults\n");
	}
	// check if config is valid
	scripting_GetGlobal("save_completed", NULL);
	if(scripting_IsNilResult()) {
		runScript(PATH_SCRIPTS, "config.lua");
		runScript(PATH_SCRIPTS, "artpack.lua");
		printf("[warning] defunct config file found, overriding using defaults\n");
	}
		
	setSettingf("version", INI_VERSION);

	/* parse any comandline switches overrinding the loaded settings */
	parse_args(argc, argv);

	/* sanity check some settings */
	checkSettings();

	scripting_Run("setupArtpacks()");
	scripting_Run("setupLevels()");
		
	/* intialize the settings cache, remember to do that everytime you
	   change something */
	updateSettingsCache();
}

void initVideo(void) {
	nebu_Video_Init();

	initVideoData();
	initArtpacks();

	runScript(PATH_SCRIPTS, "menu.lua");
	runScript(PATH_SCRIPTS, "menu_functions.lua");
	setupDisplay(gScreen);
}

void initAudio(void) {
	nebu_Audio_Init();
	runScript(PATH_SCRIPTS, "audio.lua");
	/* probe for artpacks & songs */
	Sound_initTracks();
	Sound_setup();
}
	
void initGame(void) {
	/* initialize the rest of the game's datastructures */
	initGameStructures();
	initGameLevel();
	resetScores();
}

void initInput(void) {
	nebu_Input_Init();

	gInput.mouse1 = 0;
	gInput.mouse2 = 0;
}
