#ifndef CONFIGURATION_H
#define CONFIGURATION_H

extern void parse_args(int argc, const char *argv[]);
extern void updateSettingsCache(void);
extern void initColors(void);
extern void checkSettings(void);
extern void saveSettings(void);

#include "settings.h"

#endif
