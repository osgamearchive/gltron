/*
  gltron
  Copyright (C) 1999 by Andreas Umbach <marvin@dataway.ch>
*/
#include "base/nebu_system.h" // includes main
#include "game/init.h"
#include "filesystem/path.h"
#include "base/util.h"

int main(int argc, char *argv[] ) {
	initSubsystems(argc, argv);
	runScript(PATH_SCRIPTS, "main.lua");
  return 0;
}







