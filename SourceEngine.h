#ifndef Sound_SourceEngine_H
#define Sound_SourceEngine_H

#include "Source3D.h"

namespace Sound {
  class SourceEngine : public Source3D {
  public:
    SourceEngine(System *system, SourceSample *source) {
      _system = system;
      _source = source;

      _speedShift = 1.0f;
      _pitchShift = 0.0f;
    };

    virtual void GetModifiers(float &fPan, float &fVolume, float &fShift);

    float _speedShift;
    float _pitchShift;
  };
}

#endif
