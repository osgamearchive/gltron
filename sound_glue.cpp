#include "Sound.h"

#include "SoundSystem.h"
#include "SourceMusic.h"
#include "SourceCopy.h"
#include "Source3D.h"

#include "gltron.h"

Sound::System *sound = NULL;
Sound::SourceMusic *music = NULL;
Sound::SourceSample *crash = NULL;
Sound::SourceSample *engine = NULL;

Sound::Source3D *players[PLAYERS];

extern "C" {

#include "sound_glue.h"

  void Audio_EnableEngine(void) {
    engine->Start();
    printf("turning on engine sound\n");
  }

  void Audio_DisableEngine(void) {
    engine->Stop();
    printf("turning off engine sound\n");
  }

  void Audio_Idle(void) { 
#if 1
    // iterate over all the players and update the engines
    if(engine->IsPlaying()) {
#define V 5
      for(int i = 0; i < PLAYERS; i++) {
	Player *p;
	Sound::Source3D *p3d;
	p3d = players[i];
	p = game->player + i;
	p3d->_location = Vector3(p->data->posx, p->data->posy, 0);
	p3d->_velocity = Vector3(V * dirsX[p->data->dir],
				 V * dirsY[p->data->dir],
				 0);
      }
      Sound::Listener& listener = sound->GetListener();
      listener._location = players[0]->_location;
      listener._velocity = players[0]->_velocity;
      listener._direction = players[0]->_velocity;

#undef V
    }
#endif
    sound->Idle();

  }

  void Audio_CrashPlayer(int player) {
    Sound::SourceCopy *copy = new Sound::SourceCopy(crash);
    copy->Start();
    copy->SetRemovable();
    sound->AddSource(copy);
    // fprintf(stderr, "kaboom!\n");
  }

  void Audio_Init() {
    Sound_Init();

    SDL_AudioSpec* spec = new SDL_AudioSpec;
    spec->freq = 22050;
    spec->format = AUDIO_S16;
    spec->channels = 2;
    spec->samples = 1024;

    sound = new Sound::System(spec);

    spec->userdata = sound;
    spec->callback = sound->GetCallback();

    if(SDL_OpenAudio( spec, NULL ) != 0) {
      printf("%s\n", SDL_GetError());
      exit(1);
    }

    SDL_PauseAudio(0);

  }

  void Audio_Quit() {
    Sound_Quit();
    SDL_CloseAudio();
  }

  void Audio_LoadMusic(char *name) {
    if(music != NULL) {
      music->Stop();
      music->SetRemovable();
    }
    music = new Sound::SourceMusic(sound);
    music->Load(name);
    music->SetLoop(255);
    sound->AddSource(music);
  }

  void Audio_PlayMusic() {
    music->Start();
  }

  void Audio_StopMusic() {
    music->Stop();
  }

  void Audio_SetMusicVolume(float volume) {
    music->SetVolume(volume);
  }
  
  void Audio_SetFxVolume(float volume) {
    engine->SetVolume(volume);
    crash->SetVolume(volume);
  }

  void Audio_LoadPlayers() {
#if 1
    for(int i = 0; i < PLAYERS; i++) {
      players[i] = new Sound::Source3D(sound, engine);
      players[i]->Start();
      if(i != 0)
	sound->AddSource(players[i]);
    }
#endif
  }
  void Audio_LoadSample(char *name, int number) {
    switch(number) {
    case 0:
      engine = new Sound::SourceSample(sound);
      engine->Load(name);
      break;
    case 1:
      crash = new Sound::SourceSample(sound);
      crash->Load(name);
      Audio_LoadPlayers();
      break;
    }
  }

}




