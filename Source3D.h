#ifndef Sound_Source3D_H
#define Sound_Source3D_H

#include "Sound.h"
#include "SoundSystem.h"
#include "SourceSample.h"
#include "Vector3.h"

#define USOUND 50
#define EPSILON 0.1f	 
#define SOUND_VOL_THRESHOLD 0.01
#define VOLSCALE_BASE 1000

namespace Sound {
  class Source3D : public Source { 
  public:
    Source3D(System *system, SourceSample *source) { 
      _system = system; 
      _source = source;

      _location = Vector3(0,0,0);
      _velocity = Vector3(0,0,0);

      _position = 0;
    };
    Vector3 _location;
    Vector3 _velocity;
    SourceSample* _source;

    virtual void Mix(Uint8 *data, int len);
    virtual void GetModifiers(float &fPan, float &fVolume, float &fShift);
    //  protected:
    Uint32 _position;

  protected:
    Source3D() {
      _location = Vector3(0,0,0);
      _velocity = Vector3(0,0,0);

      _position = 0;
    };
  };
}

#endif
