#include "Source.h"

namespace Sound {
  Source::Source() {
    _system = NULL;
    _isPlaying = 0;
    _loop = 0;
    _removable = 0;
    _volume = 0.5;
    _type = 0;
  }
  Source::~Source() {
    fprintf(stderr, "Source destructor called\n");
  }

  int Source::Mix(Uint8 *data, int len) { }
  void Source::Reset() { }
  void Source::Start() { _isPlaying = 1; Reset(); }
  void Source::Stop() { _isPlaying = 0; Reset(); }
  void Source::Pause() { _isPlaying = 0; }
  void Source::UnPause() { _isPlaying = 1; }
	void Source::Idle() { }

  Uint8 Source::IsPlaying() { return _isPlaying; }
  void Source::SetRemovable() { _removable = 1; }
  Uint8 Source::IsRemovable() { return _removable; }
  void Source::SetVolume(float volume) { _volume = volume; }
  float Source::GetVolume() { return _volume; }
  void Source::SetLoop(Uint8 loop) { _loop = loop; }
  Uint8 Source::GetLoop() { return _loop; }
  void Source::SetType(int type) { _type = type; }
  int Source::GetType(void) { return _type; }
  void Source::SetName(char* name) { _name = name; }
  char* Source::GetName(void) { return _name; }
}
