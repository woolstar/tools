#include "basic.h"
#include "tscene.h"

int	tsimport_test(ts_cameras * dest)
{
	int istep;
	tscene_camera * step;

	for (istep= dest-> count, step= dest-> list; (istep); istep --, dest ++)
		{ if ((step-> attr & TSC_IMPORT) && (step-> import)) return 1; }

	return 0;
}

static int	loadcam_(tsrc_import * ref)
{
	return 1;
}

static int	loadwvm_(tsrc_import * ref)
{
	return 1;
}

static int	loadinc_(tsrc_import * ref)
{
	FILE * fsrc= fopen(ref-> srcfile, "r") ;
	char ixline[256], *p, c;
	int icount, jstep ;
	float * fptr, * ffil, * fblock ;

	ref-> n= 0, ref-> width= 0;
	if (NULL == fsrc) return 1;
	icount= 0;
	while (fgets(ixline, sizeof(ixline), fsrc))
		{ p= ixline;  P_space;  if (* p) icount ++; }

	if (! icount)
		{ fclose(fsrc);  return 0; }
	rewind(fsrc);
	fblock= EMEM((icount * 10), float);
	if (NULL == fblock)
		{ fclose(fsrc);  return 1; }

	fptr= fblock, icount= 0;
	while (fgets(ixline, sizeof(ixline), fsrc))
	{
		p= ixline;  P_space ;
		for (jstep= 10, ffil= fptr; (jstep && * p); jstep --, ffil ++)
			{ *ffil= atof(p);  P_word ; }
		if (jstep) continue ;
		fptr= ffil, icount ++;
	}

	ref-> n= icount, ref-> width= 10;
	ref-> data= fblock ;

	fclose(fsrc);
	return 0;
}

int	tsimport_load(ts_cameras * dest)
{
	int istep;
	tscene_camera * step;
	
	for (istep= dest-> count, step= dest-> list; (istep); istep --, dest ++)
	{
		if (! (step-> attr & TSC_IMPORT) || ! step-> import) continue ;
		switch (step-> import-> type)
		{
			case CAM_CAM:	if (loadcam_(step-> import)) return 1;  break;
			case CAM_WVM:	if (loadwvm_(step-> import)) return 1;  break;
			case CAM_INC:	if (loadinc_(step-> import)) return 1;  break;
		}
	}

	return 0;
}

float	tsimport_motionvalue(int trackoff, int pos, tsrc_import * src)
{
	float * fptr ;
	if (! src) return 0.0;
	if ((trackoff < 1) || (trackoff > src-> width)) return 0.0;

	if (pos < 1) pos= 0; else pos --;
	if (pos >= src-> n) pos= src-> n -1;

	fptr= src-> data + (src-> width * pos) + (trackoff -1);
	return * fptr;
}

void	tsimport_motionvec(svector *ptr, int trackoff, int pos, tsrc_import * src)
{
	float * fptr ;
	if (! src)
		{ M_zero((* ptr));  return ; }
	if ((trackoff < 1) || ((trackoff +2) > src-> width))
		{ M_zero((* ptr));  return ; }

	if (pos < 1) pos= 0; else pos --;
	if (pos >= src-> n) pos= src-> n -1;

	fptr= src-> data + (src-> width * pos) + (trackoff -1);
	ptr-> x= *(fptr ++), ptr-> y= *(fptr ++), ptr-> z= *fptr;
}

#if 0
thing
{
	int	type, aux ;
	char srcfile[128];

	long n, width;
	float * data ;

} tsrc_import ;

#endif

