#include "testsrv.h"

#include <dlfcn.h>


game_import_t	gi;
void *hdll = NULL;

typedef game_export_t *GAMEAPI (game_import_t *import);

char cbuffer[8192];

void	bprintf (int printlevel, char *fmt, ...)
{
	va_list arglist;

  // convert to string
	va_start(arglist, fmt);
	vsprintf(cbuffer, fmt, arglist);
	va_end(arglist);

  printf("%s", cbuffer);
}

void	dprintf_func (char *fmt, ...)
{
	va_list arglist;

  // convert to string
	va_start(arglist, fmt);
	vsprintf(cbuffer, fmt, arglist);
	va_end(arglist);

  printf("%s", cbuffer);
}

void	cprintf (edict_t *ent, int printlevel, char *fmt, ...)
{
	va_list arglist;

  // convert to string
	va_start(arglist, fmt);
	vsprintf(cbuffer, fmt, arglist);
	va_end(arglist);

  printf("%s", cbuffer);
}

void	centerprintf (edict_t *ent, char *fmt, ...)
{
	va_list arglist;

  // convert to string
	va_start(arglist, fmt);
	vsprintf(cbuffer, fmt, arglist);
	va_end(arglist);

  printf("%s", cbuffer);
}

void	sound (edict_t *ent, int channel, int soundindex, float volume, float attenuation, float timeofs)
{
}

void	positioned_sound (vec3_t origin, edict_t *ent, int channel, int soundinedex, float volume, float attenuation, float timeofs)
{
}

void	configstring (int num, char *string)
{
}

void	error (char *fmt, ...)
{
}

int		modelindex (char *name)
{
  return 0;
}

int		soundindex (char *name)
{
  return 0;
}

int		imageindex (char *name)
{
  return 0;
}

void	setmodel (edict_t *ent, char *name)
{
  return 0;
}


trace_t	rettrace;

trace_t	trace (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, edict_t *passent, int contentmask)
{
  return rettrace;
}

int		pointcontents (vec3_t point)
{
  return 0;
}

qboolean	inPVS (vec3_t p1, vec3_t p2)
{
  return 0;
}

qboolean	inPHS (vec3_t p1, vec3_t p2)
{
  return 0;
}

void		SetAreaPortalState (int portalnum, qboolean open)
{
}

qboolean	AreasConnected (int area1, int area2)
{
  return 0;
}

void	linkentity (edict_t *ent)
{
}

void	unlinkentity (edict_t *ent)
{
}

int		BoxEdicts (vec3_t mins, vec3_t maxs, edict_t **list,	int maxcount, int areatype)
{
  return 0;
}

void	Pmove (pmove_t *pmove)
{
}


void	multicast (vec3_t origin, multicast_t to)
{
}

void	unicast (edict_t *ent, qboolean reliable)
{
}

void	WriteChar (int c)
{
}

void	WriteByte (int c)
{
}

void	WriteShort (int c)
{
}

void	WriteLong (int c)
{
}

void	WriteFloat (float f)
{
}

void	WriteString (char *s)
{
}

void	WritePosition (vec3_t pos)
{
}

void	WriteDir (vec3_t pos)
{
}

void	WriteAngle (float f)
{
}


void	*TagMalloc (int size, int tag)
{
  return malloc(size);
}

void	TagFree (void *block)
{
  free(block);
}

void	FreeTags (int tag)
{
}

cvar_t cvarret = 
{
  "noname",
  "",
  "",
  0,
  0,
  0,
  0,
};

cvar_t	*cvar (char *var_name, char *value, int flags)
{
  return &cvarret;
}

cvar_t	*cvar_set (char *var_name, char *value)
{
  return &cvarret;
}

cvar_t	*cvar_forceset (char *var_name, char *value)
{
  return &cvarret;
}

int		argc (void)
{
  return 0;
}

char	*argv (int n)
{
  return 0;
}

char	*args (void)
{
  return "";
}

void	AddCommandString (char *text)
{
}

void	DebugGraph (float value, int color)
{
}


int main(int argc, char *argv[])
{
	GAMEAPI *getapi;

	gi.bprintf = bprintf;
	gi.dprintf = dprintf_func;
	gi.cprintf = cprintf;
	gi.centerprintf = centerprintf;
	gi.sound = sound;
	gi.positioned_sound = positioned_sound;
	gi.configstring = configstring;
	gi.error = error;
	gi.modelindex = modelindex;
	gi.soundindex = soundindex;
	gi.imageindex = imageindex;
	gi.setmodel = setmodel;
	gi.trace = trace;
	gi.pointcontents = pointcontents;
	gi.inPVS = inPVS;
	gi.inPHS = inPHS;
	gi.SetAreaPortalState = SetAreaPortalState;
	gi.AreasConnected = AreasConnected;
	gi.linkentity = linkentity;
	gi.unlinkentity = unlinkentity;
	gi.BoxEdicts = BoxEdicts;
	gi.Pmove = Pmove;
	gi.multicast = multicast;
	gi.unicast = unicast;
	gi.WriteChar = WriteChar;
	gi.WriteByte = WriteByte;
	gi.WriteShort = WriteShort;
	gi.WriteLong = WriteLong;
	gi.WriteFloat = WriteFloat;
	gi.WriteString = WriteString;
	gi.WritePosition = WritePosition;
	gi.WriteDir = WriteDir;
	gi.WriteAngle = WriteAngle;
	gi.TagMalloc = TagMalloc;
	gi.TagFree = TagFree;
	gi.FreeTags = FreeTags;
	gi.cvar = cvar;
	gi.cvar_set = cvar_set;
	gi.cvar_forceset = cvar_forceset;
	gi.argc = argc;
	gi.argv = argv;
	gi.args = args;
	gi.AddCommandString = AddCommandString;
	gi.DebugGraph = DebugGraph;

  printf("Start test\n");

  printf("load q2admin.so\n");
	hdll = dlopen("./q2admin.so", RTLD_NOW);

  if(hdll == NULL)
  {
    printf("Failed to load (%s)\n", dlerror());
    return 1;
  }
	
  printf("loaded q2admin.so\n");

 	printf ("get function \"GetGameApi\"\n");
	getapi = (GAMEAPI*)dlsym(hdll, "GetGameAPI");
	
  if(getapi == NULL)
  {

		dlclose(hdll);

  	printf ("No \"GetGameApi\" entry in q2admin.so (%s)\n", dlerror());
    return 1;
  }

 	printf ("run function \"GetGameApi\"\n");

	(*getapi)(&gi);
	
 	printf ("Closing q2admin.so\n");

	dlclose(hdll);

 	printf ("finished\n");

  return 0;
}


