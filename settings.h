#ifndef SETTINGS_H
#define SETTINGS_H

/* 
 * SettingsCache - A Structure for storing lua settings during play to 
 * avoid the overhead of fetching from lua repeatedly. Values should be 
 * considered and const as they are never written back out to lua.
 */
typedef struct SettingsCache {
  int use_stencil;
  int show_scores;
  int show_ai_status;
  int show_fps;
  int softwareRendering;
  int show_floor_texture;
  int line_spacing;
  int antialias_lines;
  int show_model; 
  int show_decals;
  int alpha_trails;
  int shadow_lod;
  int turn_cycle;
  int light_cycles;
  int lod;
  int fov;
  int stretch_textures;
  int show_skybox;
  int show_recognizer;
  int show_impact;
  int show_glow;
  int show_crash_texture;
  int show_wall;
  int fast_finish;
  int playMusic;
  int playEffects;
  int camType;
  float znear;
  float clear_color[4];
} SettingsCache;


extern float getSettingf(char *name);
extern int getSettingi(char *name);

extern void setSettingf(char *name, float f);
extern void setSettingi(char *name, int i);

#endif
