#ifndef Sound_SourceCopy_H
#define Sound_SourceCopy_H

#include "Sound.h"
#include "SoundSystem.h"
#include "SourceSample.h"
#include "Vector3.h"

namespace Sound {
  class SourceCopy : public Source { 
  public:
    SourceCopy(SourceSample *source) { 
      _source = source;

      _position = 0;
    };
    SourceSample* _source;
    virtual void Mix(Uint8 *data, int len);
    // protected:
    Uint32 _position;
  };
}

#endif
