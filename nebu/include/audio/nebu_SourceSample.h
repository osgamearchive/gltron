#ifndef NEBU_Sound_SourceSample_H
#define NEBU_Sound_SourceSample_H

#include "nebu_Sound.h"

#include "nebu_Source.h"
#include "nebu_SoundSystem.h"

namespace Sound {
  class SourceSample : public Source {
  public:
    SourceSample(System *system);
    virtual ~SourceSample();
    void Load(char *filename);
    virtual int Mix(Uint8 *data, int len);

    Uint8* _buffer;
    Uint32 _buffersize;

  protected:
    virtual void Reset() { _position = 0; }
      
  private:
    Uint32 _position;
    Uint32 _decoded;
  };
}
#endif





