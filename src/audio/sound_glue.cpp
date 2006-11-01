#include "audio/sound_glue.h"
extern "C" {
#include "game/game.h"
#include "video/recognizer.h"
#include "scripting/nebu_scripting.h"
#include "video/video.h"
#include "game/camera.h"
#include "game/game_data.h"
#include "configuration/settings.h"
}

#include "Nebu_audio.h"

#include "SDL.h"
#include "SDL_sound.h"

#include "base/nebu_debug_memory.h"

static Sound::System *sound = NULL;
static Sound::SourceMusic *music = NULL;
static Sound::SourceSample *sample_crash = NULL;
static Sound::SourceSample *sample_engine = NULL;
static Sound::SourceSample *sample_recognizer = NULL;

static Sound::Source3D *players[MAX_PLAYERS];
static Sound::Source3D *recognizerEngine;

#define TURNLENGTH 250.0f

static void output_decoders(void)
{
    const Sound_DecoderInfo **rc = Sound_AvailableDecoders();
    const Sound_DecoderInfo **i;
    const char **ext;

    printf("Supported sound formats:\n");
    if (rc == NULL)
        printf(" * Apparently, NONE!\n");
    else
    {
        for (i = rc; *i != NULL; i++)
        {
            printf(" * %s\n", (*i)->description);
            for (ext = (*i)->extensions; *ext != NULL; ext++)
                printf("   File extension \"%s\"\n", *ext);
            printf("   Written by %s.\n   %s\n\n", (*i)->author, (*i)->url);
        } /* for */
    } /* else */

    printf("\n");
} /* output_decoders */

extern "C" {


  void Audio_EnableEngine(void) {
		int i;
		for(i = 0; i < game->players; i++)
			if( game->player[i].data->speed > 0)
				Audio_StartEngine(i);
    sample_engine->Start();
    if (gSettingsCache.show_recognizer)
      sample_recognizer->Start();
    // printf("[audio] turning on engine sound\n");
  }

  void Audio_DisableEngine(void) {
    sample_engine->Stop();
    sample_recognizer->Stop();
    // printf("[audio] turning off engine sound\n");
  }

  void Audio_Idle(void) { 
    // iterate over all the players and update the engines
    if(sample_engine->IsPlaying()) {
      for(int i = 0; i < MAX_PLAYERS; i++) {
				Player *p;
				Sound::Source3D *p3d;
				float x, y;
				p3d = players[i];
				p = game->player + i;
				getPositionFromIndex(&x, &y, i);
				p3d->_location = Vector3(x, y, 0);
				float V = p->data->speed;

				int dt = game2->time.current - p->data->turn_time;
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
					if(p->data->boost_enabled) {
						( (Sound::SourceEngine*) p3d )->_speedShift = 1.2f;
					} else {
						( (Sound::SourceEngine*) p3d )->_speedShift = 1.0f;
					}
					( (Sound::SourceEngine*) p3d )->_pitchShift =
						p->data->speed / getSettingf("speed");
				}
						
#if 0
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
#endif
      }
    }

    if(sample_recognizer->IsPlaying()) {
      if (gSettingsCache.show_recognizer) {
				vec2 p, v;
				getRecognizerPositionVelocity(&p, &v);
				// recognizerEngine->_location = Vector3(p.x, p.y, RECOGNIZER_HEIGHT);
				recognizerEngine->_location = Vector3(p.v[0], p.v[1], 10.0f);
				recognizerEngine->_velocity = Vector3(v.v[0], v.v[1], 0);
      }
    }

		if(music && !music->IsPlaying()) {
			// check if music is enabled. if it is, advance to
			// next song
			if(gSettingsCache.playMusic) {
				scripting_Run("nextTrack()");
			}
		}

    Sound::Listener& listener = sound->GetListener();

    listener._location = Vector3(gPlayerVisuals[0].camera.cam);
		Vector3 v1 = Vector3(gPlayerVisuals[0].camera.target);
		Vector3 v2 = Vector3(gPlayerVisuals[0].camera.cam);
    listener._direction = v1 - v2;
      
    // listener._location = players[0]->_location;
    // listener._direction = players[0]->_velocity;
    listener._velocity = players[0]->_velocity;

    listener._up = Vector3(0, 0, 1);

    sound->SetMixMusic(gSettingsCache.playMusic);
    sound->SetMixFX(gSettingsCache.playEffects);
    sound->Idle();
  }

  void Audio_CrashPlayer(int player) {
    Sound::SourceCopy *copy = new Sound::SourceCopy(sample_crash);
    copy->Start();
    copy->SetRemovable();
    copy->SetType(Sound::eSoundFX);
    sound->AddSource(copy);
  }

  void Audio_Init(void) {
    Sound_Init(); // Init SDL_Sound
    // output_decoders();

    SDL_AudioSpec* spec = new SDL_AudioSpec;
    spec->freq = 22050;
    spec->format = AUDIO_S16SYS;
    spec->channels = 2;
    spec->samples = 1024;

    sound = new Sound::System(spec);

    spec->userdata = sound;
    spec->callback = sound->GetCallback();

	SDL_AudioSpec obtained;

    if(SDL_OpenAudio( spec, &obtained ) != 0) {
      fprintf(stderr, "[error] %s\n", SDL_GetError());
      sound->SetStatus(Sound::eUninitialized);
    } else {
      sound->SetStatus(Sound::eInitialized);
			/*
			fprintf(stderr, "[sound] frequency: %d\n", obtained.freq);
			fprintf(stderr, "[sound] format: %d\n", obtained.format);
			fprintf(stderr, "[sound] channels: %d\n", obtained.channels);
			fprintf(stderr, "[sound] silence: %d\n", obtained.silence);
			fprintf(stderr, "[sound] buffer in samples: %d\n", obtained.samples);
			fprintf(stderr, "[sound] buffer in bytes: %d\n", obtained.size);
			*/
    }
    sound->SetMixMusic(gSettingsCache.playMusic);
    sound->SetMixFX(gSettingsCache.playEffects);
  }

  void Audio_Start(void) {
    SDL_PauseAudio(0);
  }

  void Audio_Quit(void) {
    SDL_PauseAudio(1);
    Sound_Quit();
    SDL_CloseAudio();
	if(sound)
	{
		delete sound;
		sound = NULL;
	}
	if(sample_crash)
	{
		delete sample_crash;
		sample_crash = NULL;
	}
	if(sample_engine)
	{
		delete sample_engine;
		sample_engine = NULL;
	}
	if(sample_recognizer)
	{
		delete sample_recognizer;
		sample_recognizer = NULL;
	}
  }

  void Audio_LoadMusic(char *name) {
    if(music != NULL) {
      music->Stop();
      music->SetRemovable();
    }
    music = new Sound::SourceMusic(sound);
    music->Load(name);
		if(getSettingi("loopMusic"))
			music->SetLoop(255);
    music->SetType(Sound::eSoundMusic);

    char *sname = new char[32];
    sprintf(sname, "music");
    music->SetName(sname);
    sound->AddSource(music);
  }

  void Audio_PlayMusic(void) {
    music->Start();
  }

  void Audio_StopMusic(void) {
    music->Stop();
  }

  void Audio_SetMusicVolume(float volume) {
    music->SetVolume(volume);
  }
  
  void Audio_SetFxVolume(float volume) {
    sample_engine->SetVolume(volume);
    sample_crash->SetVolume(volume);
    if(volume > 0.8f)
      sample_recognizer->SetVolume(volume);
    else 
      sample_recognizer->SetVolume(volume * 1.25f);
  }

  void Audio_StartEngine(int iPlayer) {
    players[iPlayer]->Start();
  }

  void Audio_StopEngine(int iPlayer) {
    players[iPlayer]->Stop();
  }
 
  void Audio_LoadPlayers(void) {
    for(int i = 0; i < MAX_PLAYERS; i++) {
      if(i != 0) {
				players[i] = new Sound::Source3D(sound, sample_engine);
				players[i]->SetType(Sound::eSoundFX);
				sound->AddSource(players[i]);

				char *name = new char[32];
				sprintf(name, "player %d", i);
				players[i]->SetName(name);

      } else {
				players[i] = new Sound::SourceEngine(sound, sample_engine);
				players[i]->SetType(Sound::eSoundFX);
				sound->AddSource(players[i]);

				char *name = new char[32];
				sprintf(name, "player %d", i);
				players[i]->SetName(name);
      }
    }
    recognizerEngine = new Sound::Source3D(sound, sample_recognizer);
    recognizerEngine->SetType(Sound::eSoundFX);
    recognizerEngine->Start();
    sound->AddSource(recognizerEngine);

    char *name = new char[32];
    sprintf(name, "recognizer");
    recognizerEngine->SetName(name);

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
