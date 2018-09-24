//
// q2admin
//
// zb_zbot.c
//
// copyright 2000 Shane Powell
//

#include "g_local.h"


int clientsidetimeout = 30;  // 30 seconds should be good for internet play
int zbotdetectactivetimeout = 0;  // -1 == random

// char testchars[] = "!@#%^&*()_=|?.>,<[{]}\':1234567890qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM";
char testchars[] = "!@#%^&*()_=|?.>,<[{]}\':";
//                  012345678901234567890 1234567890123456789012345678901234567890123456789012345678901234
//                            1         2          3         4         5         6         7         8
int testcharslength = sizeof(testchars) - 1;


qboolean zbc_enable = TRUE;
qboolean timescaledetect = TRUE;
qboolean swap_attack_use = FALSE;


byte impulsesToKickOn[MAXIMPULSESTOTEST];
byte maxImpulses = 0;

char *impulsemessages[] = 
{
	"169 (zbot toggle menu command)",
	"170 (zbot toggle menu command)",
	"171 (zbot toggle menu command)",
	"172 (zbot toggle menu command)",
	"173 (zbot toggle menu command)",
	"174 (zbot toggles bot on/off command)",
	"175 (zbot toggles scanner display command)"
};



//===================================================================

qboolean checkImpulse(byte impulse)
{
  int i;

  if(!maxImpulses)
  {
    return TRUE;
  }

  for(i = 0; i < maxImpulses; i++)
  {
    if(impulsesToKickOn[i] == impulse)
    {
      return TRUE;
    }
  }

  return FALSE;
}




void lookupClientIp(edict_t *ent)
{
	char *ip;
//	char buffer2[256];
	int client = getEntOffset(ent) - 1;


	if(proxyinfo[client].ipaddress[0])
	{
		return;
	}

	ip = Info_ValueForKey(proxyinfo[client].userinfo, "ip");

	if(ip && *ip)
	{
		q2a_strcpy(proxyinfo[client].ipaddress, ip);
		return;
	}

	// alternate ip lookup
/*	
	if(logfile->value != 2)
	{
		sprintf(buffer2, "logfile 2;dumpuser %d;logfile %d\n", client, logfile->value);
	}
	else
	{
		sprintf(buffer2, "dumpuser %d\n", client);
	}

	gi.AddCommandString(buffer2);
  addCmdQueue(client, QCMD_GETIPALT, 0, 0);
*/
}





void readIpFromLog(int client, edict_t *ent)
{
	FILE *dumpfile;
	long fpos;

	if(proxyinfo[client].ipaddress[0])
	{
		return;
	}

	sprintf(buffer, "%s/qconsole.log", moddir);
	dumpfile = fopen(buffer, "rt");
	if(!dumpfile)
	{
		return;
	}

	fseek(dumpfile, 0, SEEK_END);
	fpos = ftell(dumpfile) - 1;

	while(getLastLine(buffer, dumpfile, &fpos))
	{
		if(startContains(buffer, "ip"))
		{
			char *cp = buffer + 3;
			char *dp = proxyinfo[client].ipaddress;

      SKIPBLANK(cp);

			while(*cp && *cp != ' ' && *cp != '\n')
			{
				*dp++ = *cp++;
			}

			*dp = 0;
			break;
		}
		else if(startContains(buffer, "userinfo"))
		{
			break;
		}
	}

	fclose(dumpfile);
}




int checkForOverflows(edict_t *ent, int client)
{
  FILE *q2logfile;
  char checkmask1[100], checkmask2[100];
  int ret = 0;

  sprintf(buffer, "%s/qconsole.log", moddir);
  q2logfile = fopen(buffer, "rt");
  if(!q2logfile)
  {
    return 0;  // assume ok
  }

  fseek(q2logfile, proxyinfo[client].logfilecheckpos, SEEK_SET);

  sprintf(checkmask1, "WARNING: msg overflowed for %s", proxyinfo[client].name);
  sprintf(checkmask2, "%s overflowed", proxyinfo[client].name);

  while(fgets(buffer, 256, q2logfile))
  {
		if(startContains(buffer, checkmask1) || startContains(buffer, checkmask2))
		{ // we have a problem...
      ret = 1;
	    proxyinfo[client].clientcommand &= ~CCMD_ZPROXYCHECK2;
      removeClientCommand(client, QCMD_ZPROXYCHECK1);
      removeClientCommand(client, QCMD_ZPROXYCHECK2);
      addCmdQueue(client, QCMD_RESTART, 2 + (5 * random()), 0, 0);

    	sprintf(checkmask1, "I(%d) Exp(%s) (%s) (overflow detected)", proxyinfo[client].charindex, proxyinfo[client].teststr, buffer);
      logEvent(LT_INTERNALWARN, client, ent, checkmask1, IW_OVERFLOWDETECT, 0.0);
      break;
    }
  }
  
  fseek(q2logfile, 0, SEEK_END);
  proxyinfo[client].logfilecheckpos = ftell(q2logfile);
  fclose(q2logfile);

  return ret;
}




void serverLogZBot(edict_t *ent, int client)
{
  addCmdQueue(client, QCMD_LOGTOFILE1, 0, 0, 0);

	if(customServerCmd[0])
	{
    // copy string across to buffer, replacing %c with client number
    char *cp = customServerCmd;
    char *dp = buffer;

    while(*cp)
    {
      if(*cp == '%' && tolower(*(cp + 1)) == 'c')
      {
        sprintf(dp, "%d", client);
        dp += q2a_strlen(dp);
        cp += 2;
      }
      else
      {
        *dp++ = *cp++;
      }
    }

    *dp = 0x0;

		gi.AddCommandString(buffer);
	}
}



//===================================================================


/*
static pmove_done = 0;
static vec3_t pmove_origin, pmove_old, pmove_orgvel, pmove_newvel;
static short pmove_orggrav;
*/


void	Pmove_internal (pmove_t *pmove)
{
  if(q2adminrunmode == 0)
  {
    gi.Pmove(pmove);
    return;
  }

//  pmove_orggrav = pmove->s.gravity;
//  VectorCopy(pmove->s.velocity, pmove_orgvel);
//  VectorCopy(pmove->s.origin, pmove_old);

  gi.Pmove(pmove);

//  VectorCopy(pmove->s.origin, pmove_origin);
//  VectorCopy(pmove->s.velocity, pmove_newvel);

//  pmove_done = 1;
}


int VectorCompare (vec3_t v1, vec3_t v2)
{
	if (v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2])
			return 0;
			
	return 1;
}


void ClientThink (edict_t *ent, usercmd_t *ucmd)
{
	int client;
  char *msg = 0;
  INITPERFORMANCE_2(1);
  INITPERFORMANCE_2(2);

	if(!dllloaded) return;

  if(q2adminrunmode == 0)
  {
  	dllglobals->ClientThink(ent, ucmd);
	  copyDllInfo();
    return;
  }

	client = getEntOffset(ent) - 1;
  
  STARTPERFORMANCE(1);

	if(ucmd->impulse)
	{
		if(client >= maxclients->value) return;

		if(displayimpulses)
		{
			if(ucmd->impulse >= 169 && ucmd->impulse <= 175)
			{
        msg = impulsemessages[ucmd->impulse - 169];
				gi.bprintf (PRINT_HIGH, "%s generated a impulse %s\n", proxyinfo[client].name, msg);
			}
			else
			{
        msg = "generated a impulse";
				gi.bprintf (PRINT_HIGH, "%s generated a impulse %d\n", proxyinfo[client].name, ucmd->impulse);
			}
		}

		if(ucmd->impulse >= 169 && ucmd->impulse <= 175)
		{
			proxyinfo[client].impulse = ucmd->impulse;
      addCmdQueue(client, QCMD_LOGTOFILE2, 0, 0, 0);
		}
    else
    {
      proxyinfo[client].impulse = ucmd->impulse;
      addCmdQueue(client, QCMD_LOGTOFILE3, 0, 0, 0);
    }

		if(disconnectuserimpulse && checkImpulse(ucmd->impulse))
		{
      proxyinfo[client].impulsesgenerated++;

      if(proxyinfo[client].impulsesgenerated >= maximpulses)
      {
        addCmdQueue(client, QCMD_DISCONNECT, 1, 0, msg);
      }
		}
	}

  if(swap_attack_use)
  {
    byte temp = (ucmd->buttons & BUTTON_ATTACK);

    if(ucmd->buttons & BUTTON_USE)
    {
      ucmd->buttons |= BUTTON_ATTACK;
    }
    else
    {
      ucmd->buttons &= ~BUTTON_ATTACK;
    }

    if(temp)
    {
      ucmd->buttons |= BUTTON_USE;
    }
    else
    {
      ucmd->buttons &= ~BUTTON_USE;
    }
  }

  if(!(proxyinfo[client].clientcommand & BANCHECK))
  {
		if(!(proxyinfo[client].clientcommand & CCMD_ZBOTDETECTED) && zbc_enable)
		{
			if(zbc_ZbotCheck(client, ucmd))
			{
				proxyinfo[client].clientcommand |= (CCMD_ZBOTDETECTED | CCMD_ZPROXYCHECK2);
				removeClientCommand(client, QCMD_ZPROXYCHECK1);
				addCmdQueue(client, QCMD_ZPROXYCHECK2, 1, IW_ZBCHECK, 0);
				addCmdQueue(client, QCMD_RESTART, 1, IW_ZBCHECK, 0);
			}
		}

    STARTPERFORMANCE(2);
  	dllglobals->ClientThink(ent, ucmd);
    STOPPERFORMANCE_2(2, "mod->ClientThink", 0, NULL);

	  copyDllInfo();
  }

  STOPPERFORMANCE_2(1, "q2admin->ClientThink", 0, NULL);

/*
  if(pmove_done)
  {
    vec3_t change;
    
    VectorSubtract(pmove_orgvel, pmove_newvel, change);
    if(change[0] < 0) change[0] = -change[0];
    if(change[1] < 0) change[1] = -change[1];

    if(change[0] > 1000.0 || change[1] > 1000.0)
    {
      gi.cprintf(ent, PRINT_HIGH, "c:%f, %f\n", change[0], change[1]);
    }
  }
*/
}




void G_RunFrame (void)
{
	static int client = -1;
  int maxdoclients;
	edict_t *ent;
  byte command;
  unsigned long data;
  char *str;
  INITPERFORMANCE_2(1);
  INITPERFORMANCE_2(2);

	if(!dllloaded) return;

  if(q2adminrunmode == 0)
  {
  	dllglobals->RunFrame();
	  copyDllInfo();
    return;
  }

  STARTPERFORMANCE(1);

  lframenum++;
	ltime = lframenum * FRAMETIME;

  if(serverinfoenable && lframenum > 10)
  {
//    sprintf(buffer, "logfile 2;set Bot \"No Bots\" s\n");
    sprintf(buffer, "set Bot \"No Bots\" s\n");
  	gi.AddCommandString(buffer);
    serverinfoenable = 0;
		serverlogfile = 0;
  }
	else if(serverlogfile && lframenum > 10)
	{
		serverlogfile = 0;
//  	gi.AddCommandString("logfile 2\n");
	}


  if(maxReconnectList)
  {
    int i;

    for(i = 0; i < maxReconnectList; i++)
    {
      if(reconnectlist[i].reconnecttimeout < ltime)
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

        if(i + 1 < maxReconnectList)
        {
          q2a_memmove(&(reconnectlist[i]), &(reconnectlist[i + 1]), (maxReconnectList - (i + 1)) * sizeof(reconnect_info));
          i--;
        }
        maxReconnectList--;
      }
    }
  }

  if(framesperprocess && (lframenum % framesperprocess) != 0)
  {
  	dllglobals->RunFrame();
	  copyDllInfo();
    return;
  }

  maxdoclients = client + maxclientsperframe;
  if(maxdoclients > maxclients->value)
  {
    maxdoclients = maxclients->value;
  }
  
	for(; client < maxdoclients; client++)
	{
    if(getCommandFromQueue(client, &command, &data, &str))
    {
			if(client < 0)
			{
				ent = NULL;
			}
			else
			{
				ent = getEnt((client + 1));
			}

      if(!proxyinfo[client].inuse)
      {
        switch(command)
        {
        case QCMD_STARTUP:
          addCmdQueue(client, QCMD_STARTUPTEST, 2, 0, 0);
  	      proxyinfo[client].clientcommand |= CCMD_STARTUPTEST;
          break;

        case QCMD_STARTUPTEST:
          if(proxyinfo[client].clientcommand & CCMD_ZBOTDETECTED)
          {
            break;
          }

          proxyinfo[client].inuse = 1;

          if(proxyinfo[client].retries > MAXSTARTTRY)
          {
						if(zbotdetect)
						{
							serverLogZBot(ent, client);
							proxyinfo[client].clientcommand &= ~CCMD_STARTUPTEST;
							proxyinfo[client].clientcommand |= (CCMD_ZBOTDETECTED | CCMD_ZPROXYCHECK2);
							addCmdQueue(client, QCMD_ZPROXYCHECK2, 1, IW_STARTUP, 0);
							proxyinfo[client].charindex = -1;
							logEvent(LT_INTERNALWARN, client, ent, "Startup Init Fail", IW_STARTUPFAIL, 0.0);
						}
            break;
          }

				  stuffcmd(ent, zbot_teststring1);
          addCmdQueue(client, QCMD_STARTUPTEST, 5, 0, 0);
          proxyinfo[client].retries++;
          break;

        case QCMD_DISCONNECT:
//					stuffcmd(ent, "disconnect\n");
          proxyinfo[client].clientcommand |= CCMD_KICKED;
          logEvent(LT_CLIENTKICK, client, ent, str, 0, 0.0);
          gi.cprintf (ent, PRINT_HIGH, "You have been kicked %s\n", proxyinfo[client].name);

          // NOTE: no break, fall though to the QCMD_KICK

        case QCMD_KICK:
          sprintf(buffer, "\nkick %d\n", client);
      	  gi.AddCommandString(buffer);
          break;

        case QCMD_RECONNECT:
        {
          int i;
	        char *ip = Info_ValueForKey(proxyinfo[client].userinfo, "ip");
          char *bp = ip;

          while(*bp && *bp != ':')
          {
            bp++;
          }

          *bp = 0;

          q2a_strcpy(reconnectlist[maxReconnectList].userinfo, proxyinfo[client].userinfo);
          reconnectlist[maxReconnectList].reconnecttimeout = ltime + reconnect_time;

          // check for retry entry..
          for(i = 0; i < maxretryList; i++)
          {
            if(q2a_strcmp(retrylist[i].ip, ip) == 0)
            {
              break;
            }
          }

          if(i < maxretryList)
          {
            if(retrylist[i].retry >= 5)
            {
              int j;

              // remove the retry list entry if needed...
              for(j = 0; j < maxReconnectList; j++)
              {
                if(reconnectlist[j].retrylistidx == i)
                {
                  break;
                }
              }

              if(j >= maxReconnectList)
              {
                if(i + 1 < maxretryList)
                {
                  q2a_memmove(&(retrylist[i]), &(retrylist[i + 1]), (maxretryList - (i + 1)) * sizeof(retrylist_info));
                }
                maxretryList--;
              }

              // cut off here...
              sprintf(buffer, "\ndisconnect\n");
					    stuffcmd(ent, buffer);
              break;
            }

            retrylist[i].retry++;
            reconnectlist[maxReconnectList].retrylistidx = i;
          }
          else
          {
            q2a_strcpy(retrylist[maxretryList].ip, ip);
            retrylist[maxretryList].retry = 0;
            maxretryList++;
          }

          maxReconnectList++;

          sprintf(buffer, "\nconnect %s\n", reconnect_address);
					stuffcmd(ent, buffer);
//          addCmdQueue(client, QCMD_KICK, 0, 0, NULL);
          break;
        }

        default:
          // add command to back of line for processing later..
          addCmdQueue(client, command, 0, data, str);
          break;
        }
      }
      else switch(command)
      {
      case QCMD_STARTUPTEST:
        if(proxyinfo[client].clientcommand & CCMD_ZBOTDETECTED)
        {
          break;
        }

        if(proxyinfo[client].retries > MAXSTARTTRY)
        {
					if(zbotdetect)
					{
						serverLogZBot(ent, client);
						proxyinfo[client].clientcommand &= ~CCMD_STARTUPTEST;
						proxyinfo[client].clientcommand |= (CCMD_ZBOTDETECTED | CCMD_ZPROXYCHECK2);
						addCmdQueue(client, QCMD_ZPROXYCHECK2, 1, IW_STARTUPTEST, 0);
						proxyinfo[client].charindex = -2;
						logEvent(LT_INTERNALWARN, client, ent, "Startup Init Fail 2", IW_STARTUPFAIL, 0.0);
						break;
					}
        }

				stuffcmd(ent, zbot_teststring1);
        addCmdQueue(client, QCMD_STARTUPTEST, 5, 0, 0);
        proxyinfo[client].retries++;
        break;

      case QCMD_RESTART:
				if(zbotdetect)
				{
					if(!(proxyinfo[client].clientcommand & CCMD_ZBOTCLEAR))
					{
		        addCmdQueue(client, QCMD_ZPROXYCHECK1, 0, 0, 0); // retry check for proxy
					}
				}
        break;

      case QCMD_ZPROXYCHECK1:
        // are we at the end?
			  if(proxyinfo[client].charindex >= testcharslength)
			  {
          break;
        }

				// begin test for proxies
				proxyinfo[client].teststr[0] = testchars[proxyinfo[client].charindex];
				proxyinfo[client].teststr[1] = BOTDETECT_CHAR1;
				proxyinfo[client].teststr[2] = BOTDETECT_CHAR2;
				proxyinfo[client].teststr[3] = zbot_testchar1;
				proxyinfo[client].teststr[4] = zbot_testchar2;
				proxyinfo[client].teststr[5] = RANDCHAR();
				proxyinfo[client].teststr[6] = RANDCHAR();
				proxyinfo[client].teststr[7] = 0;
				proxyinfo[client].teststr[8] = 0;

        sprintf(buffer, "\n%s\n%s\n", proxyinfo[client].teststr, zbot_teststring_test2);

				stuffcmd(ent, buffer);

        proxyinfo[client].clientcommand |= CCMD_ZPROXYCHECK2;
        addCmdQueue(client, QCMD_ZPROXYCHECK2, clientsidetimeout, IW_ZBOTTEST, 0);
        break;

      case QCMD_ZPROXYCHECK2: // are we dealing with a proxy???
      {
        char text[50];

				if(!(proxyinfo[client].clientcommand & CCMD_ZPROXYCHECK2))
        {
    		  sprintf(text, "I(%d) Exp(%s)", proxyinfo[client].charindex, proxyinfo[client].teststr);
          logEvent(LT_INTERNALWARN, client, ent, text, data, 0.0);
          break;
        }

        if(proxyinfo[client].charindex >= testcharslength)
        {
    		  sprintf(text, "I(%d >= end) Exp(%s)", proxyinfo[client].charindex, proxyinfo[client].teststr);
          logEvent(LT_INTERNALWARN, client, ent, text, data, 0.0);
          break;
        }


				// yes...  detected by long timeout or the normal timeout on detect
				if(!(proxyinfo[client].clientcommand & CCMD_ZBOTDETECTED))
				{
          if(checkForOverflows(ent, client))
          {
            break;
          }

          if(proxyinfo[client].retries < MAXDETECTRETRIES)
          {
  				  // try and get "unknown command" off the screen as fast as possible
  		      proxyinfo[client].clientcommand &= ~CCMD_ZPROXYCHECK2;
            addCmdQueue(client, QCMD_CLEAR, 0, 0, 0);
            addCmdQueue(client, QCMD_RESTART, 2 + (3 * random()), 0, 0);
            proxyinfo[client].retries++;
            break;
          }

					serverLogZBot(ent, client);
				}

				proxyinfo[client].clientcommand &= ~CCMD_ZPROXYCHECK2;
				proxyinfo[client].clientcommand |= CCMD_ZBOTDETECTED;

				if(displayzbotuser)
				{
					int i;

					q2a_strcpy(buffer, zbotuserdisplay);
					q2a_strcat(buffer, "\n");

					for(i = 0; i < numofdisplays; i++)
					{
						gi.bprintf (PRINT_HIGH, buffer, proxyinfo[client].name);
					}
				}
				
				if(customClientCmd[0])
				{
          addCmdQueue(client, QCMD_CUSTOM, 0, 0, 0);
				}

				if(disconnectuser)
				{
          addCmdQueue(client, QCMD_DISCONNECT, 1, 0, zbotuserdisplay);
				}
        break;
      }

      case QCMD_TESTSTANDARDPROXY:
        gi.cprintf(ent, PRINT_HIGH, "%s: p_version Standard Proxy Test\r\n", proxyinfo[client].name);
        break;

      case QCMD_TESTRATBOT:
        gi.cprintf(ent, PRINT_HIGH, "ratbot Detect Test ( %s )\r\n", "rbkck &%trf");
        addCmdQueue(client, QCMD_TESTRATBOT2, clientsidetimeout, 0, 0);
        proxyinfo[client].clientcommand |= CCMD_RATBOTDETECT;
        addCmdQueue(client, QCMD_TESTRATBOT3, 2, 0, 0);
        break;

      case QCMD_TESTRATBOT2:
				if(!(proxyinfo[client].clientcommand & CCMD_RATBOTDETECT))
        {
          logEvent(LT_INTERNALWARN, client, ent, "RatBot detect problem", 0, 0.0);
          break;
        }

//        proxyinfo[client].clientcommand &= ~CCMD_RATBOTDETECT;
        break;

			case QCMD_TESTRATBOT3:
        proxyinfo[client].clientcommand |= CCMD_RATBOTDETECTNAME;
        addCmdQueue(client, QCMD_TESTRATBOT4, clientsidetimeout, 0, 0);
        sprintf(buffer, "\nname " RATBOT_CHANGENAMETEST ";wait;wait;name \"%s\"\n", proxyinfo[client].name);
				stuffcmd(ent, buffer);
				break;

			case QCMD_TESTRATBOT4:
				if(!(proxyinfo[client].clientcommand & CCMD_RATBOTDETECTNAME))
        {
          logEvent(LT_INTERNALWARN, client, ent, "RatBot Detect 2 problem", 0, 0.0);
          break;
        }

				// yes...  detected by long timeout or the normal timeout on detect
				if(!(proxyinfo[client].clientcommand & CCMD_ZBOTDETECTED))
				{
          if(checkForOverflows(ent, client))
          {
            addCmdQueue(client, QCMD_TESTRATBOT3, 2, 0, 0);
            break;
          }

          if(proxyinfo[client].rbotretries < MAXDETECTRETRIES)
          {
//  		      proxyinfo[client].clientcommand &= ~CCMD_RATBOTDETECTNAME;
            addCmdQueue(client, QCMD_TESTRATBOT3, 2 + (3 * random()), 0, 0);
            proxyinfo[client].rbotretries++;
            break;
          }

					proxyinfo[client].charindex = -4;
					serverLogZBot(ent, client);
				}

				proxyinfo[client].clientcommand &= ~CCMD_RATBOTDETECTNAME;
				proxyinfo[client].clientcommand |= CCMD_ZBOTDETECTED;

				if(displayzbotuser)
				{
					int i;

					q2a_strcpy(buffer, zbotuserdisplay);
					q2a_strcat(buffer, "\n");

					for(i = 0; i < numofdisplays; i++)
					{
						gi.bprintf (PRINT_HIGH, buffer, proxyinfo[client].name);
					}
				}
				
				if(customClientCmd[0])
				{
          addCmdQueue(client, QCMD_CUSTOM, 0, 0, 0);
				}

				if(disconnectuser)
				{
          addCmdQueue(client, QCMD_DISCONNECT, 1, 0, zbotuserdisplay);
				}
				break;

      case QCMD_DISPLOGFILE:
        displayLogFileCont(ent, client, data);
        break;

      case QCMD_DISPLOGFILELIST:
        displayLogFileListCont(ent, client, data);
        break;

      case QCMD_DISPLOGEVENTLIST:
        displayLogEventListCont(ent, client, data, FALSE);
        break;

      case QCMD_GETIPALT:
				// open logfile and read IP a\ddress from log
				readIpFromLog(client, ent);
        addCmdQueue(client, QCMD_GETIPALT, 0, 0, 0);
        break;

      case QCMD_LOGTOFILE1:
        logEvent(LT_ZBOT, client, ent, NULL, proxyinfo[client].charindex, 0.0);
	      break;

      case QCMD_LOGTOFILE2:
        logEvent(LT_ZBOTIMPULSES, client, ent, impulsemessages[proxyinfo[client].impulse - 169], proxyinfo[client].impulse, 0.0);
        break;

      case QCMD_LOGTOFILE3:
        logEvent(LT_IMPULSES, client, ent, NULL, proxyinfo[client].impulse, 0.0);
        break;

      case QCMD_CONNECTCMD:
				if(customClientCmdConnect[0])
				{
					sprintf(buffer, "%s\n", customClientCmdConnect);
					stuffcmd(ent, buffer);
				}

        if(customServerCmdConnect[0])
        {
          // copy string across to buffer, replacing %c with client number
          char *cp = customServerCmdConnect;
          char *dp = buffer;

          while(*cp)
          {
            if(*cp == '%' && tolower(*(cp + 1)) == 'c')
            {
              sprintf(dp, "%d", client);
              dp += q2a_strlen(dp);
              cp += 2;
            }
            else
            {
              *dp++ = *cp++;
            }
          }

          *dp = 0x0;

		      gi.AddCommandString(buffer);
        }
        break;

      case QCMD_CLEAR:
				stuffcmd(ent, "clear\n");
        break;

      case QCMD_CUSTOM:
				if(customClientCmd[0])
				{
					sprintf(buffer, "%s\n", customClientCmd);
					stuffcmd(ent, buffer);
				}
        break;

      case QCMD_DISCONNECT:
//					stuffcmd(ent, "disconnect\n");
        proxyinfo[client].clientcommand |= CCMD_KICKED;
        logEvent(LT_CLIENTKICK, client, ent, str, 0, 0.0);
        gi.cprintf (ent, PRINT_HIGH, "You have been kicked %s\n", proxyinfo[client].name);

        // NOTE: no break, fall though to the QCMD_KICK

      case QCMD_KICK:
        sprintf(buffer, "\nkick %d\n", client);
      	gi.AddCommandString(buffer);
        break;

      case QCMD_RECONNECT:
        sprintf(buffer, "\nconnect %s\n", reconnect_address);
				stuffcmd(ent, buffer);
//        addCmdQueue(client, QCMD_KICK, 0, 0, NULL);
        break;

      case QCMD_CLIPTOMAXRATE:
				sprintf(buffer, "rate %d\n", maxrateallowed);
				stuffcmd(ent, buffer);
        break;

      case QCMD_CLIPTOMINRATE:
				sprintf(buffer, "rate %d\n", minrateallowed);
				stuffcmd(ent, buffer);
        break;

      case QCMD_SETUPMAXFPS:
				stuffcmd(ent, "set cl_maxfps $cl_maxfps u\n");
        addCmdQueue(client, QCMD_FORCEUDATAUPDATE, 0, 0, 0);
        break;

      case QCMD_FORCEUDATAUPDATE:
        if(proxyinfo[client].rate)
        {
				  sprintf(buffer, "set rate %d\nset rate %d\n", proxyinfo[client].rate + 1, proxyinfo[client].rate);
				  stuffcmd(ent, buffer);
        }
        break;

      case QCMD_SETMAXFPS:
        if(maxfpsallowed)
        {
				  sprintf(buffer, "set cl_maxfps %d\n", maxfpsallowed);
				  stuffcmd(ent, buffer);
        }
        break;

      case QCMD_SETMINFPS:
        if(minfpsallowed)
        {
				  sprintf(buffer, "set cl_maxfps %d\n", minfpsallowed);
				  stuffcmd(ent, buffer);
        }
        break;

			case QCMD_DISPBANS:
				displayNextBan(ent, client, data);
				break;

			case QCMD_DISPLRCONS:
				displayNextLRCon(ent, client, data);
				break;

      case QCMD_DISPFLOOD:
        displayNextFlood(ent, client, data);
        break;

			case QCMD_DISPSPAWN:
        displayNextSpawn(ent, client, data);
        break;

			case QCMD_DISPVOTE:
        displayNextVote(ent, client, data);
        break;

			case QCMD_DISPDISABLE:
        displayNextDisable(ent, client, data);
        break;

      case QCMD_CHANGENAME:
				sprintf(buffer, "name \"%s\"\n", proxyinfo[client].name);
				stuffcmd(ent, buffer);
        break;

			case QCMD_CHANGESKIN:
				sprintf(buffer, "skin \"%s\"\n", proxyinfo[client].skin);
				stuffcmd(ent, buffer);
				break;

      case QCMD_BAN:
        gi.cprintf (NULL, PRINT_HIGH, "%s: %s\n", proxyinfo[client].name, proxyinfo[client].buffer);
        gi.cprintf (ent, PRINT_HIGH, "%s: %s\n", proxyinfo[client].name, proxyinfo[client].buffer);
    	  addCmdQueue(client, QCMD_DISCONNECT, 1, 0, proxyinfo[client].buffer);
        break;

      case QCMD_DISPCHATBANS:
				displayNextChatBan(ent, client, data);
				break;

      case QCMD_STUFFCLIENT:
        stuffNextLine(ent, client);
        break;

      case QCMD_TESTADMIN:
				stuffcmd(ent, "!setadmin $q2adminpassword\n");
        break;

			case QCMD_RUNVOTECMD:
				gi.AddCommandString(cmdvote);
				break;

			case QCMD_TESTTIMESCALE:
				if(timescaledetect)
				{
					stuffcmd(ent, "swpq2ts $timescale\n");
			    addCmdQueue(client, QCMD_TESTTIMESCALE, 15, 0, 0);
				}
				break;

      case QCMD_SETUPCL_PITCHSPEED:
				stuffcmd(ent, "set cl_pitchspeed $cl_pitchspeed u\n");
        addCmdQueue(client, QCMD_FORCEUDATAUPDATEPS, 0, 0, 0);
        break;

      case QCMD_FORCEUDATAUPDATEPS:
        if(proxyinfo[client].cl_pitchspeed)
        {
				  sprintf(buffer, "set cl_pitchspeed %d\nset cl_pitchspeed %d\n", proxyinfo[client].cl_pitchspeed + 1, proxyinfo[client].cl_pitchspeed);
				  stuffcmd(ent, buffer);
        }
        break;
      }
    }
    else
    {
      if(maxdoclients < maxclients->value)
      {
        maxdoclients++;
      }
    }
  }

  if(client >= maxclients->value)
  {
    client = -1;
  }

  checkOnVoting();

  STARTPERFORMANCE(2);
	dllglobals->RunFrame();
  STOPPERFORMANCE_2(2, "mod->G_RunFrame", 0, NULL);

	copyDllInfo();

  STOPPERFORMANCE_2(1, "q2admin->G_RunFrame", 0, NULL);
}




