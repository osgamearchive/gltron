#include "gltron.h"


#ifdef macintosh
/* it seems that MacOS 8.6 and lower do not have interrupt-safe memcpy */
#define memcpy(from,to,size) BlockMoveData(to,from,size)
/* important: cannot call printf/fprintf inside an interrupt procedure */
#define fprintf(arg1, arg2)
#endif

#define USOUND 500
void getSound3dData(int i_player, int i_object, 
	       float *pan, float *vol, float *shift) {
  Point vp, vo, lc, pc, pp, po;
  Player *player, *object;

  float cos2phi;
  float dist2;
  float scalar;

  float temp;
  
  player = &(game->player[ i_player ]);
  object = &(game->player[ i_object ]);
  
  pp.x = player->data->posx;
  pp.y = player->data->posy;

  vp.x = game->settings->game_speed * dirsX[ player->data->dir ];
  vp.y = game->settings->game_speed * dirsY[ player->data->dir ];

  lc.x = player->camera->target[0] - player->camera->cam[0];
  lc.y = player->camera->target[1] - player->camera->cam[1];

  pc.x = player->camera->cam[0];
  pc.y = player->camera->cam[1];

  po.x = object->data->posx;
  po.y = object->data->posy;

  vo.x = game->settings->game_speed * dirsX[ object->data->dir ];
  vo.y = game->settings->game_speed * dirsY[ object->data->dir ];

  scalar = lc.x * ( po.x - pp.x ) + lc.y * ( po.y - pp.y );
  
  temp = ( lc.x * lc.x + lc.y * lc.y ) * 
      ( ( po.x - pp.x ) * ( po.x - pp.x ) + 
	( po.y - pp.y ) * ( po.y - pp.y ) );
  if(temp != 0) {
    cos2phi = scalar * scalar / temp;
    if(1 - cos2phi > 0)
      *pan = sqrt( 1 - cos2phi );
    else *pan = 0;
    if( lc.x * ( po.y - pp.y ) - lc.y * (po.x - pp.x) > 0 )
      *pan = - *pan;
  } else {
    *pan = 0;
  }

  dist2 = ( po.x - pp.x ) * ( po.x - pp.x ) +
    ( po.y - pp.y ) * ( po.y - pp.y );
  if(dist2 < 1) dist2 = 1;

#define VOLSCALE_BASE 1000
  *vol = 
    (dist2 > VOLSCALE_BASE * game->settings->game_speed) ? 
    (VOLSCALE_BASE * game->settings->game_speed / dist2) : (1.0);
  *shift = 
    ( USOUND + 
      abs( vo.x * ( vp.x - po.x ) + vo.y * ( vp.y - po.y ) ) / sqrt(dist2) ) /
    ( USOUND + 
      abs( vp.x * ( vp.x - po.x ) + vp.y * ( vp.y - po.y ) ) / sqrt(dist2) );
}

int checkFormat() {
  /*
  if(format != AUDIO_S16SYS) {
    fprintf(stderr, "can't perform fx, unknown format\n");
    return 1;
  }
  */
  return 0;
}
	      
void fxShift(float shift, Uint8 *target, Uint8 *source, int len, int *consumed) {
  int i, j, k;
  float l;
  float pa = 0;

  if(checkFormat()) {
    *consumed = 0;
    return;
  }

  len /= 4; 

  for(i = 0; i < len; i++) { // LR pairs 
    for(j = 0; j < 2; j++) { // channels
      pa = i * shift;
      k = (int) pa;
      l = pa - k;

      *(Sint16*) (target + 2 * j + 4 * i) +=
	( *(Sint16*) (source + 2 * j + 4 * (k + 0) ) * ( 1 - l ) +
	  *(Sint16*) (source + 2 * j + 4 * (k + 2) ) * ( l ) );
      
    }
  }
  *consumed = ( (int)(len * shift + 0.49999) ) * 4;
}

void fxPan(float pan, float vol, Uint8 *buf, int len) {
  int i;

  float left_vol =  - vol * ( -1.0 + pan ) / 2.0;
  float right_vol = vol * ( 1.0 + pan ) / 2.0;

  if(checkFormat())
    return;

  for(i = 0; i < len; i += 4) {
    *(Sint16*) (buf + i) *= left_vol;
    *(Sint16*) (buf + i + 2) *= right_vol;
  }
}
      
static int *sourceBuf = NULL;

extern Mix_Chunk *game_fx;
  
void moveSoundSource(int type, int player, int object, int len) {
  Mix_Chunk *chunk;

  switch(type) {
  case SOUND_ENGINE:
    chunk = game_fx + SOUND_ENGINE;
    break;
  default:
    fprintf(stderr, "unknown sound source requested\n");
    return;
    break;
  }
  sourceBuf[ player * MAX_PLAYERS + object] =
    ( sourceBuf[ player * MAX_PLAYERS + object ] + len ) % chunk->alen;
}

static Uint8 soundBuffer[65536];
static int sourceBuffer[MAX_PLAYERS * MAX_PLAYERS];

Uint8* getSoundSource(int type, int player, int object, int len, int *delivered) {
  Mix_Chunk *chunk;
  Uint8* buf;
  int i, rest, pos;
  int loop;

  if(sourceBuf == NULL) {
    sourceBuf = sourceBuffer;
    for(i = 0; i < MAX_PLAYERS * MAX_PLAYERS; i++) {
      sourceBuf[ i ] = 0;
    }
  }

  switch(type) {
  case SOUND_ENGINE:
    chunk = game_fx + SOUND_ENGINE;
    loop = 1;
    break;
  case SOUND_CRASH:
    chunk = game_fx + SOUND_CRASH;
    loop = 0;
    break;
  default:
    fprintf(stderr, "unknown sound source requested\n");
    return NULL;
    break;
  }
  
  buf = soundBuffer;
  pos = sourceBuf[ player * MAX_PLAYERS + object ];
  rest = chunk->alen - pos;
  // fprintf(stderr, "pos is %d, rest is %d, len is %d\n", pos, rest, len);


  if(rest < len) {
    // fprintf(stderr, "rest < len\n");
    if(loop == 1) {
      // fprintf(stderr, "before memcpy\n");
      memcpy(buf, chunk->abuf + pos, rest);
      // fprintf(stderr, "before memcpy2\n");
      memcpy(buf + rest, chunk->abuf, len - rest);
      // fprintf(stderr, "after memcpy\n");
      *delivered = len;
    } else {
      // fprintf(stderr, "no loop\n");
      memcpy(buf, chunk->abuf + pos, rest);
      *delivered = rest;
    } 
  } else {
    memcpy(buf, chunk->abuf + pos, len);
    // int i;
    // fprintf(stderr, "rest > len\n");
    // for(i = 0; i < len; i++) {
    // *(buf + i) = *(chunk->abuf + pos + i);
    // fprintf(stderr, "%d\n", i);
    *delivered = len;
  }
  // fprintf(stderr, "after memcpy\n");

  return buf;
}


#define SOUND_VOL_THRESHOLD 0.01

void playCrashSound(int player_i, int player_o) {
  float pan, vol, shift;
  if(game->settings->playEffects == 0)
    return;
  getSound3dData(player_i, player_o, &pan, &vol, &shift);
  printf("starting crash sound with volume %.2f\n",  game->settings->fxVolume * (0.5 + vol / 2.0));
  playGameFX(SOUND_CRASH, game->settings->fxVolume * (0.5 + vol / 2.0));
}

void mixEngineSound(int player_i, Uint8* buf, int len) {
    float pan, vol, shift;
    Uint8 *source;
    int clen;
    int consumed;
    int delivered;
    int i;
  
    if(game->settings->playEffects == 0)
      return;

    
    for(i = 0; i < game->players; i++) {
      if(i != player_i && game->player[i].data->speed > 0) {
	getSound3dData(player_i, i, &pan, &vol, &shift);
	
	clen = (int)((len / 4) * shift + 1) * 4;

	/*
	  printf("sound data for player %d / %d: %.2f %.2f %.2f\n",
	  player_i, i, pan, vol, shift);
	*/
	if(vol > SOUND_VOL_THRESHOLD) {
	  source = getSoundSource(SOUND_ENGINE, player_i, i, clen, &delivered);
	  fxPan(pan, game->settings->fxVolume * vol, source, clen);
	  fxShift(shift, buf, source, len, &consumed);
	  moveSoundSource(SOUND_ENGINE, player_i, i, consumed);
	}
    }
  }
  // fprintf(stderr, "finished mixing\n");
}
