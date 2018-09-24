//
// q2admin
//
// zb_init.c
//
// copyright 2000 Shane Powell
//

#include "g_local.h"

game_import_t	gi;
game_export_t	globals;
game_export_t	*dllglobals;

cvar_t  *gamedir, *maxclients, *logfile, *rconpassword, *port, *q2admintxt;

qboolean quake2dirsupport = TRUE;

char dllname[256];

qboolean dllloaded = FALSE;

qboolean zbotdetect = TRUE;


qboolean nameChangeFloodProtect = FALSE;
int nameChangeFloodProtectNum = 5;
int nameChangeFloodProtectSec = 2;
int nameChangeFloodProtectSilence = 10;
char nameChangeFloodProtectMsg[256];

qboolean skinChangeFloodProtect = FALSE;
int skinChangeFloodProtectNum = 5;
int skinChangeFloodProtectSec = 2;
int skinChangeFloodProtectSilence = 10;
char skinChangeFloodProtectMsg[256];

struct chatflood_s  floodinfo = 
{
  FALSE, 5, 2, 10
};
char chatFloodProtectMsg[256];


qboolean disconnectuser = TRUE;
qboolean displayzbotuser = TRUE;
int numofdisplays = 4;
char zbotuserdisplay[256];
char timescaleuserdisplay[256];
qboolean displaynamechange = TRUE;
qboolean disconnectuserimpulse = FALSE;
int maximpulses = 1;

char moddir[256];

qboolean displayimpulses = FALSE;
qboolean play_team_enable = FALSE;
qboolean play_all_enable = FALSE;
qboolean play_person_enable = FALSE;
qboolean say_person_enable = FALSE;
qboolean say_group_enable = FALSE;
qboolean extendedsay_enable = FALSE;
qboolean spawnentities_enable = FALSE;
qboolean spawnentities_internal_enable = FALSE;
qboolean vote_enable = FALSE;

qboolean consolechat_disable = FALSE;


qboolean gamemaptomap = FALSE;


qboolean banOnConnect = TRUE;

qboolean lockDownServer = FALSE;


int randomwaitreporttime = 55;

char *zbotversion = "Q2Admin Version 1.8\n";
qboolean serverinfoenable = TRUE;
qboolean serverlogfile = FALSE;

char zbotmotd[256];
char motd[4096];

int maxrateallowed = 0;
int minrateallowed = 0;
int maxfpsallowed = 0;
int minfpsallowed = 0;

char buffer[0x10000];
char buffer2[256];

char adminpassword[256];

char customServerCmd[256];
char customClientCmd[256];
char customClientCmdConnect[256];
char customServerCmdConnect[256];

char clientVoteCommand[256];

char reconnect_address[256] = { 0 };
int reconnect_time = 60;
int reconnect_checklevel = 0;

int logfilecheckcount;

proxyinfo_t *proxyinfo;
proxyinfo_t *proxyinfoBase;
proxyreconnectinfo_t *reconnectproxyinfo;



reconnect_info* reconnectlist;
retrylist_info* retrylist;
int maxReconnectList = 0;
int maxretryList = 0;



int lframenum;
float	ltime;


int proxy_bwproxy = 1;
int proxy_nitro2 = 1;


int q2adminrunmode = 100;
int maxclientsperframe = 100;
int framesperprocess = 0;


qboolean cl_pitchspeed_display = TRUE;
qboolean cl_pitchspeed_enable = FALSE;
qboolean cl_pitchspeed_kick = FALSE;
char cl_pitchspeed_kickmsg[256];

qboolean filternonprintabletext = FALSE;


char lockoutmsg[256];





char	com_token[MAX_TOKEN_CHARS];

/*
==============
COM_Parse

Parse a token out of a string
==============
*/
char *COM_Parse (char **data_p, char **command_p)
{
	int		c;
	int		len;
	char	*data;

	data = *data_p;
	len = 0;
	com_token[0] = 0;
	
	if (!data)
	{
		*data_p = NULL;
		return "";
	}
		
// skip whitespace
skipwhite:
	while ( (c = *data) <= ' ')
	{
		if (c == 0)
		{
			*data_p = NULL;
			return "";
		}
		data++;
	}
	
// skip // comments
	if (c=='/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;
		goto skipwhite;
	}

// handle quoted strings specially
	if (c == '\"')
	{
		data++;

		if(command_p)
		{
			*command_p = data;
		}

		while (1)
		{
			c = *data++;
			if (c=='\"' || !c)
			{
				com_token[len] = 0;
				*data_p = data;
				return com_token;
			}
			if (len < MAX_TOKEN_CHARS)
			{
				com_token[len] = c;
				len++;
			}
		}
	}

	if(command_p)
	{
		*command_p = data;
	}

// parse a regular word
	do
	{
		if (len < MAX_TOKEN_CHARS)
		{
			com_token[len] = c;
			len++;
		}
		data++;
		c = *data;
	} while (c>32);

	if (len == MAX_TOKEN_CHARS)
	{
//		Com_Printf ("Token exceeded %i chars, discarded.\n", MAX_TOKEN_CHARS);
		len = 0;
	}
	com_token[len] = 0;

	*data_p = data;
	return com_token;
}







void InitGame (void)
{
	int i;

  INITPERFORMANCE(1);
  INITPERFORMANCE(2);

	gi.dprintf (zbotversion);

	if(!dllloaded) return;

  if(q2adminrunmode < 100)
  {
  	gi.dprintf ("(Q2Admin runlevel %d)\n", q2adminrunmode);
  }

  if(q2adminrunmode == 0)
  {
	  dllglobals->Init();
	  copyDllInfo();
    return;
  }

  STARTPERFORMANCE(1);
  STARTPERFORMANCE(2);
	dllglobals->Init();
  STOPPERFORMANCE(2, "mod->InitGame", 0, NULL);

	copyDllInfo();

	maxclients = gi.cvar ("maxclients", "4", 0);
	logfile = gi.cvar ("logfile", "0", 0);
  rconpassword = gi.cvar("rcon_password", "", 0);
	proxyinfoBase = gi.TagMalloc ((maxclients->value + 1) * sizeof(proxyinfo_t), TAG_GAME);
	q2a_memset(proxyinfoBase, 0x0, (maxclients->value + 1) * sizeof(proxyinfo_t));
	proxyinfo = proxyinfoBase + 1;
  proxyinfo[-1].inuse = 1;

  reconnectproxyinfo = gi.TagMalloc (maxclients->value  * sizeof(proxyreconnectinfo_t), TAG_GAME);
  q2a_memset(reconnectproxyinfo, 0x0, maxclients->value * sizeof(proxyreconnectinfo_t));

  reconnectlist = (reconnect_info *)gi.TagMalloc (maxclients->value * sizeof(reconnect_info), TAG_GAME);
  maxReconnectList = 0;

  retrylist = (retrylist_info *)gi.TagMalloc (maxclients->value * sizeof(retrylist_info), TAG_GAME);
  maxretryList = 0;

  logEvent(LT_SERVERINIT, 0, NULL, NULL, 0, 0.0);

  banhead = NULL;
	motd[0] = 0;

  for(i = -1; i < maxclients->value; i++)
  {

		proxyinfo[i].inuse = 0;
		proxyinfo[i].admin = 0;
		proxyinfo[i].clientcommand = 0;
		proxyinfo[i].stuffFile = 0;
		proxyinfo[i].floodinfo.chatFloodProtect = FALSE;
		proxyinfo[i].impulsesgenerated = 0;
		proxyinfo[i].retries = 0;
		proxyinfo[i].rbotretries = 0;
		proxyinfo[i].charindex = 0;
		proxyinfo[i].teststr[0] = 0;
    proxyinfo[i].cl_pitchspeed = 0;
    proxyinfo[i].votescast = 0;
    proxyinfo[i].votetimeout = 0;

		removeClientCommands(i);
  }

  STOPPERFORMANCE(1, "q2admin->InitGame", 0, NULL);
}


void SpawnEntities (char *mapname, char *entities, char *spawnpoint)
{
  int len, currentlen;
  FILE *motdptr;
  int i;
	char *backupentities = entities;
  INITPERFORMANCE(1);
  INITPERFORMANCE(2);

	if(!dllloaded) return;

  if(q2adminrunmode == 0)
  {
    dllglobals->SpawnEntities(mapname, backupentities, spawnpoint);
	  copyDllInfo();
    return;
  }


  STARTPERFORMANCE(1);

//	q2a_memset(proxyinfoBase, 0x0, (maxclients->value + 1) * sizeof(proxyinfo_t));

  for(i = -1; i < maxclients->value; i++)
  {

		if(i < 0 || proxyinfo[i].inuse == 0)
		{
//			proxyinfo[i].inuse = 0;
			proxyinfo[i].admin = 0;
			proxyinfo[i].clientcommand = 0;
			proxyinfo[i].floodinfo.chatFloodProtect = FALSE;
			proxyinfo[i].stuffFile = 0;
		}
		else
		{
			proxyinfo[i].clientcommand &= (LEVELCHANGE_KEEP);
		}

		proxyinfo[i].impulsesgenerated = 0;
		proxyinfo[i].rbotretries = 0;
		proxyinfo[i].retries = 0;
		proxyinfo[i].charindex = 0;
		proxyinfo[i].teststr[0] = 0;
    proxyinfo[i].cl_pitchspeed = 0;
    proxyinfo[i].votescast = 0;
    proxyinfo[i].votetimeout = 0;
		removeClientCommands(i);
  }

  proxyinfo[-1].inuse = 1;
  freeBanLists();
  freeLRconLists();
  freeFloodLists();
	freeVoteLists();
	freeDisableLists();


	motd[0] = 0;
	if(zbotmotd[0])
	{
		motdptr = fopen(zbotmotd, "rt");

		if(!motdptr)
		{
			zbotmotd[0] = 0;
		}
		else
		{

			len = 0;
			while(fgets(buffer, 256, motdptr))
			{
				currentlen = q2a_strlen(buffer);

				if(len + currentlen > sizeof(motd))
				{
					break;
				}

				len += currentlen;
				q2a_strcat(motd, buffer);
			}

			fclose(motdptr);
		}
	}

  zbot_teststring1[7] = zbot_teststring_test1[7] = '0' + (int)(9.9 * random());
  zbot_teststring1[8] = zbot_teststring_test1[8] = '0' + (int)(9.9 * random());
  zbot_teststring_test2[3] = '0' + (int)(9.9 * random());
  zbot_teststring_test2[4] = '0' + (int)(9.9 * random());
  zbot_testchar1 = '0' + (int)(9.9 * random());
  zbot_testchar2 = '0' + (int)(9.9 * random());

	if(spawnentities_enable)
	{
    q2a_strcpy(buffer, moddir);
    q2a_strcat(buffer, "/q2adminmaps/");
    q2a_strcat(buffer, mapname);
    q2a_strcat(buffer, ".q2aspawn");
    ReadSpawnFile(buffer, TRUE);

		// parse out all the turned off entities...
		while (1)
		{
			char *com_tok;
			char *classnamepos;
			char keyname[256];

			// parse the opening brace	
			com_tok = COM_Parse (&entities, NULL);
			if (!entities)
				break;
			if (com_tok[0] != '{')
				break;

			// go through all the dictionary pairs
			while (1)
			{	
			// parse key
				com_tok = COM_Parse (&entities, &classnamepos);
				if (com_tok[0] == '}')
					break;
				if (!entities)
					break;

				q2a_strncpy (keyname, com_tok, sizeof(keyname)-1);
				
			// parse value	
				com_tok = COM_Parse (&entities, NULL);
				if (!entities)
					break;

				if (com_tok[0] == '}') // {
					break;

				if(!Q_stricmp("classname", keyname) && checkDisabledEntities(com_tok))
				{
					classnamepos[0] = '_';  // change the 'classname' entry to '_lassname', this makes the q2 code ingore it.
					
					// side-effect: it may cause error messages on the console screen depending on the mod...
				}
			}
		}

    freeOneLevelSpawnLists();
	}

  STARTPERFORMANCE(2);
  dllglobals->SpawnEntities(mapname, backupentities, spawnpoint);
  STOPPERFORMANCE(2, "mod->SpawnEntities", 0, NULL);

  copyDllInfo();

  readBanLists();
  readLRconLists();
  readFloodLists();
	readVoteLists();
	readDisableLists();

  // exec the map cfg file...
  q2a_strcpy(buffer, "exec ");
  q2a_strcat(buffer, mapname);
  q2a_strcat(buffer, "-post.cfg\n");
  gi.AddCommandString(buffer);

  STOPPERFORMANCE(1, "q2admin->SpawnEntities", 0, NULL);
}



qboolean UpdateInternalClientInfo(int client, edict_t *ent, char *userinfo)
{
	char *ip = Info_ValueForKey(userinfo, "ip");

	if(ip && *ip)
	{
    int i, num;

		q2a_strcpy(proxyinfo[client].ipaddress, ip);

    for(i = 0; i < 4; i++)
    {
      num = q2a_atoi(ip);

      if(num > 255)
      {
        num = 255;
      }

      proxyinfo[client].ipaddressBinary[i] = num;

      while(isdigit(*ip))
      {
        ip++;
      }

      if(*ip == '.')
      {
        ip++;
      }
      else
      {
        break;
      }
    }
	}

	ip = Info_ValueForKey(userinfo, "Nitro2");

	if(ip && *ip)
	{														
	  if(proxy_nitro2)
		{
			proxyinfo[client].clientcommand	|= CCMD_NITRO2PROXY;
		}
		else
		{
			return TRUE;
		}
	}

	ip = Info_ValueForKey(userinfo, "bwproxy");

	if(ip && *ip)
	{														
		if(proxy_bwproxy)
		{
			proxyinfo[client].clientcommand	|= CCMD_NITRO2PROXY;
		}
		else
		{
			return TRUE;
		}
	}

	return FALSE;
}



qboolean checkReconnectUserInfoSame(char *userinfo1, char *userinfo2)
{
  if(reconnect_checklevel)
  {
	  char *cp1 = Info_ValueForKey(userinfo1, "ip");
	  char *cp2 = Info_ValueForKey(userinfo2, "ip");
    char *bp = cp1;

    while(*bp && *bp != ':')
    {
      bp++;
    }

    *bp = 0;

    bp = cp2;

    while(*bp && *bp != ':')
    {
      bp++;
    }

    *bp = 0;

    if(q2a_strcmp(cp1, cp2) != 0)
    {
      return FALSE;
    }

	  cp1 = Info_ValueForKey(userinfo1, "name");
	  cp2 = Info_ValueForKey(userinfo2, "name");

    if(q2a_strcmp(cp1, cp2) != 0)
    {
      return FALSE;
    }

    cp1 = Info_ValueForKey(userinfo1, "skin");
	  cp2 = Info_ValueForKey(userinfo2, "skin");

    return (q2a_strcmp(cp1, cp2) == 0);
  }

  return (q2a_strcmp(userinfo1, userinfo2) == 0);
}



qboolean checkReconnectList(char *username)
{
  int i;

  for(i = 0; i < maxclients->value; i++)
  {
    if(reconnectproxyinfo[i].inuse && q2a_strcmp(reconnectproxyinfo[i].name, username) == 0)
    {
      reconnectproxyinfo[i].inuse = FALSE;
      return FALSE;
    }
  }

  return TRUE;
}





qboolean ClientConnect (edict_t *ent, char *userinfo)
{
	int client;
  char *s;

	qboolean ret;

  INITPERFORMANCE(1);
  INITPERFORMANCE(2);

	if(!dllloaded) return FALSE;

  if(q2adminrunmode == 0)
  {
    ret = dllglobals->ClientConnect(ent, userinfo);
	  copyDllInfo();
    return ret;
  }

  STARTPERFORMANCE(1);

  // allways clearout just in case there isn't any clients (therefore runframe doesn't get called)
  if(maxReconnectList)
  {
    int i;

    for(i = 0; i < maxReconnectList; i++)
    {
      if(reconnectlist[i].reconnecttimeout < ltime)
      {
        // wipe out the retry list if it's the only one pointing to it.
        int j;

        for(j = 0; j < maxReconnectList; j++)
        {
          if(j != i && reconnectlist[j].retrylistidx == reconnectlist[i].retrylistidx)
          {
            break;
          }
        }

        if(j >= maxReconnectList)
        {
          if(reconnectlist[i].retrylistidx + 1 < maxretryList)
          {
            q2a_memmove(&(retrylist[reconnectlist[i].retrylistidx]), &(retrylist[reconnectlist[i].retrylistidx + 1]), (maxretryList - (reconnectlist[i].retrylistidx + 1)) * sizeof(retrylist_info));
          }
          maxretryList--;
        }
        
        // wipe out the reconnect list entry
        if(i + 1 < maxReconnectList)
        {
          q2a_memmove(&(reconnectlist[i]), &(reconnectlist[i + 1]), (maxReconnectList - (i + 1)) * sizeof(reconnect_info));
          i--;
        }
        maxReconnectList--;
      }
    }
  }
  
  client = getEntOffset(ent) - 1;
	
  if(proxyinfo[client].baninfo)
  {
		if(proxyinfo[client].baninfo->numberofconnects)
		{
			proxyinfo[client].baninfo->numberofconnects--;
		}

    proxyinfo[client].baninfo = NULL;
  }

  proxyinfo[client].inuse = 0;
	proxyinfo[client].admin = 0;
	proxyinfo[client].clientcommand = 0;
  proxyinfo[client].retries = 0;
	proxyinfo[client].rbotretries = 0;
	proxyinfo[client].charindex = 0;
	proxyinfo[client].ipaddress[0] = 0;
	proxyinfo[client].name[0] = 0;
	proxyinfo[client].skin[0] = 0;
  proxyinfo[client].ipaddressBinary[0] = 0;
  proxyinfo[client].ipaddressBinary[1] = 0;
  proxyinfo[client].ipaddressBinary[2] = 0;
  proxyinfo[client].ipaddressBinary[3] = 0;
  proxyinfo[client].stuffFile = 0;
  proxyinfo[client].impulsesgenerated = 0;
  proxyinfo[client].floodinfo.chatFloodProtect = FALSE;
  proxyinfo[client].cl_pitchspeed = 0;
  proxyinfo[client].votescast = 0;
  proxyinfo[client].votetimeout = 0;
  removeClientCommands(client);

	ret = 1;

	if(client < maxclients->value)
	{
		if(UpdateInternalClientInfo(client, ent, userinfo))
		{
			sprintf(buffer, zbotuserdisplay, proxyinfo[client].name);
			currentBanMsg = buffer;
			logEvent(LT_BAN, client, ent, currentBanMsg, 0, 0.0);

			if(!banOnConnect)
			{
				ret = 0;
			}
			else
			{
				proxyinfo[client].clientcommand |= CCMD_BANNED;
				q2a_strcpy(proxyinfo[client].buffer, currentBanMsg);
			}
		}
	}

	// check for malformed or illegal info strings
	if (!Info_Validate(userinfo))
	{
		q2a_strcpy (userinfo, "\\name\\badinfo\\skin\\male/grunt");
	}

  q2a_strcpy(proxyinfo[client].userinfo, userinfo);

	// set name
	s = Info_ValueForKey (userinfo, "name");
	q2a_strncpy (proxyinfo[client].name, s, sizeof(proxyinfo[client].name)-1);

	s = Info_ValueForKey (userinfo, "skin");
	q2a_strncpy (proxyinfo[client].skin, s, sizeof(proxyinfo[client].skin)-1);

// 	q2a_strcpy(ent->client->pers.netname, proxyinfo[client].name);
	q2a_strncpy (proxyinfo[client].userinfo, userinfo, sizeof(proxyinfo[client].userinfo) - 1);

	if(lockDownServer && checkReconnectList(proxyinfo[client].name))
	{
     currentBanMsg = lockoutmsg;

    logEvent(LT_BAN, client, ent, currentBanMsg, 0, 0.0);
    gi.cprintf (NULL, PRINT_HIGH, "%s: %s\n", proxyinfo[client].name, currentBanMsg);

    if(banOnConnect)
    {
      ret = 0;
    }
    else
    {
      proxyinfo[client].clientcommand |= CCMD_BANNED;
      q2a_strcpy(proxyinfo[client].buffer, currentBanMsg);
    }
	}
	else if(checkCheckIfBanned(ent, client))
  {
    logEvent(LT_BAN, client, ent, currentBanMsg, 0, 0.0);
    gi.cprintf (NULL, PRINT_HIGH, "%s: %s\n", proxyinfo[client].name, currentBanMsg);
//      gi.cprintf(NULL, PRINT_HIGH, "%s\n", currentBanMsg);

    if(banOnConnect)
    {
      ret = 0;
    }
    else
    {
      proxyinfo[client].clientcommand |= CCMD_BANNED;
      q2a_strcpy(proxyinfo[client].buffer, currentBanMsg);
    }
  }
  else if(ret && !(proxyinfo[client].clientcommand & CCMD_BANNED))
  {
    qboolean doConnect = TRUE;

    // is reconnect_address set?
    if(!isBlank(reconnect_address))
    {
	    char *ip = Info_ValueForKey(userinfo, "ip");
      char *bp = ip;
      int i;

      while(*bp && *bp != ':')
      {
        bp++;
      }

      *bp = 0;


      // check to see if they are in the reconnect list?
      for(i = 0; i < maxReconnectList; i++)
      {
        if(checkReconnectUserInfoSame(userinfo, reconnectlist[i].userinfo))
        {
          // found a match...
          break;
        }
        else
        {
          // check if the same IP and delete... this stops proxies from reconnecting...
          char *reconnectip = Info_ValueForKey(reconnectlist[i].userinfo, "ip");
          bp = reconnectip;

          while(*bp && *bp != ':')
          {
            bp++;
          }

          *bp = 0;
  
          if(strcmp(ip, reconnectip) == 0)
          {
            // remove from list and continue with connect
            if(i + 1 < maxReconnectList)
            {
              q2a_memmove(&(reconnectlist[i]), &(reconnectlist[i + 1]), (maxReconnectList - (i + 1)) * sizeof(reconnect_info));
              i--;
            }
            maxReconnectList--;
          }
        }
      }

      if(i < maxReconnectList)
      {
        int j;

        // remove the retry list entry if needed...
        for(j = 0; j < maxReconnectList; j++)
        {
          if(j != i && reconnectlist[j].retrylistidx == reconnectlist[i].retrylistidx)
          {
            break;
          }
        }

        if(j >= maxReconnectList)
        {
          if(reconnectlist[i].retrylistidx + 1 < maxretryList)
          {
            q2a_memmove(&(retrylist[reconnectlist[i].retrylistidx]), &(retrylist[reconnectlist[i].retrylistidx + 1]), (maxretryList - (reconnectlist[i].retrylistidx + 1)) * sizeof(retrylist_info));
          }
          maxretryList--;
        }

        // remove from list and continue with connect
        if(i + 1 < maxReconnectList)
        {
          q2a_memmove(&(reconnectlist[i]), &(reconnectlist[i + 1]), (maxReconnectList - (i + 1)) * sizeof(reconnect_info));
        }
        maxReconnectList--;
      }
      else
      {
        // force a reconnect and exit...

        doConnect = FALSE;
        proxyinfo[client].clientcommand |= CCMD_RECONNECT;
      }
    }


    if(doConnect)
    {
      STARTPERFORMANCE(2);
      ret = dllglobals->ClientConnect(ent, userinfo);
      STOPPERFORMANCE(2, "mod->ClientConnect", client, ent);

      copyDllInfo();
    }
  }

  if(ret)
  {
    logEvent(LT_CLIENTCONNECT, client, ent, NULL, 0, 0.0);
  }

  STOPPERFORMANCE(1, "q2admin->ClientConnect", client, ent);
	return ret;
}


qboolean checkForNameChange(int client, edict_t *ent, char *userinfo)
{
 	char *s = Info_ValueForKey (userinfo, "name");
  char oldname[sizeof(proxyinfo[client].name)];
  char newname[sizeof(proxyinfo[client].name)];

 	q2a_strncpy (newname, s, sizeof(newname) - 1);
  newname[sizeof(newname) - 1] = 0;

	if(proxyinfo[client].name[0] == 0)
	{
  	q2a_strcpy (proxyinfo[client].name, newname);
	}
	else if(q2a_strcmp(proxyinfo[client].name, newname) != 0)
  {
		// check if ratbot detect?
		if(q2a_strcmp(newname, RATBOT_CHANGENAMETEST) == 0)
		{
      removeClientCommand(client, QCMD_TESTRATBOT4);
			proxyinfo[client].clientcommand &= ~CCMD_RATBOTDETECTNAME;
			proxyinfo[client].clientcommand |= CCMD_RBOTCLEAR;

			// ok not a ratbot.. turn off detection
//			addCmdQueue(client, QCMD_CHANGENAME, 0, 0, 0);
			return FALSE;
		}

		// check for flooding..
    if(nameChangeFloodProtect)
    {
      if(proxyinfo[client].clientcommand & CCMD_NCSILENCE)
      {
        if(proxyinfo[client].namechangetimeout < ltime)
        {
          proxyinfo[client].clientcommand &= ~CCMD_NCSILENCE;
        }
        else
        {
          int secleft = (int)(proxyinfo[client].namechangetimeout - ltime) + 1;

//      		q2a_strcpy(ent->client->pers.netname, proxyinfo[client].name);
          addCmdQueue(client, QCMD_CHANGENAME, 0, 0, 0);

        	gi.cprintf (ent, PRINT_HIGH, "%d seconds of name change silence left.\n", secleft);
          return FALSE;
        }
      }
    }

    q2a_strcpy(oldname, proxyinfo[client].name);
  	q2a_strcpy (proxyinfo[client].name, newname);

    if(checkCheckIfBanned(ent, client))
    {
      logEvent(LT_BAN, client, ent, currentBanMsg, 0, 0.0);
		  q2a_strcpy(proxyinfo[client].name, oldname);

      // display ban msg to user..
//    	gi.cprintf (ent, PRINT_HIGH, "Can't change name to a banned name\n");
    	gi.cprintf (ent, PRINT_HIGH, "%s\n", currentBanMsg);
//  		q2a_strcpy(ent->client->pers.netname, proxyinfo[client].name);

      if(kickOnNameChange)
      {
        addCmdQueue(client, QCMD_DISCONNECT, 1, 0, currentBanMsg);
      }
      else
      {
        addCmdQueue(client, QCMD_CHANGENAME, 0, 0, 0);
      }
    }
    else
    {
      logEvent(LT_NAMECHANGE, client, ent, oldname, 0, 0.0);

		  if(displaynamechange)
		  {
			  gi.bprintf (PRINT_HIGH, "%s changed name to %s\n", oldname, proxyinfo[client].name);
		  }

      if(nameChangeFloodProtect)
      {
        if(proxyinfo[client].namechangetimeout < ltime)
        {
          proxyinfo[client].namechangetimeout = ltime + nameChangeFloodProtectSec;
          proxyinfo[client].namechangecount = 0;
        }
        else
        {
          if(proxyinfo[client].namechangecount >= nameChangeFloodProtectNum)
          {
//      		  q2a_strcpy(ent->client->pers.netname, proxyinfo[client].name);
            sprintf(buffer, nameChangeFloodProtectMsg, proxyinfo[client].name);
			      gi.bprintf (PRINT_HIGH, "%s\n", buffer);

            if(nameChangeFloodProtectSilence == 0)
            {
    	        addCmdQueue(client, QCMD_DISCONNECT, 0, 0, nameChangeFloodProtectMsg);
            }
            else
            {
              proxyinfo[client].namechangetimeout = ltime + nameChangeFloodProtectSilence;
              proxyinfo[client].clientcommand |= CCMD_NCSILENCE;
            }
            return FALSE;
          }

          proxyinfo[client].namechangecount++;
        }
      }
    }
	}

  return TRUE;
}



qboolean checkForSkinChange(int client, edict_t *ent, char *userinfo)
{
 	char *s = Info_ValueForKey (userinfo, "skin");
  char oldskin[sizeof(proxyinfo[client].skin)];
  char newskin[sizeof(proxyinfo[client].skin)];

 	q2a_strncpy (newskin, s, sizeof(newskin) - 1);
  newskin[sizeof(newskin) - 1] = 0;

	if(proxyinfo[client].skin[0] == 0)
	{
  	q2a_strcpy (proxyinfo[client].skin, newskin);
	}
	else if(q2a_strcmp(proxyinfo[client].skin, newskin) != 0)
  {
		// check for flooding..
    if(skinChangeFloodProtect)
    {
      if(proxyinfo[client].clientcommand & CCMD_SCSILENCE)
      {
        if(proxyinfo[client].skinchangetimeout < ltime)
        {
          proxyinfo[client].clientcommand &= ~CCMD_SCSILENCE;
        }
        else
        {
          int secleft = (int)(proxyinfo[client].skinchangetimeout - ltime) + 1;

//      		q2a_strcpy(ent->client->pers.netskin, proxyinfo[client].skin);
          addCmdQueue(client, QCMD_CHANGESKIN, 0, 0, 0);

        	gi.cprintf (ent, PRINT_HIGH, "%d seconds of skin change silence left.\n", secleft);
          return FALSE;
        }
      }
    }

    q2a_strcpy(oldskin, proxyinfo[client].skin);
  	q2a_strcpy (proxyinfo[client].skin, newskin);

    logEvent(LT_SKINCHANGE, client, ent, oldskin, 0, 0.0);

    if(skinChangeFloodProtect)
    {
      if(proxyinfo[client].skinchangetimeout < ltime)
      {
        proxyinfo[client].skinchangetimeout = ltime + skinChangeFloodProtectSec;
        proxyinfo[client].skinchangecount = 0;
      }
      else
      {
        if(proxyinfo[client].skinchangecount >= skinChangeFloodProtectNum)
        {
          sprintf(buffer, skinChangeFloodProtectMsg, proxyinfo[client].name);
			    gi.bprintf (PRINT_HIGH, "%s\n", buffer);

          if(skinChangeFloodProtectSilence == 0)
          {
    	      addCmdQueue(client, QCMD_DISCONNECT, 0, 0, skinChangeFloodProtectMsg);
          }
          else
          {
            proxyinfo[client].skinchangetimeout = ltime + skinChangeFloodProtectSilence;
            proxyinfo[client].clientcommand |= CCMD_SCSILENCE;
          }
          return FALSE;
        }

        proxyinfo[client].skinchangecount++;
      }
    }
	}

  return TRUE;
}


void ClientUserinfoChanged (edict_t *ent, char *userinfo)
{
	int client;
  qboolean passon;

  INITPERFORMANCE(1);
  INITPERFORMANCE(2);

	if(!dllloaded) return;

  if(q2adminrunmode == 0)
  {
    dllglobals->ClientUserinfoChanged(ent, userinfo);
	  copyDllInfo();
    return;
  }

  STARTPERFORMANCE(1);

	client = getEntOffset(ent) - 1;

  passon = checkForNameChange(client, ent, userinfo);
  if(!checkForSkinChange(client, ent, userinfo))
  {
	  passon = FALSE;
  }

  if(passon && !(proxyinfo[client].clientcommand & BANCHECK))
  {
    STARTPERFORMANCE(2);
    dllglobals->ClientUserinfoChanged(ent, userinfo);
    STOPPERFORMANCE(2, "mod->ClientUserinfoChanged", client, ent);

	  copyDllInfo();
  }

  proxyinfo[client].rate = q2a_atoi(Info_ValueForKey(userinfo, "rate"));

  if(maxrateallowed && proxyinfo[client].rate > maxrateallowed)
  {
    addCmdQueue(client, QCMD_CLIPTOMAXRATE, 0, 0, 0);
  }

  if(minrateallowed && proxyinfo[client].rate < minrateallowed)
  {
    addCmdQueue(client, QCMD_CLIPTOMINRATE, 0, 0, 0);
  }

  proxyinfo[client].maxfps = q2a_atoi(Info_ValueForKey(userinfo, "cl_maxfps"));

  if(maxfpsallowed)
  {
    if(proxyinfo[client].maxfps == 0)
    {
      addCmdQueue(client, QCMD_SETUPMAXFPS, 0, 0, 0);
    }
    else if(proxyinfo[client].maxfps > maxfpsallowed)
    {
      addCmdQueue(client, QCMD_SETMAXFPS, 0, 0, 0);
    }
  }

	if(minfpsallowed)
	{
    if(proxyinfo[client].maxfps == 0)
    {
      addCmdQueue(client, QCMD_SETUPMAXFPS, 0, 0, 0);
    }
    else if(proxyinfo[client].maxfps < minfpsallowed)
    {
      addCmdQueue(client, QCMD_SETMINFPS, 0, 0, 0);
    }
	}

  
  if(cl_pitchspeed_enable)
  {
    int newps = q2a_atoi(Info_ValueForKey(userinfo, "cl_pitchspeed"));
    
    if(newps == 0)
    {
      addCmdQueue(client, QCMD_SETUPCL_PITCHSPEED, 0, 0, 0);
    }
    else if(proxyinfo[client].cl_pitchspeed == 0)
    {
      proxyinfo[client].cl_pitchspeed = newps;
    }
    else
    {
      proxyinfo[client].cl_pitchspeed = newps;

      // ingore changes to 150 because action quake2 generates them itself...

      if(proxyinfo[client].cl_pitchspeed != 150)
      {
        logEvent(LT_ZBOT, client, ent, NULL, -7, 0.0);

        if(cl_pitchspeed_display)
        {
			    gi.bprintf (PRINT_HIGH, "%s chanaged cl_pitchspeed to %d\n", proxyinfo[client].name, newps);
        }

        if(cl_pitchspeed_kick)
        {
          gi.cprintf (ent, PRINT_HIGH, "%s\n", cl_pitchspeed_kickmsg);

          addCmdQueue(client, QCMD_DISCONNECT, 1, 0, cl_pitchspeed_kickmsg);
        }
      }
    }
  }

  proxyinfo[client].msg = q2a_atoi(Info_ValueForKey(userinfo, "msg"));

  if(proxyinfo[client].msg)
  {
    gi.cprintf (ent, PRINT_HIGH, "Client 'msg' mode can't be changed on this server!!\n");
    addCmdQueue(client, QCMD_DISCONNECT, 1, 0, "Client 'msg' mode can't be changed on this server!!\n");
  }
  
  q2a_strcpy(proxyinfo[client].userinfo, userinfo);

  STOPPERFORMANCE(1, "q2admin->ClientUserinfoChanged", client, ent);
}





void ClientDisconnect (edict_t *ent)
{
	int client;
  INITPERFORMANCE(1);
  INITPERFORMANCE(2);

	if(!dllloaded) return;

  if(q2adminrunmode == 0)
  {
    dllglobals->ClientDisconnect(ent);
	  copyDllInfo();
    return;
  }

  STARTPERFORMANCE(1);

	client = getEntOffset(ent) - 1;
	
	if(client >= maxclients->value) return;

  if(!(proxyinfo[client].clientcommand & BANCHECK))
  {
    STARTPERFORMANCE(2);
    dllglobals->ClientDisconnect(ent);
    STOPPERFORMANCE(2, "mod->ClientDisconnect", client, ent);

	  copyDllInfo();

    // setup the reconnect info if required.
    if(lockDownServer)
    {
      int i;

      for(i = 0; i < maxclients->value; i++)
      {
        if(!reconnectproxyinfo[i].inuse)
        {
          q2a_strcpy(reconnectproxyinfo[i].name, proxyinfo[client].name);
          reconnectproxyinfo[i].inuse = TRUE;
          break;
        }
      }
    }
  }

  logEvent(LT_CLIENTDISCONNECT, client, ent, NULL, 0, 0.0);

  if(proxyinfo[client].baninfo)
  {
		if(proxyinfo[client].baninfo->numberofconnects)
		{
	    proxyinfo[client].baninfo->numberofconnects--;
		}
    proxyinfo[client].baninfo = NULL;
  }

  if(proxyinfo[client].stuffFile)
  {
    fclose(proxyinfo[client].stuffFile);
  }


  proxyinfo[client].inuse = 0;
	proxyinfo[client].admin = 0;
  proxyinfo[client].retries = 0;
	proxyinfo[client].rbotretries = 0;
	proxyinfo[client].clientcommand = 0;
	proxyinfo[client].charindex = 0;
	proxyinfo[client].ipaddress[0] = 0;
	proxyinfo[client].name[0] = 0;
	proxyinfo[client].skin[0] = 0;
  proxyinfo[client].stuffFile = 0;
  proxyinfo[client].impulsesgenerated = 0;
  proxyinfo[client].floodinfo.chatFloodProtect = FALSE;
  proxyinfo[client].votescast = 0;
  proxyinfo[client].votetimeout = 0;
  removeClientCommands(client);

  STOPPERFORMANCE(1, "q2admin->ClientDisconnect", 0, NULL);
}


void ClientBegin (edict_t *ent)
{
	int client;
	FILE *q2logfile;
  INITPERFORMANCE(1);
  INITPERFORMANCE(2);

	if(!dllloaded) return;

  if(q2adminrunmode == 0)
  {
    dllglobals->ClientBegin(ent);
	  copyDllInfo();
    return;
  }

  STARTPERFORMANCE(1);

	client = getEntOffset(ent) - 1;

  if(!(proxyinfo[client].clientcommand & BANCHECK))
  {
    STARTPERFORMANCE(2);
    dllglobals->ClientBegin(ent);
    STOPPERFORMANCE(2, "mod->ClientBegin", client, ent);

    copyDllInfo();
  }
  else
  {
    // setup ent to be valid...
		ent->client->ps.fov = 10;
  }
	
	if(client >= maxclients->value) 
  {
    STOPPERFORMANCE(1, "q2admin->ClientBegin (client >= maxclients)", client, ent);
    return;
  }

	if(!proxyinfo[client].inuse)
	{
		proxyinfo[client].admin = 0;
	}

	proxyinfo[client].inuse = 0;
	proxyinfo[client].retries = 0;
	proxyinfo[client].rbotretries = 0;
	proxyinfo[client].charindex = 0;
	proxyinfo[client].teststr[0] = 0;
	proxyinfo[client].impulsesgenerated = 0;
  proxyinfo[client].votescast = 0;
  proxyinfo[client].votetimeout = 0;

  if(proxyinfo[client].clientcommand & CCMD_RECONNECT)
  {
    addCmdQueue(client, QCMD_RECONNECT, 1, 0, NULL);
  }
  else if(proxyinfo[client].clientcommand & CCMD_BANNED)
  {
    gi.cprintf(ent, PRINT_HIGH, "%s\n", proxyinfo[client].buffer);
    addCmdQueue(client, QCMD_DISCONNECT, 1, 0, proxyinfo[client].buffer);
  }
	else if(proxyinfo[client].clientcommand & CCMD_KICKED)
	{
    addCmdQueue(client, QCMD_DISCONNECT, 1, 0, NULL);
	}
  else
  {
    addCmdQueue(client, QCMD_STARTUP, 0, 0, 0);

		if(adminpassword[0] && !proxyinfo[client].admin)
		{
			addCmdQueue(client, QCMD_TESTADMIN, 0, 0, 0);
		}

	  if(customClientCmdConnect[0] || customServerCmdConnect[0])
	  {
      addCmdQueue(client, QCMD_CONNECTCMD, 0, 0, 0);
	  }

		if(timescaledetect)
		{
	    addCmdQueue(client, QCMD_TESTTIMESCALE, 0, 0, 0);
		}

    if(zbotmotd[0])
    {
      gi.centerprintf(ent, motd);
    }

	  sprintf(buffer, "%s/qconsole.log", moddir);
	  q2logfile = fopen(buffer, "rt");
	  if(q2logfile)
	  {
  	  fseek(q2logfile, 0, SEEK_END);
	    proxyinfo[client].logfilecheckpos = ftell(q2logfile);
      fclose(q2logfile);
    }
  }

  logEvent(LT_CLIENTBEGIN, client, ent, NULL, 0, 0.0);
  STOPPERFORMANCE(1, "q2admin->ClientBegin", client, ent);
}




void WriteGame (char *filename, qboolean autosave)
{
  INITPERFORMANCE(1);

  if(!dllloaded) return;

  if(q2adminrunmode == 0)
  {
  	dllglobals->WriteGame(filename, autosave);
	  copyDllInfo();
    return;
  }

  STARTPERFORMANCE(1);

	dllglobals->WriteGame(filename, autosave);
	copyDllInfo();

  STOPPERFORMANCE(1, "q2admin->WriteGame", 0, NULL);
}


void ReadGame (char *filename)
{
  INITPERFORMANCE(1);

	if(!dllloaded) return;

  if(q2adminrunmode == 0)
  {
  	dllglobals->ReadGame(filename);
	  copyDllInfo();
    return;
  }

  STARTPERFORMANCE(1);

	dllglobals->ReadGame(filename);
	copyDllInfo();

  STOPPERFORMANCE(1, "q2admin->ReadGame", 0, NULL);
}

void WriteLevel (char *filename)
{
  INITPERFORMANCE(1);

	if(!dllloaded) return;

  if(q2adminrunmode == 0)
  {
  	dllglobals->WriteLevel(filename);
	  copyDllInfo();
    return;
  }

  STARTPERFORMANCE(1);

	dllglobals->WriteLevel(filename);
	copyDllInfo();

  STOPPERFORMANCE(1, "q2admin->WriteLevel", 0, NULL);
}

void ReadLevel (char *filename)
{
  INITPERFORMANCE(1);

	if(!dllloaded) return;

  if(q2adminrunmode == 0)
  {
  	dllglobals->ReadLevel(filename);
	  copyDllInfo();
    return;
  }

  STARTPERFORMANCE(1);

	dllglobals->ReadLevel(filename);
	copyDllInfo();

  STOPPERFORMANCE(1, "q2admin->ReadLevel", 0, NULL);
}

