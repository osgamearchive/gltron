#include "SourceMusic.h"

namespace Sound {
  SourceMusic::SourceMusic(System *system) { 
    _system = system;

    _sample = NULL;

		_sample_buffersize = 8192;
    _buffersize = 20 * _sample_buffersize;
		_buffer = (Uint8*) malloc( _buffersize );

		_decoded = 0;
    _read = 0;

    _filename = NULL;
    _rwops = NULL;
  }

  SourceMusic::~SourceMusic() { 
    fprintf(stderr, "SourceMusic destructor called\n");

		free(_buffer);
		
    if(_sample)
      Sound_FreeSample( _sample );

    if(_filename)
      free(_filename);
  }

  void SourceMusic::CreateSample(void) {
    _rwops = SDL_RWFromFile(_filename, "r");
    _sample = Sound_NewSample(_rwops, NULL,
			      _system->GetAudioInfo(),
			      _sample_buffersize );
    if(_sample == NULL) {
      fprintf(stderr, "[error] failed loading sample: %s\n", 
	      Sound_GetError());
      return;
    } 

    _read = 0;
    _decoded = 0;
    fprintf(stderr, "created sample\n");
  }

  void SourceMusic::Load(char *filename) {
		int n = strlen(filename);
		_filename = (char*) malloc(n + 1);
		memcpy(_filename, filename, n + 1);
    CreateSample();
  }

  void SourceMusic::CleanUp(void) {
		_read = 0;
    _decoded = 0;

    if(_sample != NULL) 
      Sound_FreeSample(_sample);
  }

  int SourceMusic::Mix(Uint8 *data, int len) {
    if(_sample == NULL) return 0;
		
		// printf("mixing %d bytes\n", len);

    int volume = (int)(_volume * SDL_MIX_MAXVOLUME);
    // fprintf(stderr, "setting volume to %.3f -> %d\n", _volume, volume);
    // fprintf(stderr, "entering mixer\n");
		
    if(len < (_decoded - _read + _buffersize) % _buffersize) {
			// enough data to mix
			if(_read + len <= _buffersize) {
				SDL_MixAudio(data, _buffer + _read, len, volume);
				_read = (_read + len) % _buffersize;
			} else {
				// wrap around in buffer
				// printf("wrap around in buffer\n");
				SDL_MixAudio(data, _buffer + _read, _buffersize - _read, volume);
				len -= _buffersize - _read;
				SDL_MixAudio(data, _buffer, len, volume);
				_read = len;
			}
		} else {
			// buffer under-run
			printf("buffer underrun!\n");
			// don't do anything
		}
    return 1;
  }

	void SourceMusic::Idle(void) {
		if(_sample == NULL)
			return;
		// printf("idling\n");
		while( _read == _decoded || 
					 (_read - _decoded + _buffersize) % _buffersize >
					 _sample_buffersize )	{
			if(_read == _decoded)
				printf("_read == _decoded == %d\n", _read);
			// fill the buffer
			int count = Sound_Decode(_sample);
			// printf("adding %d bytes to buffer\n", count);
			if(count <= _buffersize - _decoded) {
				memcpy(_buffer + _decoded, _sample->buffer, count);
			} else {
				// wrapping around end of buffer
				printf("wrapping around end of buffer\n");
				memcpy(_buffer + _decoded, _sample->buffer, _buffersize - _decoded);
				memcpy(_buffer, (Uint8*) _sample->buffer + _buffersize - _decoded,
							 count - (_buffersize - _decoded));
			}
			_decoded = (_decoded + count) % _buffersize;

			// check for end of sample, loop
			if(_sample->flags) {
				// some error has occurer, maybe end of sample reached
				CleanUp();
				fprintf(stderr, "end of sample reached!\n");
				if(_loop) {
					fprintf(stderr, "looping music\n");
					if(_loop != 255) 
						_loop--;
					CreateSample();
				} else {
					_isPlaying = 0;
				}
			}
		} // buffer has been filled
	}
}







