#ifndef Sound_SourceMusic_H
#define Sound_SourceMusic_H

#include "Sound.h"

#include "Source.h"
#include "SoundSystem.h"

namespace Sound {
  class SourceMusic : public Source {
  public:
    SourceMusic(System *system);
    virtual ~SourceMusic();
    void Load(char *filename);
    virtual int Mix(Uint8 *data, int len);
		virtual void Idle(void);

  protected:
    virtual void Reset(void) { 
      if(_sample != NULL) {
				CleanUp();
				CreateSample();
				fprintf(stderr, "sample resetted\n");
      };
    };
    void CleanUp(void);
    void CreateSample(void);

  private:
    Sound_Sample* _sample;
		Uint32 _sample_buffersize;
		
		Uint8* _buffer;
    Uint32 _buffersize;
    Uint32 _read;
    Uint32 _decoded;

		char *_filename;
    SDL_RWops *_rwops;
  };
}
#endif
