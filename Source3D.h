#ifndef Sound_Source3D_H
#define Sound_Source3D_H

#include "Sound.h"
#include "System.h"
#include "SourceSample.h"
#include "Vector3.h"

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
    //  protected:
    Uint32 _position;
  };
}

#endif
