#include "gltron.h"
#define MAX_CHARS 31
static char buff[MAX_CHARS]; //TODO Defines MAX_CHARS
static int  nbreads;

void
initkeyreading()
{
  nbreads=0;
  strcpy(buff, "");
  printf("Entering keyboardreadingCallbacks...\n");
}

void
handlecommand(char *command, char *params)
{
  Packet   packet;
  char     *str;
  int      i;

  //check commands here.
  switch( command[0] )
    {
    case 's'://start
      if( serverstate == preGameState && isConnected )
	{
	  if( slots[me].isMaster )
	    {
	      printf("\nAsk to start the game\n");	      
	      makeping(game2->time.current);
	      printf("ping ask when start game is%d\n", getping());
	      packet.which=me;
	      packet.type=ACTION;
	      packet.infos.action.type=STARTGAME;
	      Net_sendpacket(&packet, Net_getmainsock());
	    } else {
	      fprintf(stderr,"\nYour are not allowed to start the game, u must be Game Master\n");
	    }
	}
      break;
    case 'w':
      if( serverstate == preGameState && isConnected  )
	{
	  //Whisper
	  packet.which=me;
	  packet.type=CHAT;

	  //Find dest
	  str = strtok(params, " ");
	  printf("wisper to %s\n", str);
	  packet.infos.chat.which=BROADCAST;
	  for(i=0; i < MAX_PLAYERS; ++i)
	    {
	      if( ! strcasecmp(slots[i].name, str) )
		{
		  packet.infos.chat.which=i;
		}
	    }
	  if( packet.infos.chat.which==BROADCAST )
	    {
	      fprintf(stderr, "User %s doesn't exist.\n", str);
	      return;
	    }
	  str = strtok(NULL, " ");
	  strcpy(packet.infos.chat.mesg, str);
	  Net_sendpacket(&packet, Net_getmainsock());	  
	}
      break;
    case 'g': //Change Netsettings nbWins
      if( serverstate == preGameState && isConnected  )
	{

	  if( slots[me].isMaster )
	    {
	      packet.which=me;
	      packet.type=ACTION;
	      packet.infos.action.type=CHGENBWINS;
	      str = strtok(params, " ");
	      //str is new settings
	      packet.infos.action.which = strtol(str, (char**) NULL, 10);
	      Net_sendpacket(&packet, Net_getmainsock());	  
	    } else {
	      fprintf(stderr,"\nYour are not allowed to change game settings, u must be Game Master\n");
	    }
	}
      break;
    case 't': //Change Netsettings timeout
      if( serverstate == preGameState && isConnected  )
	{
	  
	  if( slots[me].isMaster )
	    {
	      packet.which=me;
	      packet.type=ACTION;
	      packet.infos.action.type=CHGETIMEOUT;
	      str = strtok(params, " ");
	      //str is new settings
	      packet.infos.action.which = strtol(str, (char**) NULL, 10);
	      if( packet.infos.action.which < 60 && packet.infos.action.which > 0 )
		{
		  Net_sendpacket(&packet, Net_getmainsock());
		} else {
	      fprintf(stderr,"\nValue is out of bound..\n");		  
		}	  
	    } else {
	      fprintf(stderr,"\nYour are not allowed to change game settings, u must be Game Master\n");
	    }
	}
      break;
      case 'v': //Change Netset speed
      if( serverstate == preGameState && isConnected  )
	{

	  if( slots[me].isMaster )
	    {
	      packet.which=me;
	      packet.type=ACTION;
	      packet.infos.action.type=CHGESPEED;
	      str = strtok(params, " ");
	      //str is new settings
	      packet.infos.action.which = strtol(str, (char**) NULL, 10);
	      Net_sendpacket(&packet, Net_getmainsock());	  
	    } else {
	      fprintf(stderr,"\nYour are not allowed to change game settings, u must be Game Master\n");
	    }
	}
      break;
      case 'z': //Change Netsettings nbWins
      if( serverstate == preGameState && isConnected  )
	{

	  if( slots[me].isMaster )
	    {
	      packet.which=me;
	      packet.type=ACTION;
	      packet.infos.action.type=CHGESIZE;
	      str = strtok(params, " ");
	      //str is new settings
	      packet.infos.action.which = strtol(str, (char**) NULL, 10);
	      Net_sendpacket(&packet, Net_getmainsock());	  
	    } else {
	      fprintf(stderr,"\nYour are not allowed to change game settings, u must be Game Master\n");
	    }
	}
      break;
      case 'e': //Change Netsettings nbWins
      if( serverstate == preGameState && isConnected  )
	{

	  if( slots[me].isMaster )
	    {
	      packet.which=me;
	      packet.type=ACTION;
	      packet.infos.action.type=CHGEERASE;
	      str = strtok(params, " ");
	      //str is new settings
	      packet.infos.action.which = strtol(str, (char**) NULL, 10);
	      Net_sendpacket(&packet, Net_getmainsock());	  
	    } else {
	      fprintf(stderr,"\nYour are not allowed to change game settings, u must be Game Master\n");
	    }
	}
      break;
    case 'h': //print help
      insert_wtext(pregametext, "s : start a game\nw : wipser a player\ng : change nbwins settings\nt : change timeout ( minutes)\nv : change game speed ( 0 to 3 )\nz : change arena size ( 0 to 4 )\ne : change erased Crashed\n", 0);
      break;
    default:
      insert_wtext(pregametext, "unknown command\ntype /h for hep\n", 0);
      break;
    }
}

void
keyboardreadingreturn()
{
  char command[MAX_CHARS]="", params[MAX_CHARS]="";
  Packet packet;

  //We have our buffer.
  sscanf(buff, "/%[A-Za-z] %[A-Za-z0-9 ]", command, params);
  if( strlen(command) > 0 )
    {
      handlecommand(command, params);
      strcpy(buff, "");
      //fprintf(stderr, "\ncommand: %s\nparams: %s\n", command, params);
    } else {
      fprintf(stderr, "\nsend chat: %s\n", buff);
      packet.which=me;
      packet.type=CHAT;
      packet.infos.chat.which=BROADCAST;
      strcpy(packet.infos.chat.mesg, buff);
      Net_sendpacket(&packet, Net_getmainsock());
    }
  strcpy(buff, "");
}

char*
getInputEntry()
{
  return buff;
}

void
cleankeyreading()
{
  printf("Leaving keyboardreadingCallbacks...\n");
  //call a function at return...
  keyboardreadingreturn();
}

void
keyboardreadingpressed(int k)
{
  fprintf(stderr, "%c", k);
}

void keyreading(int k, int unicode, int x, int y)
{
  
  if( nbreads > MAX_CHARS )
    {
      //leave, no space left...
      restoreCallbacks();
    } else {
      switch( k )
	{
	case 13://return
	  //finished, we leave.
	  buff[nbreads++]='\0';
	  keyboardreadingreturn();
	  restoreCallbacks();
	  break;
	case 27://escape
	  //???
	  break;
	case SDLK_BACKSPACE://backspace
	  if( nbreads > 0 )
	    {
	      nbreads--;
	      buff[nbreads]='\0';
	      keyboardreadingpressed(unicode);
	    }
	  break;
	default:
	  if( unicode < 0x80 && unicode > 0 )
	    {
	      buff[nbreads++]=(char)unicode;
	      buff[nbreads]='\0';
	    }
	  //else
	  //  buff[nbreads++]=k;
	  //call a function after key is pressed ( example: trigger. )
	  keyboardreadingpressed(unicode);
	  break;
	}
    }
}

void displaykeyboardreading() {
  displayPregame();
  SystemSwapBuffers();
}

void idlekeyboardreading()
{  
  SystemPostRedisplay();
  if( isConnected && Net_checksocks() )
    {
      handleServer();
    }
}

callbacks keyboardreadingCallbacks = {
  //displaykeyboardreading, idlekeyboardreading, keyreading, initkeyreading,
  displayPregame, idlekeyboardreading, keyreading, initkeyreading,
  cleankeyreading, NULL, NULL, NULL
};
