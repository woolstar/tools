#include "ye.h"
#include	"ymath.h"

#include <ebf.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <aart_link.h>

extern "C"
{
//	extern int			global_time;
	extern svector		*workspace;
	extern tr_camera	rendercam, *curcam;

		// dx clip
	extern void	gcam_projectpts(tr_object *opcr) ;
	extern int	gcam_clips(int n, er_face *faceptr, tr_object *opcr) ;
	extern int	gdepth_clips(int n, er_face *faceptr, tr_object *opcr) ;

		// dx mempage
	extern int	w_tilermem(long nv, long nsi) ;
	extern void	w_tilepadrel() ;

		// r math
	extern void	er_faceinfo(er_face *f, vector *) ;

		// t grid
	extern int	tgrid_build(int n, tr_set *list) ;
		// t part
	extern int	tgrid_partition(int *sections, tr_set **set) ;
		// t side
	extern void	rbuild_sides(er_side ** side_el, vector *, er_face *) ;

} ;

#if	TRENDER_THREAD
extern "C" {
	extern int	p_active ;
} ;
#	define	PTEST_ACTIVE	if (! p_active) return
#else
#	define	PTEST_ACTIVE	if (0) return
#endif

	// local

extern "C"
{
	long			cobjects = 0;
	tr_object	* tolist = NULL ;

	tro_side	*rbase_sides = NULL;
} ;

extern "C"
{
	bool	tgeom_range(void) ;
	void	tgeom_solidrange(void) ;
	void	tgeom_depth(void) ;
	int		tgeom_tile(void) ;
} ;

bool	yzor::gendata(tr_object * adest, const aart::geometry * asrc) const
{
	long	header[2] ;

	if (! asrc-> mlink || ! asrc-> mlink-> dataptr)
		return false ;

	memcpy(header, asrc-> mlink-> dataptr, sizeof(header)) ;
	if (header[0])
		tobj_mem(header[0], (char *) header[1], & (adest-> ta)) ;
	else
		tobj_conv((bf_container *) header[1], & (adest-> ta)) ;

	return setupsurf(adest) ;
}

bool	yzor::loaddata(tr_object * adest, const char * aname) const
{
	const char * p, * q ;
	char tmppath[256] ;
	int	itype, iret ;
	struct _stat	sb ;
	bf_container bdata ;

	itype= 1 ;
	p= aname ;
	if ((q= strchr(p, '.')) && ! strncmp(q +1, "bf", 2))
		itype= 0;

	q= NULL ;
	if (! _stat(aname, & sb))
	{
		q= aname ;
	}
	else
	{
		char * r ;
		strcpy(tmppath, aname) ;
		if (r= strchr(tmppath, '.')) { r ++ ;  strcpy(r, "obj"); }
			else strcat(tmppath, ".obj") ;
		if (! _stat(tmppath, & sb)) q= tmppath ;
	}

	if (NULL == q)
		return false ;

	memset(& bdata, 0, sizeof(bdata)) ;
	if (itype) iret= parse_obj(q, & bdata) ;
	else
	{
		FILE * fsrc= fopen(q, "rb") ;
		iret= load_embf(fsrc, & bdata) ;
		fclose(fsrc) ;
	}

	if (iret)
	{
		clear_embf(& bdata) ;
		return false ;
	}

	iret= tobj_conv(& bdata, & (adest-> ta)) ;
	clear_embf(& bdata) ;

	if (iret)
		return false ;

	return setupsurf(adest) ;
}

bool	yzor::loadgeom() const
{
	aart::geometry * geomrec ;
	tbagstepper<aart::geometry *> * step ;
	int	istep, imax, i, n ;
	tr_object	* fillobj ;

	if ((NULL == mnode) || (NULL == mnode-> geometryref))
		return false ;

	step= mnode-> geometryref-> getstep() ;
	istep= 0 ;
	imax= 16 ;
	while (step-> next(geomrec))
	{
		if (geomrec-> doserv) istep ++ ;
			else istep += geomrec-> src.getcount();
	}

	if (! istep)
	{
		delete step ;
		return false ;
	}

	fillobj= GM(istep, tr_object) ;
	if (NULL == fillobj)
	{
		delete step ;
		return false ;
	}

	memset(fillobj, 0, istep * sizeof(tr_object)) ;
	tolist= fillobj ;
	istep= 0;
	step-> reset() ;

	while (step-> next(geomrec))
	{
		if (geomrec-> doserv)
		{
			fillobj-> matchref= geomrec ;
			if (gendata(fillobj, geomrec))
			{
					// look for largest number of vectors needed for transform pipeline
				if (fillobj-> ta.vector_c > imax) imax= fillobj-> ta.vector_c ;
					else if (fillobj-> ta.normal_c > imax) imax= fillobj-> ta.normal_c ;

				istep ++ ;
				fillobj ++ ;
			}
		}
		else
			for (i= 0, n= geomrec-> src.getcount(); (i < n); i ++)
			{
				fillobj-> matchref= geomrec ;

				if (loaddata(fillobj, geomrec-> src[i]))
				{
						// look for largest number of vectors needed for transform pipeline
					if (fillobj-> ta.vector_c > imax) imax= fillobj-> ta.vector_c ;
						else if (fillobj-> ta.normal_c > imax) imax= fillobj-> ta.normal_c ;

					istep ++ ;
					fillobj ++ ;
				}
			}
	}

	delete step;
	cobjects= istep ;

	w_tilermem(imax, 4096) ;

	return true ;
}


bool	tgeom_ray(void)
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
		for (jstep= tstep-> ta.poly_c, faceptr= tstep-> ta.flist; (jstep --); faceptr ++)
		{
			ia= faceptr-> bits, iac= ia & 0x3ff;
			if (ia & (RACE_NORMAL | RACE_TEXTURE)) iac += iac -3;
			sict += iac;
		}
	}

	if (! sict) return 1;
	rbase_sides= sifill= GM(sict, tro_side);
		if (NULL == rbase_sides) return false ;

	for (istep= cobjects, tstep= tolist; (istep --); tstep ++)
	{
		vp= tstep-> ta.vlist -1;
		for (jstep= tstep-> ta.poly_c, faceptr= tstep-> ta.flist; (jstep --); faceptr ++)
		{
			rbuild_sides((er_side **) & sifill, vp, (er_face *) faceptr);
		}
	}

	return true;
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

	GF(makelist);
	return 0;
}


	// bring geometry into the system
bool	yzor::dogeom()
{
	int istep, jstep ;
	unsigned short
		ka, kb, 
		* eface_step ;
	vector
		* wkv ;

	tr_object * tostep ;
	aart::geometry * gref ;

	er_face *fbase, *fset;
	ev_surfbsi *bxbase, *bxcur;
	yartmath	ym ;

	if (! cobjects) return false ;

	for (istep= cobjects, tostep= tolist; (istep --); tostep ++)
	{
		gref= (aart::geometry *) tostep-> matchref ;
		if (NULL == gref)
			continue ;

			// transform points
		ym.load(gref) ;
		ym.apply(tostep-> ta.vlist, tostep-> ta.vector_c) ;
		if (tostep-> ta.normal_c && ym.invert())
			ym.applyn(tostep-> ta.nlist, tostep-> ta.normal_c) ;

			// setup surfaces
		// setupsurf(tostep) ;
		bxbase= (ev_surfbsi *) tostep-> ta.sdata.erbasis ;

			// create faces
		fbase= GM(tostep-> ta.poly_c, er_face) ;
		if (NULL == fbase)
			return false ;

		wkv= tostep-> ta.vlist -1 ;

		for (jstep= tostep-> ta.poly_c, eface_step= tostep-> ta.ilist, fset= fbase; (jstep); )
		{
			ka= *(eface_step ++) ;
			if (ka & EPB_ATTRIB)
			{
				if (EPB_SURF == (ka & 0xfc00))
				{
					kb= ka & 0x3ff ;
					bxcur= bxbase + kb ;
				}
				else
				if (EPB_BASIS == (ka & 0xfc00))
				{
					eface_step ++ ;
				}

				continue ;
			}

			kb= ka & 0x3ff ;
			fset-> bits= kb ;
			fset-> ray_count= 0;
			fset-> bound_sidea= NULL ;
			fset-> indic_sides= eface_step ;
			fset-> shading= bxcur ;

			er_faceinfo(fset, wkv) ;
			fset-> bits |= 0 ;	// auxbits from surface

			eface_step += kb ;
			if (ka & EPB_FACTN)
			{
				fset-> bits |= RACE_NORMAL ;
				eface_step += kb ;
			}
			if (ka & EPB_FACTT)
			{
				fset-> bits |= RACE_TEXTURE ;
				eface_step += (2 * kb) ;
			}

			jstep -- ;
			fset ++ ;
		}

		tostep-> ta.flist= (tro_face *) fbase ;
	}


	tgeom_ray() ;
	tgeom_grid() ;

	return true ;
}

	// legacy

bool	tgeom_range(void)
{
	int istep, jstep;
	double
		tg_near, tg_far,
		camoffset, r;
	vector camd, *vpt;
	tr_object *tstep;

	tg_near= 1e304, tg_far= -1e304;
	if (NULL == curcam)
		return false;

	M_cpy(camd, curcam-> cview)  M_neg(camd)
	camoffset= curcam-> basev.z;

	for (istep= cobjects, tstep= tolist; (istep --); tstep ++)
	{
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
		return false;
	}
	if (tg_near < 1e-5) tg_near= 1e-5;

	r= tg_far - tg_near;
	if (r < 1e-10) return false;
	curcam-> offset_z= tg_near,
	curcam-> scale_z= 267175328.0 / r;
	return true ;
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
		if (tstep-> attr & 0x10) continue;

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
		if (tstep-> attr & 0x10) continue;
		gcam_projectpts(tstep);
		gdepth_clips(tstep-> ta.poly_c, (er_face *) tstep-> ta.flist, tstep);
	}
}

int	tgeom_tile(void)
{
	int istep;
	tr_object *tstep;

	for (istep= cobjects, tstep= tolist; (istep --); tstep ++)
	{
		PTEST_ACTIVE -1;
		gcam_projectpts(tstep);

		PTEST_ACTIVE -1;
		gcam_clips(tstep-> ta.poly_c, (er_face *) tstep-> ta.flist, tstep);
	}

	return 0;
}

