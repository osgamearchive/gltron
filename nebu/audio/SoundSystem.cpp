#include "audio/nebu_SoundSystem.h"

#include <string.h>

#include "base/nebu_debug_memory.h"

namespace Sound {
  System::System(SDL_AudioSpec *spec) { 
    _spec = spec;
    _sources.next = NULL; 

    _info.format = _spec->format;
    _info.rate = spec->freq;
    _info.channels = spec->channels;
    
    _mix_music = 1; // TODO: add 'master' volume for music and fx
    _mix_fx = 1;

    _status = 0; // sound system is not initialized
  }
  System::~System()
  {
	  for(nebu_List *p = &_sources; p->next; p = p->next)
	  {
		  delete (Source*) p->data;
	  }
	  if(_sources.next)
		nebu_List_Free(_sources.next);
	  delete _spec;
  }

  void System::Callback(Uint8* data, int len) {
    // printf("callback got called for %d bytes of data\n", len);

    // ensure silence
    memset(data, 0, len);

    if(_status == eUninitialized) 
      return;

    nebu_List* p;
    int sources_mixed = 0;
    for(p = & _sources; p->next != NULL; p = p->next) {
      Source* s = (Source*) p->data;
      if(s->IsPlaying()) {
				// fprintf(stderr, "mixing source\n");
				if(!(
						 (s->GetType() & eSoundFX && ! _mix_fx ) ||
						 (s->GetType() & eSoundMusic && ! _mix_music) )
					 )
					{
						if( s->Mix(data, len) )
							sources_mixed++;
					}
				// fprintf(stderr, "done mixing %d sources\n", sources_mixed);
      }
    }
  }

  void System::AddSource(Source* source) { 
	  nebu_List_AddTail(&_sources, source);
  }

  void System::Idle(void) {
		/* idle processing */
		nebu_List *p;
		for(p = & _sources; p->next != NULL; p = p->next) {
			Source *source = (Source*) p->data;
			// check if source is removable & has stopped playing
			if(source->IsRemovable() && !source->IsPlaying()) {
				// get rid of data
				p->data = p->next->data;
				nebu_List *tmp = p->next;
				p->next = p->next->next;
				delete tmp;
				delete source;
				if(p->next == NULL)
					break;
			} else {
				source->Idle();
			}
		}
	}

	extern "C" {
    void c_callback(void *userdata, Uint8 *stream, int len) { 
      // printf("c_callback got called for %d bytes of data\n", len);
      ((System*)userdata)->Callback(stream, len);
    }
  }
}

