#include "Source3D.h"

Uint8 tmp[65536];
/* ugly old code from gltron's sound3d.c */

#define USOUND 50

void getSound3dData(Vector3& pp, Vector3& vp, Vector3& lc,
		    Vector3& po, Vector3& vo,
	       float *pan, float *vol, float *shift) {
  
  float scalar = lc.x * ( po.x - pp.x ) + lc.y * ( po.y - pp.y );
  
  float temp = ( lc.x * lc.x + lc.y * lc.y ) * 
      ( ( po.x - pp.x ) * ( po.x - pp.x ) + 
	( po.y - pp.y ) * ( po.y - pp.y ) );

  if(temp != 0) {
    float cos2phi = scalar * scalar / temp;
    if(1 - cos2phi > 0)
      *pan = sqrt( 1 - cos2phi );
    else *pan = 0;
    if( lc.x * ( po.y - pp.y ) - lc.y * (po.x - pp.x) > 0 )
      *pan = - *pan;
  } else {
    *pan = 0;
  }

  float dist2 = ( po.x - pp.x ) * ( po.x - pp.x ) +
    ( po.y - pp.y ) * ( po.y - pp.y );
  if(dist2 < 1) dist2 = 1;

  float speed = 1.0;

#define VOLSCALE_BASE 1000
  *vol = 
    (dist2 > VOLSCALE_BASE * speed) ? 
    (VOLSCALE_BASE * speed / dist2) : (1.0);
  *shift = 
    ( USOUND + 
      ( vo.x * ( pp.x - po.x ) + vo.y * ( pp.y - po.y ) ) / sqrt(dist2) ) /
    ( USOUND + 
      ( vp.x * ( pp.x - po.x ) + vp.y * ( pp.y - po.y ) ) / sqrt(dist2) );
}
	      
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
    *(Sint16*) (buf + i) *= left_vol;
    *(Sint16*) (buf + i + 2) *= right_vol;
  }
}
      
#define SOUND_VOL_THRESHOLD 0.01

namespace Sound {
  void Source3D::Mix(Uint8 *data, int len) {
    if(_source->IsPlaying()) {
      int volume = (int)(_source->GetVolume() * SDL_MIX_MAXVOLUME);
      Listener listener = _system->GetListener();
      float pan, shift, vol;
      getSound3dData(listener._location, listener._velocity, 
		     listener._direction,
		     this->_location, this->_velocity,
		     &pan, &vol, &shift);
	
      int clen = (int)((len / 4) * shift + 1) * 4;
      assert(clen < _source->_buffersize);
      // printf("volume: %.4f, panning: %.4f, shift: %.4f\n", vol, pan, shift);

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
      }
    }
  }
}
