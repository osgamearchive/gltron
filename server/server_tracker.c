#include "server_gltron.h"


void
tracker_sendinfos()
{
  Trackerpacket packet;
  packet.type=TINFOS;
  packet.which=0;
  packet.infos.infos.speed  = game->settings->game_speed;
  packet.infos.infos.size   = game->settings->arena_size;
  packet.infos.infos.erase  = game->settings->erase_crashed;
  packet.infos.infos.ipaddress.port  = settings.port;
  strcpy(packet.infos.infos.description, settings.description);
  strcpy(packet.infos.infos.version, VERSION);
  packet.infos.infos.nbplayers = nbUsers;
  Net_tsendpacket(&packet, Net_gettrackersock());
}

void
tracker_connect()
{
  Trackerpacket packet;
  printf("conecting to tracker %s:%d\n", settings.tracker, settings.tport);
  if( Net_tconnect(settings.tracker, settings.tport) )
    {
      fprintf(stderr, "can't connect to the tracker! %s\n", SDLNet_GetError());
      exit(1);
    }

  packet.type=TLOGIN;
  packet.which=0;
  packet.infos.login.type=SERVER;
  strcpy(packet.infos.login.passwd, settings.passwd);
  Net_tsendpacket(&packet, Net_gettrackersock());

  tracker_sendinfos();
}
