#include "SourceMusic.h"

namespace Sound {
  SourceMusic::SourceMusic(System *system) { 
    _system = system;

    _sample = NULL;
    _buffersize = 8192;
    _position = 0;
    _decoded = 0;

    _mem = NULL;
    _mem_size = 0;
    _rwops = NULL;
  }

  void SourceMusic::CreateSample(void) {
    _rwops = SDL_RWFromMem(_mem, _mem_size);
    _sample = Sound_NewSample(_rwops, NULL,
			      _system->GetAudioInfo(),
			      _buffersize );

    _position = 0;
    _decoded = _buffersize;
    fprintf(stderr, "created sample\n");
  }

  void SourceMusic::Load(char *filename) {
#define BUFSIZE 1024 * 1024
    gzFile file = gzopen(filename, "r");
    if(_mem != NULL) free(_mem);
    _mem = (void*) malloc(BUFSIZE);
    _mem_size = gzread(file, _mem, BUFSIZE);
    fprintf(stderr, "read %d bytes\n", _mem_size);
    gzclose(file);
    CreateSample();
  }

  void SourceMusic::CleanUp(void) {
    // cleanup doesn't free the memory of the loaded sample data
    _position = 0;
    _decoded = 0;

    if(_sample != NULL) 
      Sound_FreeSample(_sample);
  }

  void SourceMusic::Mix(Uint8 *data, int len) {
    assert(len < _buffersize);
    int volume = (int)(_volume * SDL_MIX_MAXVOLUME);
    // fprintf(stderr, "setting volume to %.3f -> %d\n", _volume, volume);
    // fprintf(stderr, "entering mixer\n");
    Uint8* buffer = (Uint8*) _sample->buffer;

    if(len <= _decoded - _position) {
      SDL_MixAudio(data, buffer + _position, len, volume);
      _position += len;
      // fprintf(stderr, "mixed %d bytes without decoding\n", len);
    } else { 
      SDL_MixAudio(data, buffer + _position, _decoded - _position,
		   volume);
      len -= _decoded - _position;
      _position = 0;
      _decoded = 0;
      _decoded += Sound_Decode(_sample);
#warning "can the buffer may have changed after decoding?"
      buffer = (Uint8*) _sample->buffer;
      // fprintf(stderr, "decoded %d bytes\n", _decoded);
      if(len <= _decoded) {
	SDL_MixAudio(data, buffer + _position, len, volume);
	_position += len;
	// fprintf(stderr, "mixed %d bytes after decoding\n", len);
      } else {
	// fprintf(stderr, "mixing remaining %d bytes\n");
	SDL_MixAudio(data, buffer + _position, _decoded - _position, 
		     volume);
	CleanUp();
	// fprintf(stderr, "end of sample reached!\n");
	if(_loop) {
	  fprintf(stderr, "looping music\n");
	  if(_loop != 255) 
	    _loop--;
	  CreateSample();
	} else {
	  _isPlaying = 0;
	}
      }
    }
  }
}
