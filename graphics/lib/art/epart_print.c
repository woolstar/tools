#include "basic.h"
#include "tscene.h"
#include "tscenelink.h"
#include "eart.h"
#include "epart_word.h"
#include "epart_post.h"
#include "tscene.h"

	/** epart_motion */
extern int	epartmo_print(FILE * fwrite, int type, int offset, tlist_element * ptr, ts_track * tkref) ;

int	epart_printcam(tscene_camera * ref, ts_track * tkptr, FILE *fdest)
{
	fprintf(fdest, "view %s\n", ref-> name);
	fprintf(fdest, "\tsize %d %d\n", ref-> x, ref-> y);

	if (ref-> attr & TSC_ASPECT)
			fprintf(fdest, "\taspect %d %d\n", ref-> aspect_width, ref-> aspect_height);
	if (ref-> attr & TSC_WINDOW)
		fprintf(fdest, "\twindow %d %d %d %d\n", ref-> wxa, ref-> wxb, ref-> wya, ref-> wyb );
	if (ref-> attr & TSC_WIDEVIEW)
		fprintf(fdest, "\twideview\n");
	if ((ref-> attr & TSC_IMPORT) && (ref-> import))
	{
		char typ[8], aux[8];

		switch (ref-> import-> type)
		{
			case CAM_CAM:  strcpy(typ, "-cam"); break ;
			case CAM_WVM:  strcpy(typ, "-wvm"); break ;
			case CAM_INC:  strcpy(typ, "-inc"); break ;
			default:
				* typ= 0;
		}
		if ((CAM_CAM == ref-> import-> type) && ref-> import-> aux) sprintf(aux, " %d", ref-> import-> aux);
			else * aux= '\0';

		if (* typ)
			fprintf(fdest, "\timport %s %s%s\n", typ, ref-> import-> srcfile, aux);
	}

	epartmo_print(fdest, _E_CAM, ref-> motion - 1, NULL, tkptr);
	fprintf(fdest, "\n");

	return 0;
}

char *epfilterLabels[]= {"", "box", "tri", "gauss", "lanczos"};

int	epart_printsetup(ts_setup * ref, FILE * fdest)
{
	char	str[256];

	fprintf(fdest, "setup\n");
	if (ref-> adapt)
		{ fprintf(fdest, "\tadapt %d\n", ref-> adapt); }
	if (ref-> attr & TSS_GAMMA) 
		{ fprintf(fdest, "\tgamma %g\n", ref-> gamma); }
	fprintf(fdest, "\tdepth %d %d\n", ref-> rdepth, ref-> gdepth);
	if (ref-> curcam)
		{ fprintf(fdest, "\tactive %s\n", ref-> curcam-> name); }

	tart_undo_seqname(str, ref-> image.ioffset, ref-> image.imageform);
	fprintf(fdest, "\timage %s", str);
	switch (ref-> image.iformat & 0x0f00)
	{
		case EV_VID: fprintf(fdest, " vid");  break;
		case EV_RGB: fprintf(fdest, " rgb");  break;
		case EV_TGA:
			fprintf(fdest, " tga");
			switch (ref-> image.iformat & 0x07)
			{
				case EVT_BT16: fprintf(fdest, "16");  break;
				case EVT_BT24: fprintf(fdest, "24");  break;
				case EVT_BT32: fprintf(fdest, "32");  break;
			}
			if (EVT_TP10 == (ref-> image.iformat & 0xf0)) fprintf(fdest, "-10");
			break;
		case EV_VAD: fprintf(fdest, " vad");  break;
		case EV_PICT: fprintf(fdest, " pict");  break; 
	}
	if (EV_FLIP & ref-> image.iformat) fprintf(fdest, " flip");
	fprintf(fdest, "\n");

	switch (ref-> bg.attr)
	{
		case TSB_COLOR:
			fprintf(fdest, "\tbackground color %g %g %g\n", C_expandl(ref-> bg.cbase));
			break;
		case TSB_FILE:
			fprintf(fdest, "\tbackground image %s\n", ref-> bg.imagename);
			break;
	}

	fprintf(fdest, "\tambient %g %g %g\n", C_expandl(ref-> ambient));
	if ((ref-> filtertype >= _F_BOX) && (ref-> filtertype <= _F_LANCZOS))
		fprintf(fdest, "\tfilter %s %d\n", epfilterLabels[ref-> filtertype], ref-> filterwidth);

	if (ref-> cmdlist)
	{
		tss_exec * cmdstep ;

		for (cmdstep= ref-> cmdlist; (cmdstep); cmdstep= cmdstep-> next)
			post_write(cmdstep, fdest) ;
	}

	if (* ref-> path)
		fprintf(fdest, "\tpath %s\n", ref-> path);	

	fprintf(fdest, "\n");
	return 0;
}

int	epart_printcontain(tl_contain * ref, tlist_element * list, FILE * fdest)
{
	unsigned int step;
	tlist_element ** teref;

	if (! ref-> count || ! ref-> list)
	{
		if (! list) return 0;
	
		fprintf(fdest, "world\n");
		for (; (list); list= list-> next)
			{ fprintf(fdest, "\tob %s\n", list-> name); }
		return 0;
	}

	fprintf(fdest, "world\n");
	for (step= ref-> count, teref= ref-> list; (step --); teref ++)
	{
		if (! * teref || (_E_OBJECT != (** teref).type)) continue ;
		fprintf(fdest, "\tob %s\n", (** teref).name);
	}
	return 0;
}
