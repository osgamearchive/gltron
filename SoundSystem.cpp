#include "SoundSystem.h"

namespace Sound {
  System::System(SDL_AudioSpec *spec) { 
    _spec = spec;
    _sources.next = NULL; 

    _info.format = _spec->format;
    _info.rate = spec->freq;
    _info.channels = spec->channels;
    
    _mix_music = 1;
    _mix_fx = 1;
  }

  void System::Callback(Uint8* data, int len) {
    // printf("callback got called for %d bytes of data\n", len);
    // ensure silence
    memset(data, 0, len);
    list* p;
    for(p = & _sources; p->next != NULL; p = p->next) {
      Source* s = (Source*) p->data;
      if(s->IsPlaying()) {
	// fprintf(stderr, "mixing source\n");
	if(!(
	     (s->GetType() & eSoundFX && ! _mix_fx ) ||
	     (s->GetType() & eSoundMusic && ! _mix_music) )
	   )
	  {
	     s->Mix(data, len);
	  }
	// fprintf(stderr, "done mixing\n");
      } else {
	// check if source is removable
	if(s->IsRemovable()) {
	  // get rid of data
	  delete (Source*) p->data;
	  p->data = p->next->data;
	  list *tmp = p->next;
	  p->next = p->next->next;
	  delete tmp;
	  if(p->next == NULL)
	    break;
	}
      }
    }
  }

  void System::AddSource(Source* source) { 
    list* p;
    for(p = & _sources; p->next != NULL; p = p->next);
    p->next = new list;
    p->next->next = NULL;
    p->data = source;
  }

  void System::Idle(void) {
#warning "todo: remove dead sound sources"
  }

  extern "C" {
    void c_callback(void *userdata, Uint8 *stream, int len) { 
      // printf("c_callback got called for %d bytes of data\n", len);
      ((System*)userdata)->Callback(stream, len);
    }
  }
}

