#include "Source3D.h"

Uint8 tmp[65536];

#define USOUND 50
#define EPSILON 0.1f	 
#define SOUND_VOL_THRESHOLD 0.1
#define VOLSCALE_BASE 1000
     
int fxShift(float shift, Uint8 *target, Uint8 *source, int len) {
  int i, j, k;
  float l;
  float pa = 0;

  len /= 4; 

  for(i = 0; i < len; i++) { // LR pairs 
    for(j = 0; j < 2; j++) { // channels
      pa = i * shift;
      k = (int) pa;
      l = pa - k;

      *(Sint16*) (target + 2 * j + 4 * i) += (Sint16) 
	( *(Sint16*) (source + 2 * j + 4 * (k + 0) ) * ( 1 - l ) +
	  *(Sint16*) (source + 2 * j + 4 * (k + 2) ) * ( l ) );
      
    }
  }
  return ( (int)(len * shift + 0.49999) ) * 4;
}

void fxPan(float pan, float vol, Uint8 *buf, int len) {
  int i;

  float left_vol =  - vol * ( -1.0 + pan ) / 2.0;
  float right_vol = vol * ( 1.0 + pan ) / 2.0;

  for(i = 0; i < len; i += 4) {
    *(Sint16*) (buf + i) = // *= left_vol
      (Sint16) (left_vol * *(Sint16*) (buf + i) );
    *(Sint16*) (buf + i + 2) = // *= right_vol
      (Sint16) (right_vol * *(Sint16*) (buf + i + 2) );
  }
}
      
namespace Sound {

  void Source3D::GetModifiers(float& fPan, float& fVolume, float& fShift) {

    Vector3& vSourceLocation = _location;
    Vector3& vSourceVelocity = _velocity;

    Listener listener = _system->GetListener();
    Vector3& vListenerLocation = listener._location;
    Vector3 vListenerVelocity = listener._velocity;
    Vector3 vListenerDirection = listener._direction;
    Vector3 vListenerUp = listener._up;

    if( (vSourceLocation - vListenerLocation).Length() < EPSILON  ) {
      fPan = 0;
      fVolume = 1.0f;
      fShift = 1.0;
      return;
    }

    vListenerDirection.Normalize();
    vListenerUp.Normalize();
    Vector3 vListenerLeft = vListenerDirection.Cross( vListenerUp );
    vListenerLeft.Normalize();

    /* panning */
    Vector3 vTarget = vSourceLocation - vListenerLocation;
    Vector3 vTargetPlanar = 
      vListenerLeft * ( vTarget * vListenerLeft ) +
      vListenerDirection * (vTarget * vListenerDirection );
  
    float cosPhi = 
      vTargetPlanar.Normalize() * 
      vListenerDirection;

    fPan = 1 - fabs(cosPhi);

    if( vTargetPlanar * vListenerLeft < 0 )
      fPan = -fPan;

  /* done panning */

  /* attenuation */
    // float fallOff = vTarget.Length2();
    float fallOff = pow(vTarget.Length(), 1.8);
    fVolume = (fallOff > VOLSCALE_BASE) ?
      (VOLSCALE_BASE / fallOff) : (1.0);
  
    /* done attenuation */

  /* doppler */

    fShift = 
      (USOUND + ( vListenerVelocity * vTarget ) / vTarget.Length() ) / 
      (USOUND + ( vSourceVelocity * vTarget ) / vTarget.Length() );

    /* done doppler */
  }

  int Source3D::Mix(Uint8 *data, int len) {
    if(_source->IsPlaying()) {
      int volume = (int)(_source->GetVolume() * SDL_MIX_MAXVOLUME);
      float pan, shift, vol;


      GetModifiers(pan, vol, shift);
      // printf("volume: %.4f, panning: %.4f, shift: %.4f\n", vol, pan, shift);

      int clen = (int)((len / 4) * shift + 1) * 4;
      assert(clen < _source->_buffersize);

      if(vol > SOUND_VOL_THRESHOLD) {
	// copy clen bytes from the buffer to a temporary buffer
	if(clen <= _source->_buffersize - _position) {
	  memcpy(tmp, _source->_buffer + _position, clen);
	} else {
	  memcpy(tmp, _source->_buffer + _position,
		 _source->_buffersize - _position);
	  memcpy(tmp + _source->_buffersize - _position, _source->_buffer,
		 clen - (_source->_buffersize - _position));
	}

	fxPan(pan, vol, tmp, clen);

	// fxPan(pan, vol, tmp, len);
	// SDL_MixAudio(data, tmp, len, SDL_MIX_MAXVOLUME);

	// fxshift mixes the data to the stream
	_position += fxShift(shift, data, tmp, len);

	if(_position > _source->_buffersize)
	  _position -= _source->_buffersize;

	fprintf(stderr, "mixed %s\n", GetName());
	return 1; // mixed something
      }
    }
    return 0; // didn't mix anything to the stream
  }
}









