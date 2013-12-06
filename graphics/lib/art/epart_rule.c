#include "basic.h"
#include "tscene.h"
#include "tscenelink.h"
#include "eart.h"
#include "epart_word.h"
#include "epart_post.h"

	/*** allocation counts:
		camera, lights+objects, worldgrid, surfaces
		number of tracks, depth of tracks.
	*/

unsigned int		eact_view, eact_character,
					eact_element, eact_worgroup,
					eact_material, eactm_map, eactm_mimage,
					eact_elemsys;
unsigned long		eactl_track, eactl_space, eactl_deep;

extern	int		epart_holdc;
extern	char	epart_hold[];

	/********/

thing TRLspan
{
	char name[32];
	int depth;
	struct TRLspan *next;
} trl_span;

static tscene	*master;
static trl_span	*tlslist, *tlsend;
static te_system	*elcmuse;

static char		activecameraname[64];

	/********/

		/** confirm N non-keywords */
int	epart_checkparam(int n)
{
	long i, av;
	hcUSE
	while (n --)
		{ SCAN;  if (!i || av) RE  hcUSE }
	return 0;
}
		/** confirm N values */
int	epart_checkval(int n)
{
	long i, av;
	hcUSE
	while (n --)
		{ SCAN;  if (! isNUM) RE  hcUSE }
	return 0;
}

	/********/

static trl_span * trl_new(void)
{
	trl_span *ptr;
	ptr= EMEM(1, trl_span);  if (NULL == ptr) return NULL;
	if (tlslist) { tlsend-> next= ptr, tlsend= ptr; }
		else { tlslist= tlsend= ptr; }

	return ptr;
}

static tscene_element * find_element(char *name)
{
	int istep;
	tscene_element *osear;

	for (istep= eact_element, osear= (tscene_element *) master-> memory.b1;
		(istep && strcmp(name, osear-> name)); osear ++) istep -- ;
	if (! istep) return NULL;
	return osear;
}

	/********/

static int	chek_post(int span, int depth, char *name)
{
	trl_span *rec;

	rec= trl_new();
	if (NULL == rec) RE
	eactl_track += span,
	eactl_space += span * depth;

	if (depth > eactl_deep) eactl_deep= depth;
	strcpy(rec-> name, name);
	rec-> depth= depth;
	if (tlslist) { tlsend-> next= rec, tlsend= rec; }
		else { tlslist= tlsend= rec; }
	return 0;
}

static int	chek_import(void)
{
	long i, av;
	int typ= 0;
	char c;

	hcSCAN ;
	if ('-' == * epart_hold)
	{
		c= epart_hold[1];
		if ('w' == c) typ= CAM_WVM ;
			else if ('i' == c) typ= CAM_INC ;
			else typ= CAM_CAM ;
		hcSCAN ;
	}
		else typ= CAM_CAM ;

	hcSCAN ;
	if (isINT && (CAM_CAM == typ))
	{
		hcUSE
	}

	return 0;
}

static int	chek_view(void)
{
	long i, av;
	unsigned int cdpth;
	char tmpname[32];

	SCAN;
	if (i < 3)
		{ printf(" >> camera name must identified\n");  RE }
	strcpy(tmpname, epart_hold);
	hcUSE
	cdpth= 0;

	while (SCAN)
	{
		if (3 != i) RE
		switch (av)
		{
			case _SIZE:
			case _SCENE_ASPECT:
				if ( epart_checkval(2)) RE
				break;
			case _SCENE_WINDOW:
				if ( epart_checkval(4)) RE
				break;
			case _SCENE_WIDEVIEW:
				hcUSE
				break;
			case _SCENE_IMPORT:
				hcUSE
				if (chek_import()) RE
				break;
			case _MOTION:
				i= epartck_motion();
				if (i > cdpth) cdpth= i;
				if (! cdpth) RE
				break;

			case _SCENE_VIEW:
			case _SCENE_CHARACTER:
			case _SCENE_SETUP:
			case _SCENE_LIGHT:
			case _SCENE_SPOT:
			case _SCENE_BEAM:
			case _SCENE_OBJECT:
			case _SCENE_MATERIAL:
			case _SCENE_WORLD:
				if (! cdpth) RE
				return chek_post(10, cdpth, tmpname);
			default:
				RE
		}
	}

	return chek_post(10, cdpth, tmpname);
}

static void	default_view(tscene_camera *ptr)
{
	bzero(ptr, sizeof(tscene_camera));
	ptr-> x= 256,
	ptr-> y= 256;
}

static void	final_view(tscene_camera *ptr)
{
	ul attr;
	float ftmp;

	attr= ptr-> attr;
	if (attr & TSC_ASPECT)
	{
		if (ptr-> y > 0) ftmp= ptr-> gaspect * ptr-> x / ptr-> y;
			else ftmp= 1.0;
		ptr-> aspect= ftmp;
	}
	epartru_post(MO_CAMERA, ptr-> motion);

	tlsend= tlsend-> next;
}

static int	rule_import(tsrc_import * ptr)
{
	long i, av;
	int typ= 0;
	char c;

	hcSCAN ;
	if ('-' == * epart_hold)
	{
		c= epart_hold[1];
		if ('w' == c) typ= CAM_WVM ;
			else if ('i' == c) typ= CAM_INC ;
			else typ= CAM_CAM ;
		hcSCAN ;
	}
		else typ= CAM_CAM ;

	ptr-> type= typ;
	strncpy(ptr-> srcfile, epart_hold, sizeof(ptr-> srcfile));
	hcSCAN ;

	if (isINT && (CAM_CAM == typ))
	{
		ptr-> aux= AHOLDI ;
		hcUSE
	}
		else ptr-> aux= 0;

	ptr-> n= 0, ptr-> width= 0;
	ptr-> data= NULL ;

	return 1;
}

static int	test_view(tscene_camera *ptr)
{
	long i, av;
	int ia, ib, ic, id;
	float va, vb;
	tsrc_import tmp_import ;

	SCAN;
	if (i < 3) return 1;
	strcpy(ptr-> name, epart_hold);
	hcUSE

	while (SCAN)
	{
		if (3 != i) RE
		switch (av)
		{
			case _SIZE:
				hcSCAN;  ia= AHOLDI;
				hcSCAN;  ib= AHOLDI;
				hcUSE
				if (ia && ib)
					{ ptr-> x= ia, ptr-> y= ib; }
				break;
			case _SCENE_ASPECT:
				hcSCAN;  va= AHOLDI;
				hcSCAN;  vb= AHOLDI;
				hcUSE
				if ((va > 0.0) && (vb > 0.0))
				{
					ptr-> attr |= TSC_ASPECT;
					ptr-> aspect_width= va,
					ptr-> aspect_height= vb;
					ptr-> gaspect= va / vb;
				}
				break;
			case _SCENE_WINDOW:
				hcSCAN;  ia= AHOLDI;
				hcSCAN;  ib= AHOLDI;
				hcSCAN;  ic= AHOLDI;
				hcSCAN;  id= AHOLDI;
				if (ia && ib && ic && id && (ib > ia) && (id > ic))
				{
					ptr-> attr |= TSC_WINDOW;
					ptr-> wxa= ia, ptr-> wxb= ib,
					ptr-> wya= ic, ptr-> wyb= id;
				}
				hcUSE
				break;
			case _SCENE_WIDEVIEW:
				hcUSE
				ptr-> attr |= TSC_WIDEVIEW;
				break;
			case _SCENE_IMPORT:
				if (rule_import(& tmp_import))
				{
					ptr-> import= EME(tsrc_import);
					if (ptr-> import)
					{
						* (ptr-> import)= tmp_import;
						ptr-> attr |= TSC_IMPORT;
					}
				}
				break;
			case _MOTION:
				epartck_motion();
				break;

			case _SCENE_VIEW:
			case _SCENE_CHARACTER:
			case _SCENE_SETUP:
			case _SCENE_LIGHT:
			case _SCENE_SPOT:
			case _SCENE_BEAM:
			case _SCENE_OBJECT:
			case _SCENE_MATERIAL:
			case _SCENE_WORLD:
				return 0;
			default:
				RE
		}
	}

	return 0;
}

static int	rule_view(tscene_camera *ptr)
{
	long i, av;
	int ia, ib, ic, id;
	float va, vb;
	tsrc_import tmp_import ;

	SCAN;
	if (i < 3) return 1;
	if (strcmp(tlsend-> name, epart_hold))
		RE
	strcpy(ptr-> name, epart_hold);
	hcUSE

	while (SCAN)
	{
		if (3 != i) RE
		switch (av)
		{
			case _SIZE:
				hcSCAN;  ia= AHOLDI;
				hcSCAN;  ib= AHOLDI;
				hcUSE
				if (ia && ib)
					{ ptr-> x= ia, ptr-> y= ib; }
				break;
			case _SCENE_ASPECT:
				hcSCAN;  va= AHOLDI;
				hcSCAN;  vb= AHOLDI;
				hcUSE
				if ((va > 0.0) && (vb > 0.0))
				{
					ptr-> attr |= TSC_ASPECT;
					ptr-> aspect_width= va,
					ptr-> aspect_height= vb;
					ptr-> gaspect= va / vb;
				}
				break;
			case _SCENE_WINDOW:
				hcSCAN;  ia= AHOLDI;
				hcSCAN;  ib= AHOLDI;
				hcSCAN;  ic= AHOLDI;
				hcSCAN;  id= AHOLDI;
				if (ia && ib && ic && id && (ib > ia) && (id > ic))
				{
					ptr-> attr |= TSC_WINDOW;
					ptr-> wxa= ia, ptr-> wxb= ib,
					ptr-> wya= ic, ptr-> wyb= id;
				}
				hcUSE
				break;
			case _SCENE_WIDEVIEW:
				hcUSE
				ptr-> attr |= TSC_WIDEVIEW;
				break;
			case _SCENE_IMPORT:
				if (rule_import(& tmp_import))
				{
					ptr-> import= EME(tsrc_import);
					if (ptr-> import)
					{
						* (ptr-> import)= tmp_import;
						ptr-> attr |= TSC_IMPORT;
					}
				}
				break;
			case _MOTION:
				ia= epartru_motion(MO_CAMERA, & (ptr-> motion), tlsend-> depth);
					if (ia) return ia;
				break;

			case _SCENE_VIEW:
			case _SCENE_CHARACTER:
			case _SCENE_SETUP:
			case _SCENE_LIGHT:
			case _SCENE_SPOT:
			case _SCENE_BEAM:
			case _SCENE_OBJECT:
			case _SCENE_MATERIAL:
			case _SCENE_WORLD:
				final_view(ptr);
				return 0;
			default:
				RE
		}
	}

	final_view(ptr);
	return 0;
}

static tscene_camera * locate_view(void)
{
	int istep;
	tscene_camera *step;

	for (istep= eact_view, step= (tscene_camera *) master-> memory.a1;
		(istep && step && strcmp(step-> name, epart_hold)); istep --, step ++) ;
	if (istep) return step;
	return NULL;
}

static int	chek_character(void)
{
	if (epart_checkparam( 1)) RE
	return 0;
}

static int	rule_character(tscene_character * ptr)
{
	long i, av;

	hcSCAN;
	strncpy(ptr-> name, epart_hold, sizeof(ptr-> name));
	hcUSE
	return 0;
}

static int	chek_setup(void)
{
	long i, av;

	while (SCAN)
	{
		if (3 != i) RE
		switch (av)
		{
			case _SCENE_ADAPT:
			case _SCENE_GAMMA:
				if ( epart_checkval(1)) RE
				break;
			case _SCENE_DEPTH:
				if ( epart_checkval(2)) RE
				break;
			case _SCENE_ACTIVE:
				hcSCAN;
				if (!i || av) RE
				strcpy(activecameraname, epart_hold);
				hcUSE
				break;
			case _SCENE_IMAGE:
				if ( epart_checkparam(2)) RE
				SCAN;
				if isTOK(_SCENE_FLIP)
					{ hcUSE }
				break;
			case _SCENE_BACKGR:
				hcSCAN ;
				if isTOK(_COLOR)
				{
					if (epart_checkparam(3)) RE
					hcUSE
				}
				else
				if isTOK(_SCENE_IMAGE)
				{
					hcSCAN;
					if (av) RE
					hcUSE
				}
					else RE
				break;
			case _SCENE_PATH:
				if (epart_checkparam(1)) RE
				hcUSE
				break;
			case _SCENE_POST:
				hcSCAN ;
				hcSCAN ;
				if ((7 == i) && ('{' == * epart_hold))
					{ hcUSE ;  flush_block() ; }
				break ;
			case _AMBIENT:
				if ( epart_checkval(3)) RE
				break;
			case _SCENE_FILTER:
				hcSCAN;
				switch (av)
				{
					case _SCENE_BOX:
					case _SCENE_GAUSS:
					case _SCENE_LANCZOS:
					case _SCENE_TRI:
						break;
					default:
						RE
				}
				hcSCAN;
				if (! isNUM) RE
				hcUSE
				break;

			case _SCENE_VIEW:
			case _SCENE_CHARACTER:
			case _SCENE_SETUP:
			case _SCENE_LIGHT:
			case _SCENE_SPOT:
			case _SCENE_BEAM:
			case _SCENE_OBJECT:
			case _SCENE_MATERIAL:
			case _SCENE_WORLD:
				return 0;
			default:
				RE
		}
	}

	return 0;
}

static void default_setup(ts_setup *ptr)
{
	bzero(ptr, sizeof(ts_setup));

	ptr-> rdepth= 1, ptr-> gdepth= 2;
	ptr-> filtertype= 1;
	ptr-> ambient.r= ptr-> ambient.g= ptr-> ambient.b= 0.33;

	ptr-> image.iformat= EV_TGA | EVT_TP10 | EVT_BT24;
	strcpy(ptr-> image.imageform, "test.tga");
}

static void	rule_setup(ts_setup *ptr)
{
	long i, av;
	int ia, ib, ic;
	float va, vb, vc;
	tss_exec * exrec, * exlink ;

	while (SCAN)
	{
		if (3 != i) return;
		switch (av)
		{
			case _SCENE_ADAPT:
				hcSCAN;  ia= AHOLDI;
				hcUSE
				if (ia > 0)
					ptr-> adapt= ia;
				break;
			case _SCENE_GAMMA:
				hcSCAN;  va= AHOLDF;
				hcUSE
				if (va > 0.0)
				{
					ptr-> attr|= TSS_GAMMA,
					ptr-> gamma= va;
				}
				break;
			case _SCENE_DEPTH:
				hcSCAN;  ia= AHOLDI;
				hcSCAN;  ib= AHOLDI;
				hcUSE
				if (ia >= 0)
					ptr-> rdepth= ia;
				if (ib >= 0)
					ptr-> gdepth= ib;
				break;
			case _SCENE_ACTIVE:
				hcSCAN;  hcUSE
				ptr-> curcam= locate_view();
				break;
			case _SCENE_IMAGE:
				hcSCAN; ia= tart_seqname(ptr-> image.imageform, & ic, epart_hold);
				hcSCAN; ib= ato_imagecode(epart_hold);
				hcSCAN;
				if isTOK(_SCENE_FLIP)
				{
					ib |= EV_FLIP;
					hcUSE
				}
				if (ia && ib)
				{
					ptr-> image.icode= ia,
					ptr-> image.ioffset= ic,
					ptr-> image.iformat= ib;
				}
				break;
			case _SCENE_BACKGR:
				hcSCAN ;
				if isTOK(_COLOR)
				{
					hcSCAN;  va= AHOLDF;
					hcSCAN;  vb= AHOLDF;
					hcSCAN;  vc= AHOLDF;
					hcUSE
					ptr-> bg.attr= 1,
					ptr-> bg.cbase.r= va,
					ptr-> bg.cbase.g= vb,
					ptr-> bg.cbase.b= vc;
				}
				else
				if isTOK(_SCENE_IMAGE)
				{
					hcSCAN;
					strcpy(ptr-> bg.imagename, epart_hold);
					hcUSE
					ptr-> bg.attr= 2;
				}
				break;
			case _SCENE_PATH:
				hcSCAN;
				strcpy(ptr-> path, epart_hold);
				hcUSE
				break;
			case _SCENE_POST:
				hcSCAN ;
				exrec= EME(tss_exec) ;
				if (NULL == exrec)
				{
					hcSCAN ;  hcUSE
					flush_block() ;
					break ;
				}

				exrec-> type= 1 ;
				exrec-> next= NULL ;
				strncpy(exrec-> cmdword, epart_hold, sizeof(exrec-> cmdword)) ;
				hcSCAN ;
				if ((7 == i) && ('{' == * epart_hold))
				{
					hcUSE
					i= post_read(exrec-> cmdform, sizeof(exrec-> cmdform)) ;
					if (i) { EFREE(exrec);  break; }
				}
					else * exrec-> cmdform= '\0' ;

				for (exlink= ptr-> cmdlist;
					(exlink && exlink-> next); exlink= exlink-> next ) ;
				if (exlink) exlink-> next= exrec;
					else ptr-> cmdlist= exrec ;
				break ;
			case _AMBIENT:
				hcSCAN;  va= AHOLDF;
				hcSCAN;  vb= AHOLDF;
				hcSCAN;  vc= AHOLDF;
				hcUSE
				if ((va > -1.0) && (vb > -1.0) && (vc > -1.0))
				{
					ptr-> ambient.r= va,
					ptr-> ambient.g= vb,
					ptr-> ambient.b= vc;
				}
				break;
			case _SCENE_FILTER:
				hcSCAN;
				switch (av)
				{
					case _SCENE_BOX:  ia= _F_BOX;  break;
					case _SCENE_GAUSS:  ia= _F_GAUSS;  break;
					case _SCENE_LANCZOS:  ia= _F_LANCZOS;  break;
					case _SCENE_TRI:  ia= _F_TRI;  break;
					default:
						ia= 0;
				}
				hcSCAN;
				ib= AHOLDI;
				hcUSE
				if (ia && (ib >= 0))
				{
					ptr-> filtertype= ia,
					ptr-> filterwidth= ib;
				}
				break;

			default:
				return;
		}
	}

}

	/************/

static int	chek_falloff(void)
{
	long i, av;

	hcSCAN;
	if (3 != i) RE
	switch (av)
	{
		case _SCENE_FLAT:
		case _SCENE_LINEAR:
		case _SCENE_REAL:
			if (epart_checkval(1)) RE
			hcUSE
			break;
		case _SCENE_COMPLEX:
			if (epart_checkval(3)) RE
			hcUSE
			break;
		default:
			RE
	}
	return 0;
}

static int	rule_falloff(svector *param)
{
	long i, av;
	float da, db, dc;

	hcSCAN;
	switch (av)
	{
		case _SCENE_FLAT:
			hcSCAN;
			da= AHOLDF;
			if (da > 0.0)
			{
				hcUSE
				param-> x= da;
				return 1;
			}
			break;
		case _SCENE_LINEAR:
			hcSCAN;
			da= AHOLDF;
			if (da > 0.0)
			{
				hcUSE
				param-> y= da;
				return 2;
			}
			break;
		case _SCENE_REAL:
			hcSCAN;
			da= AHOLDF;
			if (da > 0.0)
			{
				hcUSE
				param-> z= da;
				return 4;
			}
			break;
		case _SCENE_COMPLEX:
			hcSCAN;  da= AHOLDF;
			hcSCAN;  db= AHOLDF;
			hcSCAN;  dc= AHOLDF;
			hcUSE
			if ((da >= 0.0) && (db >= 0.0) && (dc >= 0.0) && ((da + db + dc) > 0.0))
			{
					/** set complex **/
				param-> x= da,
				param-> y= db,
				param-> z= dc;
				return 7;
			}
			break;
	}
	return 0;
}

static int	chek_light(void)
{
	long i, av;
	unsigned int cdpth;
	char tmpname[32];

	SCAN;
	if (i < 3)
		{ printf(" >> light name must identified\n");  RE }
	strcpy(tmpname, epart_hold);
	hcUSE
	cdpth= 0;

	while (SCAN)
	{
		if (3 != i) RE
		switch (av)
		{
			case _SCENE_BRIGHT:
			case _SCENE_BUFTOL:
			case _SAFE:
				if ( epart_checkval(1)) RE
				break;
			case _SCENE_SHADOW:
				hcUSE
				break;
			case _SCENE_FALLOFF:
				if ( chek_falloff()) RE
				break;
			case _SCENE_PARENT:
				if ( epart_checkparam(1)) RE
				break;
			case _MOTION:
				i= epartck_motion();
				if (i > cdpth) cdpth= i;
				if (! cdpth) RE
				break;

			case _SCENE_VIEW:
			case _SCENE_CHARACTER:
			case _SCENE_SETUP:
			case _SCENE_LIGHT:
			case _SCENE_SPOT:
			case _SCENE_BEAM:
			case _SCENE_OBJECT:
			case _SCENE_MATERIAL:
			case _SCENE_WORLD:
				if (! cdpth) RE
				return chek_post(6, cdpth, tmpname);
			default:
				RE
		}
	}

	if (! cdpth) RE
	return chek_post(6, cdpth, tmpname);
}

static void	default_lite(tscene_element *ptr)
{
	bzero(ptr, sizeof(tscene_element));

	ptr-> dat.light.bright= 1.0;
}

static int	rule_light(tscene_element *ptr)
{
	long i, av;
	int ia;
	float va;
	tscene_element *tepar;

	SCAN;
	if (strcmp(tlsend-> name, epart_hold)) RE
	strcpy(ptr-> name, epart_hold);
	hcUSE

	while (SCAN)
	{
		if (3 != i) RE
		switch (av)
		{
			case _SCENE_BRIGHT:
				hcSCAN;  va= AHOLDF;
				hcUSE
				if (va > -1.0)
					{ ptr-> dat.light.bright= va; }
				break;
			case _SAFE:
				hcSCAN;  va= AHOLDF;
				hcUSE
				if (va > 1e-10)
				{
					ptr-> attr |= TSL_SAFE;
					ptr-> dat.light.safe= va;
				}
				break;
			case _SCENE_SHADOW:
				hcUSE
				ptr-> attr |= TSL_SHADOW;
				break;
			case _SCENE_FALLOFF:
				ia= rule_falloff(& (ptr-> dat.light.fall));
				if (! ia) RE
				ptr-> attr |= TSL_FALLOFF | ia;
				break;
			case _SCENE_PARENT:
				hcSCAN;
				tepar= find_element(epart_hold);
				if (NULL == tepar) RE
				hcUSE
				ptr-> parent= tepar;
				break;
			case _MOTION:
				ia= epartru_motion(MO_LIGHT, & (ptr-> motion), tlsend-> depth);
					if (ia) return ia;
				break;

			default:
				ptr-> type= _E_LIGHT;
				epartru_post(MO_LIGHT, ptr-> motion);
				tlsend= tlsend-> next;
				return 0;
		}
	}

	ptr-> type= _E_LIGHT;
	epartru_post(MO_LIGHT, ptr-> motion);
	return 0;
}

static int	chek_spot(void)
{
	long i, av;
	unsigned int cdpth;
	char tmpname[32];

	SCAN;
	if (i < 3)
		{ printf(" >> spot name must identified\n");  RE }
	strcpy(tmpname, epart_hold);
	hcUSE
	cdpth= 0;

	while (SCAN)
	{
		if (3 != i) RE
		switch (av)
		{
			case _SCENE_BRIGHT:
			case _SCENE_BUFTOL:
			case _SAFE:
			case _SCENE_CONE:
			case _SCENE_POWER:
				if ( epart_checkval(1)) RE
				break;
			case _SCENE_SHADOW:
				hcUSE
				break;
			case _SCENE_FALLOFF:
				if ( chek_falloff()) RE
				break;
			case _SCENE_BUFFER:
			case _SCENE_SOFT:
				if ( epart_checkval(2)) RE
				break;
			case _SCENE_PARENT:
				if ( epart_checkparam(1)) RE
				break;
			case _MOTION:
				i= epartck_motion();
				if (i > cdpth) cdpth= i;
				if (! cdpth) RE
				break;

			case _SCENE_VIEW:
			case _SCENE_CHARACTER:
			case _SCENE_SETUP:
			case _SCENE_LIGHT:
			case _SCENE_SPOT:
			case _SCENE_BEAM:
			case _SCENE_OBJECT:
			case _SCENE_MATERIAL:
			case _SCENE_WORLD:
				if (! cdpth) RE
				return chek_post(10, cdpth, tmpname);
			default:
				RE
		}
	}

	if (! cdpth) RE
	return chek_post(10, cdpth, tmpname);
}

static int	rule_spot(tscene_element *ptr)
{
	long i, av;
	int ia, ib;
	float va;
	tscene_element *tepar;

	SCAN;
	if (strcmp(tlsend-> name, epart_hold)) RE
	strcpy(ptr-> name, epart_hold);
	hcUSE

	while (SCAN)
	{
		if (3 != i) RE
		switch (av)
		{
			case _SCENE_BRIGHT:
				hcSCAN;  va= AHOLDF;
				hcUSE
				if (va > -1.0)
					{ ptr-> dat.light.bright= va; }
				break;
			case _SCENE_CONE:
				hcSCAN;  va= AHOLDF;
				hcUSE
				if (va > 0.0)
				{
					if (va > 1.0) va= 1.0;
					ptr-> attr |= TSLS_CONE;
					ptr-> dat.light.atten= va;
				}
				break;
			case _SCENE_POWER:
				hcSCAN;  va= AHOLDF ;
				hcUSE
				if (va > 0.0)
				{
					ptr-> attr |= TSLS_POWER;
					ptr-> dat.light.atten= va;
				}
				break ;
			case _SCENE_SHADOW:
				hcUSE
				ptr-> attr |= TSL_SHADOW;
				break;
			case _SCENE_BUFFER:
				hcSCAN;  ia= AHOLDI;
				hcSCAN;  ib= AHOLDI;
				hcSCAN;
				if ((ia > 0) && (ib > 0))
				{
					ptr-> attr |= TSL_BUFFER;
					ptr-> dat.light.bufferx= ia,
					ptr-> dat.light.buffery= ib,
					ptr-> dat.light.buffertol= 0.02;
				}
				break;
			case _SCENE_BUFTOL:
				hcSCAN;  va= AHOLDF;
				hcUSE
				if (va > 0.0)
					{ ptr-> dat.light.buffertol= va; }
				break;
			case _SCENE_SOFT:
				hcSCAN;  ia= AHOLDI;
				hcSCAN;  va= AHOLDF;
				hcUSE
				if (ia && (va >= 0.0))
				{
					ptr-> attr |= TSL_SOFT;
					ptr-> dat.light.softrad= ia,
					ptr-> dat.light.soft= va;
				}
				break;
			case _SAFE:
				hcSCAN;  va= AHOLDF;
				hcUSE
				if (va > 1e-10)
				{
					ptr-> attr |= TSL_SAFE;
					ptr-> dat.light.safe= va;
				}
				break;
			case _SCENE_FALLOFF:
				ia= rule_falloff(& (ptr-> dat.light.fall));
				if (! ia) RE
				ptr-> attr |= TSL_FALLOFF | ia;
				break;
			case _SCENE_PARENT:
				hcSCAN;
				tepar= find_element(epart_hold);
				if (NULL == tepar) RE
				hcUSE
				ptr-> parent= tepar;
				break;
			case _MOTION:
				ia= epartru_motion(MO_SPOT, & (ptr-> motion), tlsend-> depth);
					if (ia) return ia;
				break;

			default:
				ptr-> type= _E_SPOT;
				epartru_post(MO_SPOT, ptr-> motion);
				tlsend= tlsend-> next;
				return 0;
		}
	}

	ptr-> type= _E_SPOT;
	epartru_post(MO_SPOT, ptr-> motion);
	return 0;
}

static int	chek_beam(void)
{
	long i, av;
	unsigned int cdpth;
	char tmpname[32];

	SCAN;
	if (i < 3)
		{ printf(" >> beam name must identified\n");  RE }
	strcpy(tmpname, epart_hold);
	hcUSE
	cdpth= 0;

	while (SCAN)
	{
		if (3 != i) RE
		switch (av)
		{
			case _SCENE_BRIGHT:
			case _SCENE_BUFTOL:
			case _SAFE:
				if ( epart_checkval(1)) RE
				break;
			case _SCENE_SHADOW:
				hcUSE
				break;
			case _SCENE_BUFFER:
			case _SCENE_SOFT:
				if ( epart_checkval(2)) RE
				break;
			case _SCENE_PARENT:
				if ( epart_checkparam(1)) RE
				break;
			case _MOTION:
				i= epartck_motion();
				if (i > cdpth) cdpth= i;
				if (! cdpth) RE
				break;

			case _SCENE_VIEW:
			case _SCENE_CHARACTER:
			case _SCENE_SETUP:
			case _SCENE_LIGHT:
			case _SCENE_SPOT:
			case _SCENE_BEAM:
			case _SCENE_OBJECT:
			case _SCENE_MATERIAL:
			case _SCENE_WORLD:
				if (! cdpth) RE
				return chek_post(6, cdpth, tmpname);
			default:
				RE
		}
	}

	if (! cdpth) RE
	return chek_post(6, cdpth, tmpname);
}

static int	rule_beam(tscene_element *ptr)
{
	long i, av;
	int ia, ib;
	float va;
	tscene_element *tepar;

	SCAN;
	if (strcmp(tlsend-> name, epart_hold)) RE
	strcpy(ptr-> name, epart_hold);
	hcUSE

	while (SCAN)
	{
		if (3 != i) RE
		switch (av)
		{
			case _SCENE_BRIGHT:
				hcSCAN;  va= AHOLDF;
				hcUSE
				if (va > -1.0)
					{ ptr-> dat.light.bright= va; }
				break;
			case _SCENE_SHADOW:
				hcUSE
				ptr-> attr |= TSL_SHADOW;
				break;
			case _SCENE_BUFFER:
				hcSCAN;  ia= AHOLDI;
				hcSCAN;  ib= AHOLDI;
				hcSCAN;
				if ((ia > 0) && (ib > 0))
				{
					ptr-> attr |= TSL_BUFFER;
					ptr-> dat.light.bufferx= ia,
					ptr-> dat.light.buffery= ib,
					ptr-> dat.light.buffertol= 0.02;
				}
				break;
			case _SCENE_BUFTOL:
				hcSCAN;  va= AHOLDF;
				hcUSE
				if (va > 0.0)
					{ ptr-> dat.light.buffertol= va; }
				break;
			case _SCENE_SOFT:
				hcSCAN;  ia= AHOLDI;
				hcSCAN;  va= AHOLDF;
				hcUSE
				if (ia && (va >= 0.0))
				{
					ptr-> attr |= TSL_SOFT;
					ptr-> dat.light.softrad= ia,
					ptr-> dat.light.soft= va;
				}
				break;
			case _SAFE:
				hcSCAN;  va= AHOLDF;
				hcUSE
				if (va > 1e-10)
				{
					ptr-> attr |= TSL_SAFE;
					ptr-> dat.light.safe= va;
				}
				break;
			case _SCENE_PARENT:
				hcSCAN;
				tepar= find_element(epart_hold);
				if (NULL == tepar) RE
				hcUSE
				ptr-> parent= tepar;
				break;
			case _MOTION:
				ia= epartru_motion(MO_BEAM, & (ptr-> motion), tlsend-> depth);
					if (ia) return ia;
				break;

			default:
				ptr-> type= _E_BEAM;
				epartru_post(MO_BEAM, ptr-> motion);
				tlsend= tlsend-> next;
				return 0;
		}
	}

	ptr-> type= _E_BEAM;
	epartru_post(MO_BEAM, ptr-> motion);
	return 0;
}

static int	check_obj(void)
{
	long i, av;
	unsigned int cdpth;
	char tmpname[32];

	SCAN;
	if (i < 3)
		{ printf(" >> object name must identified\n");  RE }
	strcpy(tmpname, epart_hold);
	hcUSE
	cdpth= 0;

	while (SCAN)
	{
		if (3 != i) RE
		switch (av)
		{
			case _SCENE_USE:
			case _SCENE_PARENT:
			case _SCENE_ROOT:
				if ( epart_checkparam(1)) RE
				break;
			case _MOTION:
				i= epartck_motion();
				if (i > cdpth) cdpth= i;
				if (! cdpth) RE
				break;
			case _SCENE_SERV:
				if ( epart_checkparam(5)) RE
				eact_elemsys ++;
				break;
			case _INVIS:
				hcUSE ;
				break ;

			case _SCENE_VIEW:
			case _SCENE_CHARACTER:
			case _SCENE_SETUP:
			case _SCENE_LIGHT:
			case _SCENE_SPOT:
			case _SCENE_BEAM:
			case _SCENE_OBJECT:
			case _SCENE_MATERIAL:
			case _SCENE_WORLD:
				if (! cdpth) RE
				return chek_post(10, cdpth, tmpname);
			default:
				RE
		}
	}

	if (! cdpth) RE
	return chek_post(10, cdpth, tmpname);
}

static void	default_obj(tscene_element *ptr)
{
	bzero(ptr, sizeof(tscene_element));
	ptr-> dat.object.icode= 1;
	strcpy(ptr-> dat.object.srcnameform, "test.bf");
	* ptr-> dat.object.srcroot= '\0';
}

static int	rule_object(tscene_element *ptr)
{
	long i, av;
	int ia, ib;
	tscene_element *tepar;

	SCAN;
	if (strcmp(tlsend-> name, epart_hold)) RE
	strcpy(ptr-> name, epart_hold);
	hcUSE

	while (SCAN)
	{
		if (3 != i) return 0;
		switch (av)
		{
			case _SCENE_USE:
				hcSCAN;
				ia= tart_seqname(ptr-> dat.object.srcnameform, & ib, epart_hold);
				if (ia < 1) RE
				hcUSE
				ptr-> dat.object.icode= ia,
				ptr-> dat.object.ioffset= ib;
				break;
			case _SCENE_PARENT:
				hcSCAN;
				tepar= find_element(epart_hold);
				if (NULL == tepar) RE
				hcUSE
				ptr-> parent= tepar;
				break;
			case _SCENE_ROOT:
				hcSCAN;
				strcpy(ptr-> dat.object.srcroot, epart_hold);
				hcUSE
				break;
			case _SCENE_SERV:
				if (NULL == elcmuse) RE
				hcSCAN;
				strcpy(elcmuse-> cmdstring, epart_hold);
				hcSCAN;
				if (! atoi(epart_hold))
				{
					strcat(elcmuse-> cmdstring, " ");
					strcat(elcmuse-> cmdstring, epart_hold);
					hcSCAN;
				}
				elcmuse-> cmdport= atoi(epart_hold);
				if (! elcmuse-> cmdport)
					{ printf(" >> Object serv port zero\n");  RE }
				hcSCAN;
				strcpy(elcmuse-> cmdparams, epart_hold);
				hcSCAN;
				if (! av)
				{
					strcat(elcmuse-> cmdparams, " ");
					strcat(elcmuse-> cmdparams, epart_hold);
				}
				hcUSE
				ptr-> attr |= TSO_CMD;
				ptr-> cmd= elcmuse ++;
				break;
			case _INVIS:
				hcUSE ;
				ptr-> attr |= TSO_INVIS ;
				break ;
			case _MOTION:
				ia= epartru_motion(MO_OBJECT, & (ptr-> motion), tlsend-> depth);
					if (ia) return ia;
				break;
			default:
				ptr-> type= _E_OBJECT;
				epartru_post(MO_OBJECT, ptr-> motion);
				tlsend= tlsend-> next;
				return 0;
		}
	}

	ptr-> type= _E_OBJECT;
	epartru_post(MO_OBJECT, ptr-> motion);
	tlsend= tlsend-> next;
	return 0;
}

static int	check_material(void)
{
	long i, av;

	hcUSE
	while (SCAN)
	{
		if (3 != i) RE
		switch (av)
		{
			case _SURFACE:
				if ( epart_checkparam(1) || epartck_material()) RE
				eact_material ++;
				break;

			case _SCENE_VIEW:
			case _SCENE_CHARACTER:
			case _SCENE_SETUP:
			case _SCENE_LIGHT:
			case _SCENE_SPOT:
			case _SCENE_BEAM:
			case _SCENE_OBJECT:
			case _SCENE_MATERIAL:
			case _SCENE_WORLD:
				return 0;
			default:
				RE
		}
	}

	return 0;
}

static void	default_material(tscene_surface *ptr)
{
	bzero(ptr, sizeof(tscene_surface));
}

static int	rule_material(tscene_surface **ptr)
{
	long i, av;
	tscene_surface *srffil;

	srffil= *ptr;
	while (SCAN)
	{
		if ((3 != i) || (_SURFACE != av)) break;

		default_material(srffil);
		hcSCAN; strcpy(srffil-> sname, epart_hold); hcUSE
		if ( epartru_material(srffil)) RE
		srffil ++;
		eact_material ++;
	}

	*ptr= srffil;
	return 0;
}

static int	check_world(void)
{
	long i, av;

	hcUSE
	while (SCAN)
	{
		if (3 != i) RE
		switch (av)
		{
			case _SCENE_OB:
				if ( epart_checkparam(1)) RE
				eact_worgroup ++;
				break;

			case _SCENE_VIEW:
			case _SCENE_CHARACTER:
			case _SCENE_SETUP:
			case _SCENE_LIGHT:
			case _SCENE_SPOT:
			case _SCENE_BEAM:
			case _SCENE_OBJECT:
			case _SCENE_MATERIAL:
			case _SCENE_WORLD:
				return 0;
			default:
				RE
		}
	}

	return 0;
}

static void	rule_world(tscene_element **ref)
{
	long i, av;
	tscene_element *teptr;

	hcUSE
	while (SCAN)
	{
		if ((3 != i) || (_SCENE_OB != av)) break;
		hcSCAN;
		teptr= find_element(epart_hold);
		if (NULL == teptr) break;
		*(ref ++)= teptr;
		eact_worgroup ++;
		hcUSE
	}
}

	/********/

static int	check_art(void)
{
	long i, av;

	while (SCAN)
	{
		if (3 != i) RE
		switch (av)
		{
			case _SCENE_VIEW:
				hcUSE
				if (chek_view()) RE
				eact_view ++;
				break;
			case _SCENE_CHARACTER:
				hcUSE
				if (chek_character()) RE
				eact_character ++;
				break;
			case _SCENE_SETUP:
				hcUSE
				if (chek_setup()) RE
				break;
			case _SCENE_LIGHT:
				hcUSE
				if (chek_light()) RE
				eact_element ++;
				break;
			case _SCENE_SPOT:
				hcUSE
				if (chek_spot()) RE
				eact_element ++;
				break;
			case _SCENE_BEAM:
				hcUSE
				if (chek_beam()) RE
				eact_element ++;
				break;
			case _SCENE_OBJECT:
				hcUSE
				if (check_obj()) RE
				eact_element ++;
				break;
			case _SCENE_MATERIAL:
				hcUSE
				if (check_material()) RE
				break;
			case _SCENE_WORLD:
				hcUSE
				if (check_world()) RE
				break;

			default:
				printf(" >> data outside of proper header.\n");
				RE
		}
	}
	return 0;
}

static void	default_art(tscene *ptr)
{
	ptr-> cam.count= 0;
}

static int	rule_art(tscene *dest)
{
	long i, av;

	tscene_camera *camfil;
	tscene_character *charfill;
	tscene_element *elefil;

	camfil= (tscene_camera *) dest-> memory.a1;
	charfill= (tscene_character *) dest-> memory.a2;
	elefil= (tscene_element *) dest-> memory.b1;

	default_setup(& (dest-> setup));

	while (SCAN)
	{
		if (3 != i) return 0;
		switch (av)
		{
			case _SCENE_VIEW:
				hcUSE
				default_view(camfil);
				if ( rule_view(camfil)) RE
				camfil ++,
				eact_view ++;
				break;
			case _SCENE_CHARACTER:
				hcUSE
				if (rule_character(charfill)) RE
				charfill ++;
				eact_character ++;
				break;
			case _SCENE_SETUP:
				hcUSE
				rule_setup(& (dest-> setup));
				break;
			case _SCENE_LIGHT:
				hcUSE
				default_lite(elefil);
				if ( rule_light(elefil)) RE
				elefil ++,
				eact_element ++;
				break;
			case _SCENE_SPOT:
				hcUSE
				default_lite(elefil);
				if ( rule_spot(elefil)) RE
				elefil ++,
				eact_element ++;
				break;
			case _SCENE_BEAM:
				hcUSE
				default_lite(elefil);
				if ( rule_beam(elefil)) RE
				elefil ++,
				eact_element ++;
				break;
			case _SCENE_OBJECT:
				hcUSE
				default_obj(elefil);
				if ( rule_object(elefil)) RE
				elefil ++,
				eact_element ++;
				break;
			case _SCENE_MATERIAL:
				hcUSE
				if ( rule_material((tscene_surface **) & (dest-> memory.c2))) RE
				break;
			case _SCENE_WORLD:
				hcUSE
				rule_world( (tscene_element **) master-> memory.d1);
				break;

			default:
				return 0;
		}
	}
	return 0;
}

static void	test_setup(tscene *dest)
{
	long i, av;

	master-> memory.a1= NULL;
	while (SCAN)
	{
		if (3 == i)
		{
			if (_SCENE_SETUP == av)
				{  hcUSE rule_setup(& (dest-> setup));  continue; }
			if ((_SCENE_VIEW == av) && (dest-> cam.list))
			{
				hcSCAN;
				if (strcmp(epart_hold, activecameraname)) continue;
				test_view(dest-> cam.list);
			}
		}
		hcUSE
	}
}

int	test_art(char *name, tscene *ptr)
{
	if (open_art(name)) RE

	eact_view= 0, eact_character= 0,
	eact_element= 0, eact_worgroup= 0,
	eact_material= 0, eactm_map= 0, eactm_mimage= 0;
	eactl_track= 0, eactl_space= 0, eactl_deep= 0;
	tlslist= tlsend= NULL;

	*activecameraname= '\0';

	if (check_art())
		{ close_art(); printf(" >> failure parsing at %s.\n", epart_hold);  RE }
	if (tlsend) tlsend-> next= NULL;
		else RE
	if (! eact_view || ! eact_element || ! eact_worgroup || ! eactl_track || ! eactl_space) RE

		/** finalize structure */
	ptr-> cam.count= eact_view,
	ptr-> elem.count= eact_element,
	ptr-> mat.count= eact_material,
	ptr-> set.count= eact_worgroup,
	ptr-> tracks.count= eactl_track;
	ptr-> tracks.totalspan= eactl_deep;

	rewind_art();
	master= ptr;

	ptr-> memory.a1= malloc(sizeof(tscene_camera));
	ptr-> cam.list= ptr-> setup.curcam= (tscene_camera *) master-> memory.a1;

	default_art(ptr);
	test_setup(ptr);

	ptr-> setup.curcam= ptr-> cam.list;
	close_art();
	return 0;
}

int	parse_art(char *name, tscene *ptr)
{
	if (open_art(name))
	{
		RE
	}

	eact_view= 0, eact_element= 0, eact_elemsys= 0, eact_worgroup= 0,
	eact_material= 0, eactm_map= 0, eactm_mimage= 0;
	eactl_track= 0, eactl_space= 0;
	tlslist= tlsend= NULL;

	if (check_art())
		{ close_art(); printf(" >> failure parsing at %s.\n", epart_hold);  RE }
	if (tlsend) tlsend-> next= NULL;
		else RE

	if (! eact_view || ! eact_element || ! eact_worgroup || ! eactl_track || ! eactl_space) RE
	if (! eact_material) eact_material ++;

		/** allocation **/
	ptr-> memory.a1= malloc(eact_view * sizeof(tscene_camera));
	ptr-> memory.a2= malloc(eact_character * sizeof(tscene_character));
	ptr-> memory.b1= malloc(eact_element * sizeof(tscene_element));
	ptr-> memory.b2= (eact_elemsys) ? malloc(eact_elemsys * sizeof(te_system)) : NULL;
	ptr-> memory.c1= malloc(eact_material * sizeof(tscene_surface)),
	ptr-> memory.c3= malloc(eactm_map * sizeof(tsc_map)),
	ptr-> memory.c4= malloc(eactm_mimage * sizeof(tsc_texture));
	ptr-> memory.d1= malloc(eact_worgroup * sizeof(ts_element *));
	ptr-> memory.e1= malloc(eactl_track * sizeof(us)),
	ptr-> memory.e2= malloc(eactl_track * sizeof(float *)),
	ptr-> memory.e3= malloc(eactl_space * sizeof(float));

	rewind_art();
	default_art(ptr);

	eact_view= 0, eact_element= 0, eact_elemsys= 0,
	eact_material= 0,
	eact_worgroup= 0;
	tlsend= tlslist;
	ptr-> memory.c2= ptr-> memory.c1;
	elcmuse= (te_system *) ptr-> memory.b2;

		/** setup motion storage pointers */
	epartru_initmotion((us *) ptr-> memory.e1, (float **) ptr-> memory.e2, (float *) ptr-> memory.e3);
		/** setup texture map storage pointers */
	epartsu_installmap((tsc_map *) ptr-> memory.c3, (tsc_texture *) ptr-> memory.c4);

	master= ptr;
	if (rule_art(ptr))
		{ close_art(); printf(" >> failure ruling at %s.\n", epart_hold);  RE }

	if (! eact_material)
		{ /** XXXX create material **/ }

	epartru_settle(eactl_track, (us *) ptr-> memory.e1, (float **) ptr-> memory.e2);

		/** finalize structure */
	ptr-> cam.count= eact_view,
	ptr-> cam.list= (tscene_camera *) master-> memory.a1;
	ptr-> chrs.count= eact_character,
	ptr-> chrs.list= (tscene_character *) master-> memory.a2;
	ptr-> elem.count= eact_element,
	ptr-> elem.list= (tscene_element *) master-> memory.b1;
	ptr-> mat.count= eact_material,
	ptr-> mat.list= (tscene_surface *) master-> memory.c1;
	ptr-> set.count= eact_worgroup,
	ptr-> set.list= (tscene_element **) master-> memory.d1;
	ptr-> tracks.count= eactl_track,
	ptr-> tracks.totalspan= eactl_space,
	ptr-> tracks.depth= (us *) ptr-> memory.e1,
	ptr-> tracks.data= (float **) ptr-> memory.e2;

	ptr-> memory.c2= NULL;

	close_art();
	return 0;
}

void	release_art(tscene *ptr)
{
	if (ptr-> cam.count)
		{ EFREE(ptr-> memory.a1); }
	if (ptr-> chrs.count)
		{ EFREE(ptr-> memory.a2); }
	if (ptr-> elem.count)
		{ EFREE(ptr-> memory.b1);  EUFREE(ptr-> memory.b2); }
	EUFREE(ptr-> memory.c1);
	EUFREE(ptr-> memory.c3);
	EUFREE(ptr-> memory.c4);
	if (ptr-> set.count)
		{ EFREE(ptr-> memory.d1); }

	if (ptr-> tracks.count)
	{
		EFREE(ptr-> memory.e1);
		EFREE(ptr-> memory.e2);
		EFREE(ptr-> memory.e3);
	}

	bzero(ptr, sizeof(tscene));
}

