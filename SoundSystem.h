#ifndef Sound_System_H
#define Sound_System_H

#include "gltron.h"

#include "Sound.h"
#include "Source.h"
#include "Vector3.h"

namespace Sound {
  extern "C" {
    void c_callback(void *userdata, Uint8 *stream, int len);
  }

  class Listener {
  public:
    Listener() { };
    Vector3 _location;
    Vector3 _velocity;
    Vector3 _direction;
  };

  class System {
  public:
    System(SDL_AudioSpec *spec); 
    typedef void(*Audio_Callback)(void *userdata, Uint8* data, int len);
    Audio_Callback GetCallback() { return c_callback; };
    void Callback(Uint8* data, int len);
    void Idle(); /* remove dead sound sources */
    void AddSource(Source* source);
    Sound_AudioInfo* GetAudioInfo() { return &_info; };
    Listener& GetListener() { return _listener; };

  protected:
    SDL_AudioSpec *_spec;
    Sound_AudioInfo _info;
    Listener _listener;
    list _sources;
  };

}

#endif







