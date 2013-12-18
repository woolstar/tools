#include "trun.h"
#include "escene.h"

extern int			global_time;
extern tlist_scene		tfocus;
extern long			cobjects;
extern svector		*workspace;
extern tr_object	*tolist;
extern tr_camera	rendercam, *curcam;

	/** l_thread.c */
#if	TRENDER_THREAD
	extern int	p_active ;
#	define	PTEST_ACTIVE	if (! p_active) return
#else
#	define	PTEST_ACTIVE	if (0) return
#endif

ev_ffile	*rbase_ffile = NULL;
tro_side	*rbase_sides = NULL;

int	tgeom_load(void)
{
	byte *trog_step;
	unsigned short
		i, j, k, k2, k3,
		kauxattr,
		ustep, vstep,
		iob_morph,
		iob_flip,
		*of_attrib, *of_check,
		*eface_step, *eface_substep;
	int
		cts_binary,
		cts_parses;
	long
		countb, checkl,
		lsurfattr;
	lung
		step_faces,
		of_header[4],
		*of_indx,
		*of_facestat,
		*of_step;
	float
		**trag_step,
		*trag_lookup;
	double
		dtmp,
		dweight, z1, z2;
	svector
		polyn, polyv,
		*wkn, *wkni,
		*nmorph_step;
	vector
		build_norm, visibtest,
		*wkv, *wkvi, 
		*vmorph_hold,
		*vmorph_step;

	er_matrix depthm;
	er_face *fbase, *fset;
	ev_surfbsi *bxbase, *bxcur;

	tr_object
		*tsearch, *tsorch;

	/***
	XXXX
	**/

	iob_morph= 0;
	kauxattr= 0;

	k= tfocus.set.count;
	if (!k) return 0;

	for (i= cobjects, tsearch= tolist; (i--); tsearch ++)
	{
		bxbase= tsearch-> ta.sdata.erbasis, bxcur= bxbase;
		wkv= tsearch-> ta.vlist;

		tm_createm(tsearch-> iamatch, & (tfocus.tracks));
		ere_trans(tsearch-> ta.vector_c, wkv);
		if (tsearch-> ta.normal_c)
		{
			if (em_invert())
			{
				printf(" >> Error completing math inversion");
				continue;
			}
			ere_trannorm(tsearch-> ta.normal_c, tsearch-> ta.nlist);
		}
		tsearch-> ia= 3;

		k2= 0;
		fbase= EMEM(tsearch-> ta.poly_c, er_face);
		if (NULL == fbase) { RE }
		wkv --;

		for (j= tsearch-> ta.poly_c, eface_step= tsearch-> ta.ilist, fset= fbase; (j); )
		{
			k= *(eface_step ++);
			if (k & EPB_ATTRIB)
			{
				if (EPB_SURF == (k & 0xfc00))
				{
					k2= k & 0x3ff;
					bxcur= bxbase +k2;
					kauxattr= 0;
					lsurfattr= (bxcur-> surfp) ? bxcur-> surfp-> attrib : 0;
					if (lsurfattr & ERS_INVIS) kauxattr |= RACE_INVIS;
						else if (lsurfattr & ER_TRANSMT) kauxattr |= RACE_LENS;
				}
				else
				if (EPB_BASIS == (k & 0xfc00))
				{
					eface_step ++;
				}
			continue;
			}

			k3= k & 0x3ff;
			fset-> bits= k3,
			fset-> ray_count= 0,
			fset-> bound_sidea= NULL,
			fset-> indic_sides= eface_step,
			fset-> shading= bxcur;
			er_faceinfo(fset, wkv);
			fset-> bits |= kauxattr;

			eface_step += k3;
			if (k & EPB_FACTN)
			{
				fset-> bits |= RACE_NORMAL;
				eface_step += k3;
			}
			if (k & EPB_FACTT)
			{
				fset-> bits |= RACE_TEXTURE;
				eface_step += 2 * k3;
			}
			j --, fset ++;
		}

		tsearch-> ta.flist= (tro_face *) fbase;
	}

	return 0;
}

void	tgeom_range(void)
{
	int istep, jstep;
	double
		tg_near, tg_far,
		camoffset, r;
	vector camd, *vpt;
	tr_object *tstep;

	tg_near= 1e304, tg_far= -1e304;
	if (NULL == curcam) return;

	M_cpy(camd, curcam-> cview)  M_neg(camd)
	camoffset= curcam-> basev.z;

	for (istep= cobjects, tstep= tolist; (istep --); tstep ++)
	{
		if (tstep-> ia != 3) continue ;
		for (jstep= tstep-> ta.vector_c, vpt= tstep-> ta.vlist; (jstep --); vpt ++)
		{
			r= vpt-> x * camd.x + vpt-> y * camd.y + vpt-> z * camd.z + camoffset;
			if (r < tg_near) tg_near= r;
			if (r > tg_far) tg_far= r;
		}
	}

	if (tg_far < 1e-5)
	{
		curcam-> offset_z= 0.0,
		curcam-> scale_z= 0.0;
		return;
	}
	if (tg_near < 1e-5) tg_near= 1e-5;

	r= tg_far - tg_near;
	if (r < 1e-10) return;
	curcam-> offset_z= tg_near,
	curcam-> scale_z= 267175328.0 / r;
}

void	tgeom_solidrange(void)
{
	int istep, jstep;
	double
		tg_near, tg_far,
		camoffset, r;
	vector camd, *vpt;
	tr_object *tstep;

	tg_near= 1e304, tg_far= -1e304;
	if (NULL == curcam) return;

	M_cpy(camd, curcam-> cview)  M_neg(camd)
	camoffset= curcam-> basev.z;

	for (istep= cobjects, tstep= tolist; (istep --); tstep ++)
	{
			/* only do solid objects */
		if ((tstep-> ia != 3) || (tstep-> attr & 0x10)) continue;

		for (jstep= tstep-> ta.vector_c, vpt= tstep-> ta.vlist; (jstep --); vpt ++)
		{
			r= vpt-> x * camd.x + vpt-> y * camd.y + vpt-> z * camd.z + camoffset;
			if (r < tg_near) tg_near= r;
			if (r > tg_far) tg_far= r;
		}
	}

	if (tg_far < 1e-5)
	{
		curcam-> offset_z= 0.0,
		curcam-> scale_z= 0.0;
		return;
	}

	if (tg_near < 1e-5) tg_near= 1e-5;
	r= tg_far - tg_near;
	if (r < 1e-10) return;

	curcam-> offset_z= tg_near,
	curcam-> scale_z= 267175328.0 / r;
}

void	tgeom_depth(void)
{
	int istep;
	tr_object *tstep;

	if (NULL == curcam) return;
	for (istep= cobjects, tstep= tolist; (istep --); tstep ++)
	{
			/* only do solid objects */
		if ((tstep-> ia != 3) || (tstep-> attr & 0x10)) continue;
		gcam_projectpts(tstep);
		gdepth_clips(tstep-> ta.poly_c, tstep-> ta.flist, tstep);
	}
}

int	tgeom_tile(void)
{
	int istep;
	tr_object *tstep;

	for (istep= cobjects, tstep= tolist; (istep --); tstep ++)
	{
		if (tstep-> ia != 3) continue;

		PTEST_ACTIVE -1;
		gcam_projectpts(tstep);

		PTEST_ACTIVE -1;
		gcam_clips(tstep-> ta.poly_c, tstep-> ta.flist, tstep);
	}

	return 0;
}

int	tgeom_skel(void * ref, void (* drawline)(void *, int, int, int, int))
{
	int istep;
	tr_object *tstep;

	for (istep= cobjects, tstep= tolist; (istep --); tstep ++)
	{
		if (tstep-> ia != 3) continue;
		gcam_projectpts(tstep);
		gcam_lineclip(tstep-> ta.poly_c, tstep-> ta.flist, tstep, ref, drawline);
	}

	return 0;
}

int	tgeom_ray(void)
{
	int
		istep, jstep,
		ia, iac;
	long sict;
	vector *vp;
	tro_side *sifill;
	tro_face *faceptr;
	tr_object *tstep;

	sict= 0;
	for (istep= cobjects, tstep= tolist; (istep --); tstep ++)
	{
		if (tstep-> ia != 3) continue;
		for (jstep= tstep-> ta.poly_c, faceptr= tstep-> ta.flist; (jstep --); faceptr ++)
		{
			ia= faceptr-> bits, iac= ia & 0x3ff;
			if (ia & (RACE_NORMAL | RACE_TEXTURE)) iac += iac -3;
			sict += iac;
		}
	}

	if (! sict) return 1;
	rbase_sides= sifill= EMEM(sict, tro_side);
		if (NULL == rbase_sides) RE

	for (istep= cobjects, tstep= tolist; (istep --); tstep ++)
	{
		if (tstep-> ia != 3) continue;
		vp= tstep-> ta.vlist -1;
		for (jstep= tstep-> ta.poly_c, faceptr= tstep-> ta.flist; (jstep --); faceptr ++)
		{
			rbuild_sides(& sifill, vp, faceptr);
		}
	}

	return 0;
}

int	tgeom_grid(void)
{
	int setcount;
	tr_set *makelist;

	if (tgrid_partition(&setcount, &makelist))
		{ printf(" >> partitioning failed.\n"); }
	/* tpart_display(setcount, makelist); */

	if (tgrid_build(setcount, makelist))
		{ printf(" >> grid creation failed\n"); }

	EFREE(makelist);
	return 0;
}

void	tgeom_dispose(void)
{
		/*/ sides */
	EUFREE(rbase_sides);
	rbase_sides= NULL;

		/*/ grids */
	tgrid_destroy();
}

void	tpart_display(int n, tr_set *list)
{
	int istep, jstep, id;
	tr_object **tindex, *tptr;
	tlist_element * temp ;

	for (istep= n, id= 1; (istep --); id ++, list ++)
	{
		printf("grid %d\n", id);
		for (jstep= list-> count, tindex= list-> list; (jstep --); tindex ++)
		{
			tptr= *tindex;
			temp= (tlist_element *) tptr-> matchref ;
			printf("\tob %s\n", temp-> name);
		}
	}
}
