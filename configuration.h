#ifndef CONFIGURATION_H
#define CONFIGURATION_H

extern void parse_args(int argc, const char *argv[]);
extern void updateSettingsCache();
extern void initMainGameSettings();
extern void initDefaultSettings();
extern void initColors();
extern void checkSettings();
extern void saveSettings();

#include "settings.h"

#endif
