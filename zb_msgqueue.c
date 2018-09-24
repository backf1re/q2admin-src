//
// q2admin
//
// zb_msgqueue.c
//
// copyright 2000 Shane Powell
//


#include "g_local.h"




void addCmdQueue(int  client, byte command, float timeout, unsigned long data, char *str)
{
  proxyinfo[client].cmdQueue[proxyinfo[client].maxCmds].command = command;
  proxyinfo[client].cmdQueue[proxyinfo[client].maxCmds].timeout = ltime + timeout;
  proxyinfo[client].cmdQueue[proxyinfo[client].maxCmds].data = data;
  proxyinfo[client].cmdQueue[proxyinfo[client].maxCmds].str = str;
  
  proxyinfo[client].maxCmds++;
}

qboolean getCommandFromQueue(int client, byte *command, unsigned long *data, char **str)
{
  int i;

  for(i = 0; i < proxyinfo[client].maxCmds; i++)
  {
    if(proxyinfo[client].cmdQueue[i].timeout < ltime)
    {
      // found good command..
      // get info to return
      *command = proxyinfo[client].cmdQueue[i].command;
      *data = proxyinfo[client].cmdQueue[i].data;

      if(str)
      {
        *str = proxyinfo[client].cmdQueue[i].str;
      }

      // remove command
      proxyinfo[client].maxCmds--;
      if(i < proxyinfo[client].maxCmds)
      {
        q2a_memmove(proxyinfo[client].cmdQueue + i, proxyinfo[client].cmdQueue + i + 1, (proxyinfo[client].maxCmds - i) * sizeof(CMDQUEUE));
      }
      return TRUE;
    }
  }

  return FALSE;
}


void removeClientCommand(int client, byte command)
{
  int i = 0;

  while(i < proxyinfo[client].maxCmds)
  {
    if(proxyinfo[client].cmdQueue[i].command == command)
    {
      // remove command
      proxyinfo[client].maxCmds--;
      if(i < proxyinfo[client].maxCmds)
      {
        q2a_memmove(proxyinfo[client].cmdQueue + i, proxyinfo[client].cmdQueue + i + 1, (proxyinfo[client].maxCmds - i) * sizeof(CMDQUEUE));
      }
    }
    else
    {
      i++;
    }
  }
}


void removeClientCommands(int client)
{
  proxyinfo[client].maxCmds = 0;
}

