#ifndef Sound_SourceSample_H
#define Sound_SourceSample_H

#include "Sound.h"

#include "Source.h"
#include "SoundSystem.h"

namespace Sound {
  class SourceSample : public Source {
  public:
    SourceSample(System *system);
    virtual ~SourceSample();
    void Load(char *filename);
    virtual void Mix(Uint8 *data, int len);

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





