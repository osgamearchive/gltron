#ifndef SYSTEM_KEYNAMES_H
#define SYSTEM_KEYNAMES_H

#define CUSTOM_KEY_COUNT 2 * 24

typedef struct {
	int key;
	char *name;
} keyname;

typedef struct {
	keyname key[CUSTOM_KEY_COUNT];
} custom_keynames;

extern custom_keynames custom_keys;

#endif
