#include "SourceSample.h"

namespace Sound {
  SourceSample::SourceSample(System *system) { 
    _system = system;

    _buffer = NULL;
    _buffersize = 8192;

    _position = 0;
    _decoded = 0;
  }


  void SourceSample::Load(char *filename) {
#define BUFSIZE 1024 * 1024
    int size;
    void *mem;
    SDL_RWops *rwops;

    /*
    fprintf(stderr, "opening sample '%s'\n", filename);

    gzFile file = gzopen(filename, "r");
    mem = (void*) malloc(BUFSIZE);
    size = gzread(file, mem, BUFSIZE);
    gzclose(file);

    fprintf(stderr, "done reading sample '%s'\n", filename);

    rwops = SDL_RWFromMem(mem, size);
    */
    rwops = SDL_RWFromFile(filename, "r");

    Sound_Sample *sample = Sound_NewSample(rwops, NULL,
					   _system->GetAudioInfo(),
					   _buffersize );
    Sound_DecodeAll(sample);

    _buffersize = sample->buffer_size;
    _buffer = new Uint8[_buffersize];
    memcpy(_buffer, sample->buffer, _buffersize);

    // free(mem);
    // SDL_FreeRW(rwops);
    Sound_FreeSample(sample);
    
    fprintf(stderr, "done decoding sample '%s'\n", filename);
    _position = 0;
  }

  void SourceSample::Mix(Uint8 *data, int len) {
    int volume = (int)(_volume * SDL_MIX_MAXVOLUME);
    assert(len < _buffersize);

    if(len < _buffersize - _position) {
      SDL_MixAudio(data, _buffer + _position, len, volume);
      _position += len;
    } else { 
      SDL_MixAudio(data, _buffer + _position, _buffersize - _position,
		   volume);
      len -= _buffersize - _position;

      printf("end of sample reached!\n");
      if(_loop) {
	if(_loop != 255) 
	  _loop--;

	_position = 0;
	SDL_MixAudio(data, _buffer + _position, len, volume);
	_position += len;
      } else {
	_isPlaying = 0;
      }
    }
  }
}
