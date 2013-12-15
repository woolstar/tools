#include "trun.h"

/*
wp_surface	*worksurface;
*/

extern int	global_time;
extern	tlist_scene	tfocus;

FILE	*cto_objectopen(te_object *fiptr)
{
	FILE *f;
	char finame[1024], fipath[1024];

	switch (fiptr-> icode)
	{
		case 1:
			strcpy(finame, fiptr-> srcnameform);
			break ;
		case 3:
			sprintf(finame, fiptr-> srcnameform, global_time +fiptr-> ioffset);
			break;
		default:
			return NULL ;
	}

	f= fopen(finame, "rb");
	if (NULL == f)
	{
		sprintf(fipath, "%s/%s", tfocus.setup.path, finame);
		f= fopen(fipath, "rb");
	}

	return f;
}

FILE	*cto_fileopen(char *name)
{
	FILE *f;
	char fipath[1024];

	f= fopen(name, "rb");
	if (NULL == f)
	{
		sprintf(fipath, "%s/%s", tfocus.setup.path, name);
		f= fopen(fipath, "rb");
	}
	return f;
}

