//
// q2admin
//
// zb_lrcon.c
//
// copyright 2000 Shane Powell
//

#include "g_local.h"

#define LRCONFILE             "q2adminlrcon.txt"
#define LRCON_MAXCMDS         1024

typedef struct
{
	char   *lrconcmd;
  char   *password;
	byte    type;
  regex_t *r;
} lrconcmd_t;

#define LRC_SW		0
#define LRC_EX		1
#define LRC_RE		2

lrconcmd_t lrconcmds[LRCON_MAXCMDS];
int maxlrcon_cmds = 0;


qboolean ReadLRconFile(char *lrcname)
{
  FILE *lrconfile;
	int uptoLine = 0;

  if(maxlrcon_cmds >= LRCON_MAXCMDS)
  {
    return FALSE;
  }

  lrconfile = fopen(lrcname, "rt");
  if(!lrconfile)
  {
    return FALSE;
  }

  while(fgets(buffer, 256, lrconfile))
  {
    char *cp = buffer, *pp;
    int len;

    // remove '\n'
    len = q2a_strlen(buffer) - 1;
    if(buffer[len] == '\n')
    {
      buffer[len] = 0x0;
    }

    SKIPBLANK(cp);

		uptoLine++;

    if(startContains (cp, "SW:") || startContains (cp, "EX:") || startContains (cp, "RE:"))
		{
      // looks ok, add...
			switch(*cp)
			{
				case 'S':
					lrconcmds[maxlrcon_cmds].type = LRC_SW;
					break;

				case 'E':
					lrconcmds[maxlrcon_cmds].type = LRC_EX;
					break;

				case 'R':
					lrconcmds[maxlrcon_cmds].type = LRC_RE;
					break;
			}

			cp += 3;
	    SKIPBLANK(cp);

      pp = cp;

      // find len of password
      len = 0;
      while(*pp && *pp != ' ')
      {
        pp++;
        len++;
      }

      if(!len || *pp == 0)
      {
				gi.dprintf ("Error loading LRCON from line %d in file %s\n", uptoLine, lrcname);
        // no command or zero length password
        continue;
      }

      // allocate memory for password and copy into buffer
      lrconcmds[maxlrcon_cmds].password = gi.TagMalloc (len, TAG_LEVEL);
      pp = lrconcmds[maxlrcon_cmds].password;
      while(*cp && *cp != ' ')
      {
        *pp++ = *cp++;
      }
	    SKIPBLANK(cp);
      *pp = 0;

      len = q2a_strlen(cp) + 1;

      // zero length command
      if(!len)
      {
        gi.TagFree(lrconcmds[maxlrcon_cmds].password);
				gi.dprintf ("Error loading LRCON from line %d in file %s\n", uptoLine, lrcname);
        continue;
      }

      lrconcmds[maxlrcon_cmds].lrconcmd = gi.TagMalloc (len, TAG_LEVEL);
      q2a_strcpy(lrconcmds[maxlrcon_cmds].lrconcmd, cp);

			if(lrconcmds[maxlrcon_cmds].type == LRC_RE)
			{
        q_strupr(cp);

        lrconcmds[maxlrcon_cmds].r = gi.TagMalloc (sizeof(*lrconcmds[maxlrcon_cmds].r), TAG_LEVEL);
        q2a_memset(lrconcmds[maxlrcon_cmds].r, 0x0, sizeof(*lrconcmds[maxlrcon_cmds].r));
//        if(regcomp(lrconcmds[maxlrcon_cmds].r, strbuffer, REG_EXTENDED))
        if(regcomp(lrconcmds[maxlrcon_cmds].r, cp, 0))
        {
          gi.TagFree(lrconcmds[maxlrcon_cmds].r);
          lrconcmds[maxlrcon_cmds].r = 0;

          // malformed re... skip this lrcon
					gi.dprintf ("Error loading LRCON from line %d in file %s\n", uptoLine, lrcname);
					continue;
				}
			}
			else
			{
				lrconcmds[maxlrcon_cmds].r = 0;
			}
      
      maxlrcon_cmds++;

      if(maxlrcon_cmds >= LRCON_MAXCMDS)
      {
        break;
      }
    }
		else if(!(cp[0] == ';' || cp[0] == '\n'))
		{
			gi.dprintf ("Error loading LRCON from line %d in file %s\n", uptoLine, lrcname);
		}
  }

  fclose(lrconfile);

  return TRUE;
}



void freeLRconLists(void)
{
  while(maxlrcon_cmds)
  {
    maxlrcon_cmds--;
		gi.TagFree(lrconcmds[maxlrcon_cmds].password);
		gi.TagFree(lrconcmds[maxlrcon_cmds].lrconcmd);
		if(lrconcmds[maxlrcon_cmds].r)
		{
      regfree(lrconcmds[maxlrcon_cmds].r);
			gi.TagFree(lrconcmds[maxlrcon_cmds].r);
		}
  }
}

void readLRconLists(void)
{
  qboolean ret;

  freeLRconLists();

  ret = ReadLRconFile(LRCONFILE);

  sprintf(buffer, "%s/%s", moddir, LRCONFILE);
  if(ReadLRconFile(buffer))
  {
    ret = TRUE;
  }

  if(!ret)
  {
  	gi.dprintf ("WARNING: " LRCONFILE " could not be found\n");
    logEvent(LT_INTERNALWARN, 0, NULL, LRCONFILE " could not be found", IW_LRCONSETUPLOAD, 0.0);
  }
}



void reloadlrconfileRun(int startarg, edict_t *ent, int client)
{
  readLRconLists();
  gi.cprintf (ent, PRINT_HIGH, "lrcon's reloaded\n");
}


qboolean checklrcon(char *cp, int lrcon)
{
  char strbuffer[256];

	switch(lrconcmds[lrcon].type)
	{
		case LRC_SW:
			return startContains (cp, lrconcmds[lrcon].lrconcmd);

		case LRC_EX:
			return !Q_stricmp(cp, lrconcmds[lrcon].lrconcmd);

		case LRC_RE:
      q2a_strcpy(strbuffer, cp);
      q_strupr(strbuffer);
			return (regexec(lrconcmds[lrcon].r, strbuffer, 0, 0, 0) != REG_NOMATCH);
	}

	return FALSE;
}


void run_lrcon(edict_t *ent, int client)
{
  int i;

  // check each command for a good password..
  for(i = 0; i < maxlrcon_cmds; i++)
  {
    // create command string to compare...
    char *cp = gi.args(), *pp, *orgcp;
    orgcp = cp;
    
    SKIPBLANK(cp);

    pp = lrconcmds[i].password;

    // check password...
    while(*cp && *cp != ' ')
    {
      if(*pp != *cp)
      {
        break;
      }

      cp++;
      pp++;
    }

    if(*cp != ' ')
    {
      continue;
    }

    SKIPBLANK(cp);

    if(*cp)
    {
      if(checklrcon(cp, i) && q2a_strchr(cp, ';') == NULL)
      {
        sprintf(buffer, "rcon %s %s\n", rconpassword->string, cp);

        // found a good command to run..
	  		stuffcmd(ent, buffer);

        logEvent(LT_CLIENTLRCON, client, ent, orgcp, 0, 0.0);
        return;
      }
    }
  }

  gi.cprintf(ent, PRINT_HIGH, "Unknown lrcon command\n");
}



void listlrconsRun(int startarg, edict_t *ent, int client)
{
  addCmdQueue(client, QCMD_DISPLRCONS, 0, 0, 0);

	gi.cprintf (ent, PRINT_HIGH, "Start lrcon's List:\n");
}


void displayNextLRCon(edict_t *ent, int client, long lrconnum)
{
	if(lrconnum < maxlrcon_cmds)
	{
	  switch(lrconcmds[lrconnum].type)
	  {
		  case LRC_SW:
    		gi.cprintf (ent, PRINT_HIGH, "%4d SW:\"%s\" \"%s\"\n", lrconnum + 1, lrconcmds[lrconnum].password, lrconcmds[lrconnum].lrconcmd);
        break;

		  case LRC_EX:
    		gi.cprintf (ent, PRINT_HIGH, "%4d EX:\"%s\" \"%s\"\n", lrconnum + 1, lrconcmds[lrconnum].password, lrconcmds[lrconnum].lrconcmd);
        break;

		  case LRC_RE:
    		gi.cprintf (ent, PRINT_HIGH, "%4d RE:\"%s\" \"%s\"\n", lrconnum + 1, lrconcmds[lrconnum].password, lrconcmds[lrconnum].lrconcmd);
        break;
	  }
		lrconnum++;
	  addCmdQueue(client, QCMD_DISPLRCONS, 0, lrconnum, 0);
	}
	else
	{
		gi.cprintf (ent, PRINT_HIGH, "End lrcon's List\n");
	}
}



#define LRCONCMD     "[sv] !lrcon [SW/EX/RE] \"password\" \"command\"\n"

void lrconRun(int startarg, edict_t *ent, int client)
{
  char *cmd;
  int len;

  if(maxlrcon_cmds >= LRCON_MAXCMDS)
  {
	  gi.cprintf (ent, PRINT_HIGH, "Sorry, maximum lrcon's reached!!\n");
    return;
  }

	if (gi.argc() <= startarg + 2)
  {
	  gi.cprintf (ent, PRINT_HIGH, LRCONCMD);
    return;
  }

  cmd = gi.argv(startarg);

  if(Q_stricmp(cmd, "SW") == 0)
  {
    lrconcmds[maxlrcon_cmds].type = LRC_SW;
  }
  else if(Q_stricmp(cmd, "EX") == 0)
  {
    lrconcmds[maxlrcon_cmds].type = LRC_EX;
  }
  else if(Q_stricmp(cmd, "RE") == 0)
  {
    lrconcmds[maxlrcon_cmds].type = LRC_RE;
  }
  else
  {
	  gi.cprintf (ent, PRINT_HIGH, LRCONCMD);
    return;
  }

  cmd = gi.argv(startarg + 1);

  if(isBlank(cmd))
  {
	  gi.cprintf (ent, PRINT_HIGH, LRCONCMD);
    return;
  }

  len = q2a_strlen(cmd) + 1;

  lrconcmds[maxlrcon_cmds].password = gi.TagMalloc (len, TAG_LEVEL);

  q2a_strcpy(lrconcmds[maxlrcon_cmds].password, cmd);


  cmd = gi.argv(startarg + 2);

  if(isBlank(cmd))
  {
    gi.TagFree(lrconcmds[maxlrcon_cmds].password);
	  gi.cprintf (ent, PRINT_HIGH, LRCONCMD);
    return;
  }

  len = q2a_strlen(cmd) + 20;

  lrconcmds[maxlrcon_cmds].lrconcmd = gi.TagMalloc (len, TAG_LEVEL);
  processstring(lrconcmds[maxlrcon_cmds].lrconcmd, cmd, len - 1, 0);
//  q2a_strcpy(lrconcmds[maxlrcon_cmds].lrconcmd, cmd);

	if(lrconcmds[maxlrcon_cmds].type == LRC_RE)
	{
    q_strupr(cmd);

    lrconcmds[maxlrcon_cmds].r = gi.TagMalloc (sizeof(*lrconcmds[maxlrcon_cmds].r), TAG_LEVEL);
    q2a_memset(lrconcmds[maxlrcon_cmds].r, 0x0, sizeof(*lrconcmds[maxlrcon_cmds].r));
//        if(regcomp(lrconcmds[maxlrcon_cmds].r, cmd, REG_EXTENDED))
    if(regcomp(lrconcmds[maxlrcon_cmds].r, cmd, 0))
    {
      gi.TagFree(lrconcmds[maxlrcon_cmds].password);
      gi.TagFree(lrconcmds[maxlrcon_cmds].lrconcmd);
      gi.TagFree(lrconcmds[maxlrcon_cmds].r);
      lrconcmds[maxlrcon_cmds].r = 0;

      // malformed re... 
  	  gi.cprintf (ent, PRINT_HIGH, "Regular Expression couldn't compile!!\n");
			return;
		}
	}
	else
	{
		lrconcmds[maxlrcon_cmds].r = 0;
	}
  
	switch(lrconcmds[maxlrcon_cmds].type)
	{
		case LRC_SW:
    	gi.cprintf (ent, PRINT_HIGH, "%4d SW:\"%s\" \"%s\" added\n", maxlrcon_cmds + 1, lrconcmds[maxlrcon_cmds].password, lrconcmds[maxlrcon_cmds].lrconcmd);
      break;

		case LRC_EX:
    	gi.cprintf (ent, PRINT_HIGH, "%4d EX:\"%s\" \"%s\" added\n", maxlrcon_cmds + 1, lrconcmds[maxlrcon_cmds].password, lrconcmds[maxlrcon_cmds].lrconcmd);
      break;

		case LRC_RE:
    	gi.cprintf (ent, PRINT_HIGH, "%4d RE:\"%s\" \"%s\" added\n", maxlrcon_cmds + 1, lrconcmds[maxlrcon_cmds].password, lrconcmds[maxlrcon_cmds].lrconcmd);
      break;
	}

  maxlrcon_cmds++;
}



#define LRCONDELCMD     "[sv] !lrcondel lrconnum\n"


void lrconDelRun(int startarg, edict_t *ent, int client)
{
  int lrcon;

	if (gi.argc() <= startarg)
  {
	  gi.cprintf (ent, PRINT_HIGH, LRCONDELCMD);
    return;
  }

  lrcon = q2a_atoi(gi.argv(startarg));

  if(lrcon < 1 || lrcon > maxlrcon_cmds)
  {
	  gi.cprintf (ent, PRINT_HIGH, LRCONDELCMD);
    return;
  }

  lrcon--;

  gi.TagFree(lrconcmds[lrcon].password);
  gi.TagFree(lrconcmds[lrcon].lrconcmd);
	if(lrconcmds[lrcon].r)
	{
    regfree(lrconcmds[lrcon].r);
		gi.TagFree(lrconcmds[lrcon].r);
	}

  if(lrcon + 1 < maxlrcon_cmds)
  {
    q2a_memmove((lrconcmds + lrcon), (lrconcmds + lrcon + 1), sizeof(lrconcmd_t) * (maxlrcon_cmds - lrcon));
  }

  maxlrcon_cmds--;

  gi.cprintf (ent, PRINT_HIGH, "lrcon deleted\n");
}


