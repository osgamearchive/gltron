/*
  gltron
  Copyright (C) 1999 by Andreas Umbach <marvin@dataway.ch>
*/

#include "init.h"
#include "util.h"
#include "file.h"

int main(int argc, const char *argv[] ) {
	initSubsystems(argc, argv);
	runScript(PATH_SCRIPTS, "main.lua");
  return 0;
}







