#include "trun.h"
#include "ebf.h"

long		cobjects;
tr_object	*tolist;

/*
wp_surface	*worksurface;
*/

extern int	global_time;
extern	tlist_scene	tfocus;

	/*** cp_file.o */
extern FILE	*cto_objectopen(te_object *);
extern FILE	*cto_imageopen(char *);
extern FILE	*cto_fileopen(char *);

#define	HSCHECK(xx)	for (hdrt_check= sl, hdro_step= ll; \
					(*hdrt_check!= xx) && (*hdrt_check!= ELOB_END); hdrt_check++, hdro_step++) ; \
					if (*hdrt_check!= xx) RE

static int	cto_newbasis(int count, ev_surfbsi **dest, tr_object *tptr)
{
	ev_surfbsi *data;

	data= EMEM(count, ev_surfbsi);
	if (NULL == data) RE

	*dest= data;
	while (count --)
	{
		data-> mapct= 0,
		data-> mapcomb= 0,
		data-> maplist= NULL;

		data-> global_v= tptr-> ta.vlist -1,
		data-> global_n= tptr-> ta.nlist -1,
		data-> motion_t= NULL;
		data-> shader= NULL ;
		data ++;
	}

	return 0;
}

static int	cto_prepsurf(tr_object *tptr)
{
	ev_surfbsi *magic_basis;
	er_creatsrec synew;

		/*** create empty surface-basis */
	if (cto_newbasis(tptr-> ta.surface_c, &magic_basis, tptr)) RE

		/*** update surface from art file */
	tsurf_patch(tptr);

		/*** build runtime structures **/
	if (tsurf_build(tptr-> ta.slist, tptr-> ta.surface_c, magic_basis, &synew)) RE

	tptr-> ta.sdata.ermap= synew.newmap,
	tptr-> ta.sdata.ermapref= synew.newref,
	tptr-> ta.sdata.ersurf= synew.newsurf,
	tptr-> ta.sdata.erbasis= magic_basis;

	return 0;
}

#define	HREL	EFREE(hdr_types); EFREE(hdr_offset); EFREE(hdrf_offset);

static int	cto_loadb(FILE *f, tr_object *tptr)
{
	if (tobj_load(f, & (tptr-> ta))) { RE }
	if (cto_prepsurf(tptr)) { RE }

	return 0;
}

int	cto_load(void)
{
	char swk[512], *p;
	int istep,
		cobj, couse;
	long addv, maxv, maxs;
	FILE *f;
	tlist_element **uselist, *tsptr;
	tr_object *tbase, *tset;
	bf_container troot;

#if TGEOM_GENERATE
	int iret ;
	long bsz;
	char * dataptr;
#endif

	cobj= tfocus.set.count;
	tbase= EMEM(cobj, tr_object);
	if (NULL == tbase) RE

	tset= tbase;
	for (istep= tfocus.set.count, couse= 0, uselist= tfocus.set.list;
		(istep --); uselist ++)
	{
		tsptr= *uselist;
		tset-> iamatch= tsptr, tset-> ia= 1, tset-> attr= 0, tset ++, couse ++;
	}

	if (couse > cobj)
	{
		printf(" >> unique count error (%d>%d)\n", couse, cobj);
		EFREE(tbase);  RE
	}

	cobjects= couse,
	tolist= tbase;
	maxv= 0, maxs= 0;

	for (istep= cobjects, tset= tbase; (istep --); tset ++)
	{
		tset-> ta.mark= 0,
		tset-> ta.surface_c= 0, tset-> ta.vector_c= 0,
		tset-> ta.normal_c= 0, tset-> ta.poly_c= 0;
		tset-> ta.vlist= NULL, tset-> ta.nlist= NULL,
		tset-> ta.slist= NULL, tset-> ta.ilist= NULL;

#if TGEOM_GENERATE
		tsptr= tset-> iamatch;

		if ((tsptr-> attr & TSO_CMD) && tsptr-> cmd)
		{
			bsz= 0;
			iret= geom_get(tsptr-> name, & bsz, & dataptr);
			if (! iret && bsz)
			{
				tobj_mem(bsz, dataptr, & (tset-> ta));

				EFREE(dataptr);
				cto_prepsurf(tset);


				addv= tset-> ta.vector_c;  if (addv > maxv) maxv= addv;
				addv= tset-> ta.normal_c;  if (addv > maxv) maxv= addv;
				if (tset-> ta.surface_c > maxs) maxs= tset-> ta.surface_c;
				continue ;
			}
		}
#endif
		if (tsptr-> attr & TSO_INVIS)
			tset-> attr|= 0x10 ;

		bzero(& troot, sizeof(bf_container));
		if (* tset-> iamatch-> dat.object.srcroot)
		{
			f= cto_fileopen(tset-> iamatch-> dat.object.srcroot);
			if (NULL != f)
			{
				load_embf(f, & troot);
				if (troot.fptr && troot.fcount)
				{
					tset-> ta.poly_c= troot.fcount,
					tset-> ta.ilist= (us *) troot.fptr;
				}
			}
		}

		f= cto_objectopen(& (tset-> iamatch-> dat.object));
		if (NULL == f)
		{
			strcpy(swk, tset-> iamatch-> dat.object.srcnameform);
			if (p= strchr(swk, '.')) *p= '\0';
			strcat(swk, ".bf");
			f= fopen(swk, "rb");
			if (NULL == f)
			{
				printf(" >> unable to open created object %s\n", tset-> iamatch-> dat.object.srcnameform);
				continue;
			}
		}

		/****/
		if (cto_loadb(f, tset))
		{
			fclose(f);
			continue;
		}
		
		/**/
		addv= tset-> ta.vector_c;  if (addv > maxv) maxv= addv;
		addv= tset-> ta.normal_c;  if (addv > maxv) maxv= addv;
		if (tset-> ta.surface_c > maxs) maxs= tset-> ta.surface_c;

		if (troot.fcount) { EFREE(troot.fptr); }
	}

	if (w_tilermem(maxv, 4096))
		return 1;

	return 0;
}

void	cto_dispose(void)
{
	int istep;
	tr_object *tstep;

	if (cobjects)
	{
		for (istep= cobjects, tstep= tolist; (istep --); tstep ++)
		{
			if (! tstep-> ia) continue;

			if (tstep-> ta.vector_c) { EFREE(tstep-> ta.vlist); }
			if (tstep-> ta.poly_c)
			{
				EFREE(tstep-> ta.ilist);
				if (3 == tstep-> ia) EFREE(tstep-> ta.flist);
			}
			if (tstep-> ta.surface_c)
			{
				EUFREE(tstep-> ta.sdata.ermap);
				EUFREE(tstep-> ta.sdata.ermapref);
				EFREE(tstep-> ta.sdata.ersurf);
				EFREE(tstep-> ta.sdata.erbasis);
			}
		}

		EFREE(tolist);
		tolist= NULL,
		cobjects= 0;
	}
	/*
	if (worksurface)
	{
		EFREE(worksurface);
		worksurface= NULL;
	}
	*/
}

