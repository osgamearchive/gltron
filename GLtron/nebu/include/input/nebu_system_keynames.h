#ifndef NEBU_SYSTEM_KEYNAMES_H
#define NEBU_SYSTEM_KEYNAMES_H

#define CUSTOM_KEY_COUNT 4 * 24

typedef struct {
	int key;
	char *name;
} nebu_keyname;

typedef struct {
	nebu_keyname key[CUSTOM_KEY_COUNT];
} nebu_custom_keynames;

extern nebu_custom_keynames custom_keys;

#endif
