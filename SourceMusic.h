#ifndef Sound_SourceMusic_H
#define Sound_SourceMusic_H

#include "Sound.h"

#include "Source.h"
#include "SoundSystem.h"

namespace Sound {
  class SourceMusic : public Source {
  public:
    SourceMusic(System *system);
    void Load(char *filename);
    virtual void Mix(Uint8 *data, int len);

  protected:
    virtual void Reset(void) { 
      if(_position != 0) {
	CleanUp();
	CreateSample();
	fprintf(stderr, "sample resetted\n");
      };
    };
    void CleanUp(void);
    void CreateSample(void);

  private:
    Sound_Sample* _sample;
    Uint32 _buffersize;
    Uint32 _position;
    Uint32 _decoded;

    void *_mem;
    int _mem_size;
    SDL_RWops *_rwops;
  };
}
#endif
