#include "SoundSystem.h"
#include "SourceMusic.h"
#include "SourceCopy.h"
#include "Source3D.h"
#include "SourceEngine.h"
#include "Source.h"

#include "gltron.h"

static Sound::System *sound = NULL;
static Sound::SourceMusic *music = NULL;
static Sound::SourceSample *sample_crash = NULL;
static Sound::SourceSample *sample_engine = NULL;
static Sound::SourceSample *sample_recognizer = NULL;

static Sound::Source3D *players[PLAYERS];
static Sound::Source3D *recognizerEngine;

#define V 5.0f
#define TURNLENGTH 250.0f

extern "C" {

#include "sound_glue.h"

  void Audio_EnableEngine(void) {
    sample_engine->Start();
    sample_recognizer->Start();
    printf("turning on engine sound\n");
  }

  void Audio_DisableEngine(void) {
    sample_engine->Stop();
    sample_recognizer->Stop();
    printf("turning off engine sound\n");
  }

  void Audio_Idle(void) { 
    // iterate over all the players and update the engines
    if(sample_engine->IsPlaying()) {
      for(int i = 0; i < PLAYERS; i++) {
	int dt = game2->time.current - game->player[i].data->turn_time;
      
	Player *p;
	Sound::Source3D *p3d;
	p3d = players[i];
	p = game->player + i;
	p3d->_location = Vector3(p->data->posx, p->data->posy, 0);
	if(dt < TURN_LENGTH) {
	  float t = (float)dt / TURNLENGTH;

	  float vx = (1 - t) * dirsX[p->data->last_dir] +
	    t * dirsX[p->data->dir];
	  float vy = (1 - t) * dirsY[p->data->last_dir] +
	    t * dirsY[p->data->dir];
	  p3d->_velocity = Vector3(V * vx, V * vy, 0);
	} else {
	  p3d->_velocity = Vector3(V * dirsX[p->data->dir], 
				   V * dirsY[p->data->dir], 
				   0);
	}

	if(i == 0) {
	  if( dt < TURNLENGTH ) {
	    float t = (float)dt / TURNLENGTH;
	    float speedShift = ( 1 - t ) * 0.4 + t * 0.3;
	    float pitchShift = ( 1 - t ) * 0.9 + t * 1.0;
	    ( (Sound::SourceEngine*) p3d )->_speedShift = speedShift;
	    ( (Sound::SourceEngine*) p3d )->_pitchShift = pitchShift;
	  } else {
	  ( (Sound::SourceEngine*) p3d )->_speedShift = 0.3;
	  ( (Sound::SourceEngine*) p3d )->_pitchShift = 1.0;
	  }
	}
      }
    }

    if(sample_recognizer->IsPlaying()) {
      if (game2->settingsCache.show_recognizer) {
	sample_recognizer->UnPause();
	Point p, v;
	getRecognizerPositionVelocity(&p, &v);
	// recognizerEngine->_location = Vector3(p.x, p.y, RECOGNIZER_HEIGHT);
	recognizerEngine->_location = Vector3(p.x, p.y, 10.0f);
	recognizerEngine->_velocity = Vector3(v.x, v.y, 0);
      } else {
	sample_recognizer->Pause();
      }
    }

    Sound::Listener& listener = sound->GetListener();

    listener._location = Vector3(game->player[0].camera->cam);
    listener._direction = 
      Vector3(game->player[0].camera->target) -
      Vector3(game->player[0].camera->cam);
    // listener._location = players[0]->_location;
    // listener._direction = players[0]->_velocity;
    listener._velocity = players[0]->_velocity;

    listener._up = Vector3(0, 0, 1);


    sound->SetMixMusic(game2->settingsCache.playMusic);
    sound->SetMixFX(game2->settingsCache.playEffects);
    sound->Idle();
  }

  void Audio_CrashPlayer(int player) {
    Sound::SourceCopy *copy = new Sound::SourceCopy(sample_crash);
    copy->Start();
    copy->SetRemovable();
    copy->SetType(Sound::eSoundFX);
    sound->AddSource(copy);
  }

  void Audio_Init() {
    Sound_Init();

    SDL_AudioSpec* spec = new SDL_AudioSpec;
    spec->freq = 22050;
    spec->format = AUDIO_S16SYS;
    spec->channels = 2;
    spec->samples = 1024;

    sound = new Sound::System(spec);

    spec->userdata = sound;
    spec->callback = sound->GetCallback();

    if(SDL_OpenAudio( spec, NULL ) != 0) {
      fprintf(stderr, "[error] %s\n", SDL_GetError());
      exit(1); // FIXME: shutdown sound system instead
    }

  }

  void Audio_Quit() {
    SDL_PauseAudio(1);
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
    music->SetType(Sound::eSoundMusic);
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
    sample_engine->SetVolume(volume);
    sample_crash->SetVolume(volume);
    if(volume > 0.8)
      sample_recognizer->SetVolume(volume);
    else 
      sample_recognizer->SetVolume(volume * 1.25);
  }

  void Audio_StartEngine(int iPlayer) {
    players[iPlayer]->Start();
  }

  void Audio_StopEngine(int iPlayer) {
    players[iPlayer]->Stop();
  }
 
  void Audio_LoadPlayers() {
    for(int i = 0; i < PLAYERS; i++) {
      if(i != 0) {
	players[i] = new Sound::Source3D(sound, sample_engine);
	players[i]->SetType(Sound::eSoundFX);
	sound->AddSource(players[i]);
      } else {
	players[i] = new Sound::SourceEngine(sound, sample_engine);
	players[i]->SetType(Sound::eSoundFX);
	sound->AddSource(players[i]);
      }
    }
    recognizerEngine = new Sound::Source3D(sound, sample_recognizer);
    recognizerEngine->SetType(Sound::eSoundFX);
    recognizerEngine->Start();
    sound->AddSource(recognizerEngine);
  }

  void Audio_LoadSample(char *name, int number) {
    switch(number) {
    case 0:
      sample_engine = new Sound::SourceSample(sound);
      sample_engine->Load(name);
      break;
    case 1:
      sample_crash = new Sound::SourceSample(sound);
      sample_crash->Load(name);
      break;
    case 2:
      sample_recognizer = new Sound::SourceSample(sound);
      sample_recognizer->Load(name);
      break;
    default:
      /* programmer error, but non-critical */
      fprintf(stderr, "[error] unkown sample %d: '%s'\n", number, name);
    }
  }
}
