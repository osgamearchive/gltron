#ifndef Sound_Source_H
#define Sound_Source_H

#include "Sound.h"

namespace Sound {
  enum {
    eSoundMusic = 1,
    eSoundFX = 2
  };

  class System;

  class Source { // an abstract class, the basic interface for all Sources
  public:
    Source();
    virtual ~Source();
    virtual void Start();
    virtual void Stop();
    virtual void Pause();
    virtual void UnPause();
    virtual Uint8 IsPlaying();
    virtual int Mix(Uint8 *data, int len); // abstract
    virtual void SetRemovable(void);
    virtual Uint8 IsRemovable(void);
    virtual void SetVolume(float volume);
    virtual float GetVolume();
    virtual void SetLoop(Uint8 loop);
    virtual Uint8 GetLoop();
    virtual void SetType(int type);
    virtual int GetType(void);
    void SetName(char *name);
    char* GetName(void);

  protected:
    virtual void Reset();

    System* _system;
    Uint8 _isPlaying;
    Uint8 _loop;
    Uint8 _removable;
    float _volume;
    int _type;
    char* _name;
  };
}

#endif




