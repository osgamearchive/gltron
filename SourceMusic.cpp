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

  SourceMusic::~SourceMusic() { 
    fprintf(stderr, "SourceMusic destructor called\n");
    if(_sample)
      Sound_FreeSample( _sample );

    if(_mem)
      free(_mem);
    // Source::~Source();
  }

  void SourceMusic::CreateSample(void) {
    _rwops = SDL_RWFromMem(_mem, _mem_size);
    _sample = Sound_NewSample(_rwops, NULL,
			      _system->GetAudioInfo(),
			      _buffersize );
    if(_sample == NULL) {
      fprintf(stderr, "[error] failed loading sample: %s\n", 
	      Sound_GetError());
      exit(1); // FIXME: handle corrupt sample gracefully
    }

    _position = 0;
    _decoded = _buffersize;
    fprintf(stderr, "created sample\n");
  }

  void SourceMusic::Load(char *filename) {
    // FIXME: grow buffer as needed
#define BUFSIZE 10 * 1024 * 1024
    file_handle file = file_open(filename, "rb");
    if(_mem != NULL) free(_mem);
    _mem = (void*) malloc(BUFSIZE);
    _mem_size = 0;
    int readbytes;
    while(1) {
      readbytes = file_read(file, (char*)_mem + _mem_size, 8192);
      _mem_size += readbytes;
      if(readbytes != 8192) {
	if( feof( file ) ) {
	  fprintf(stderr, "eof reached\n");
	}
	fprintf(stderr, "read %d bytes from %s, error number %d\n", readbytes, filename,  ferror( file ));
	break;
      }
    }
    fprintf(stderr, "read a total of %d bytes\n", _mem_size);
    file_close(file);
    CreateSample();
  }

  void SourceMusic::CleanUp(void) {
    // cleanup doesn't free the memory of the loaded sample data
    _position = 0;
    _decoded = 0;

    if(_sample != NULL) 
      Sound_FreeSample(_sample);
  }

  int SourceMusic::Mix(Uint8 *data, int len) {
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
    //fprintf(stderr, "mixed %s\n", GetName());
    return 1;
  }
}







