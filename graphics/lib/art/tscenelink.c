#include "basic.h"
#include "tscene.h"
#include "tscenelink.h"
#include "eart.h"

int		createElementImmediate(tlist_element ** dest, long type)
{
	tlist_element * tmp;
	te_light * te = NULL;
	char cde[6], * cstep;
	long len;
	float * f;

	tmp= (tlist_element *) malloc(sizeof(tlist_element));
	if (NULL == tmp)
		{ * dest= NULL;  return 1; }

	switch (type)
	{
		case _E_LIGHT:
			len= 6; strcpy(cde, "CZ");
			te= & (tmp-> dat.light);
			break;
		case _E_BEAM:
			len= 6; strcpy(cde, "CD");
			te= & (tmp-> dat.light);
			break;
		case _E_SPOT:
			len= 10; strcpy(cde, "CZDF");
			te= & (tmp-> dat.light);
			break;
		case _E_OBJECT: len= 10; strcpy(cde, "ZSQ"); break;
		default:
			len= 0;
			break;
	}

	bzero(tmp, sizeof(tlist_element));
	tmp-> type= type;
	if (te)
	{
		te-> bright= 0.75;
	}

	if (len)
	{
		tmp-> tracks= f= (float *) malloc(len * sizeof(float));
		if (NULL == tmp-> tracks)
			{ free(tmp);  return 1; }

		for (cstep= cde; (* cstep); cstep ++)
		{
			switch (* cstep)
			{
				case 'C':  /** color */
				case 'S':  /** scale */
					*(f ++)= 1;
					*(f ++)= 1;
					*(f ++)= 1;
					break;
				case 'D':  /** direction */
					*(f ++)= 0;
					*(f ++)= 0;
					*(f ++)= 1;
					break;
				case 'F':  /** focal */
					*(f ++)= 30.0;
					break;
				case 'Q':  /** Q */
					*(f ++)= 1.0;
				case 'Z':  /** zero */
					*(f ++)= 0;
					*(f ++)= 0;
					*(f ++)= 0;
					break;
			}
		}
	}
	else
		tmp-> tracks= NULL;

	*dest= tmp;
	return 0;
}

int		motionImmediate(tlist_element * dest, tlist_scene * ref)
{
	int type, len, ind;
	float * fset;

	if (! dest || ! ref) return 1;
	type= dest-> type;
	switch (type)
	{
		case _E_LIGHT:  len= 6;  break;
		case _E_BEAM:  len= 6;  break;
		case _E_SPOT:  len= 10;  break;
		case _E_OBJECT: len= 10;  break;
		default:
			return -1;
	}

	fset= (float *) malloc(len * sizeof(float));
	if (NULL== fset) return -4;

	dest-> tracks= NULL;
	for (ind= 0; (ind < len); ind ++)
		{ fset[ind]= epart_motionvalue(ind, dest, 1, & (ref-> tracks)); }
	dest-> tracks= fset;
	dest-> motion= 0;
	return 0;
}

int		copyElement(tlist_element ** dest, tscene_element * src)
{
	tlist_element * tmp;

	tmp= (tlist_element *) malloc(sizeof(tlist_element));
	if (NULL == tmp)
		{ * dest= NULL;  return 1; }

	bzero(tmp, sizeof(tlist_element));
	strcpy(tmp-> name, src-> name);
	tmp-> type= src-> type,
	tmp-> attr= src-> attr,
	tmp-> motion= src-> motion;
	tmp-> cmd= src-> cmd;

	if (_E_OBJECT == src-> type)
			{ memcpy(& (tmp-> dat.object), & (src-> dat.object), sizeof(te_object)); }
		else { memcpy(& (tmp-> dat.light), & (src-> dat.light), sizeof(te_light)); }

	*dest= tmp;
	return 0;
}

int		insertElement(tlist_element * item, tlist_scene * ref, int place)
{
	tlist_element * step, ** site;

	if (! item)
		return 1;

	if (_E_OBJECT == item-> type)
	{
		site= & (ref-> elem.objects);
		ref-> elem.ctobject ++;
	}
	else
	{
		site= & (ref-> elem.lights);
		ref-> elem.ctlight ++;
	}

	if (! * site)
	{
		* site= item, item-> next= NULL;
		return 0;
	}

	step= * site;
	while (step && step-> next) { step= step-> next; }
	step->  next= item, item-> next= NULL;

	return 0;
}

void	removeElement(tlist_element * item, tlist_scene * ref)
{
	tlist_element * fol, ** site, * step;

	if (! item) return ;

	if (_E_OBJECT == item-> type) { site= & (ref-> elem.objects); }
		else { site= & (ref-> elem.lights); }

	for (step= * site, fol= NULL; (step && (step != item)); fol= step, step= step-> next) ;
	if (! step) return;

	if (fol) { fol-> next= step-> next; }
		else * site= step-> next;
}

void	freeElement(tlist_element * item)
{
	if (item-> tracks) free(item-> tracks);
	free(item);
}

#define	CLO(yy,xx) yy.count= xx.count, yy.list= xx.list

int  parse_artlist(char * name, tlist_scene * ptr)
{
	int iret, istep;
	tscene tmp;
	tscene_element * step, ** refstep;
	tlist_element * fill, ** reffill;

	iret= parse_art(name, & tmp);
		if (iret) return iret;

	CLO(ptr-> cam, tmp.cam);
	memcpy(& (ptr-> setup), & (tmp.setup), sizeof(ts_setup));
	CLO(ptr-> chrs, tmp.chrs);
	memcpy(& (ptr-> tracks), & (tmp.tracks), sizeof(ts_track));
	CLO(ptr-> mat, tmp.mat);
	memcpy(& (ptr-> memory), & (tmp.memory), sizeof(ts_mem));

	ptr-> elem.ctlight= ptr-> elem.ctobject= 0;
	ptr-> elem.lights= NULL, ptr-> elem.objects= NULL;

	if (! tmp.elem.count)
		{ return 0; }

	for (istep= tmp.elem.count, step= tmp.elem.list; (istep --); step ++)
	{
		iret= copyElement(& fill, step);
		if (iret)
			return iret;

		insertElement(fill, ptr, 0);
	}

	ptr-> set.count= 0 ;
	ptr-> set.list= reffill= EMEM(tmp.set.count, tlist_element *);
	if (NULL == reffill)
		return 1;

	iret= 0;
	for (istep= tmp.set.count, refstep= tmp.set.list; (istep --); refstep ++)
	{
		for (fill= ptr-> elem.objects;
			(fill && strcmp(fill-> name, (** refstep).name)); fill= fill-> next ) ;
		if (! fill) continue ;

		iret ++;
		*(reffill ++)= fill;
	}
	ptr-> set.count= iret;

	free(tmp.elem.list);
	return 0;
}

void release_artlist(tlist_scene * ptr)
{
}
