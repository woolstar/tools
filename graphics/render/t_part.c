/* tpartition.c - Written by Glenn M. Lewis - 8/21/94 for Derek Woolverton
	* This routine analyzes a list of input objects, and creates a grouping
	* of grids that improves the raytracer's efficiency significantly.
	*/

#include "rrun.h"
#include "tpartition.h"

#define REJECT_SIZE_FACTOR	(60.0)	/* See below... */
#define REJECT_DENSITY_FACTOR (18.0)
#define REJECT_DIST_FACTOR	(5.0)	/* See below... */

extern long			cobjects;
extern tr_object	*tolist;

/** Needed typedefs...  */

static tb_info	*info= 0;
static long	max_vert= 0;

static void	tg_add(tb_info *current, tb_info *base)
{
	int	istep, gcount;
	double di, dj, dni, dnj, dk;
	vector bi, bj;
	tb_info *iptr;

	if (! current-> vcount) return ;
	dj= dnj= current-> size.x * current-> size.y * current-> size.z;
	dnj /= current-> vcount;
	bi= current-> link-> ibound,
	bj= current-> link-> jbound;

	for (gcount= 1; (gcount); )
	{
		for (istep= cobjects, iptr= base, gcount= 0; (istep --); iptr ++)
		{
			if (iptr-> gridnum || !iptr-> vcount) continue;
			di= dni= iptr-> size.x * iptr-> size.y * iptr-> size.z;
			dni /= iptr-> vcount;
			if ((di > REJECT_SIZE_FACTOR * dj) || (dj > REJECT_SIZE_FACTOR * di) ||
				(dni > (REJECT_DENSITY_FACTOR * dnj)) || (dnj > (REJECT_DENSITY_FACTOR * dni)) ||
				(di < 1e-20)) continue;

			dk= 0.0;
			if (iptr-> link-> ibound.x < bj.x) { dk += bj.x - iptr-> link-> ibound.x; }
				else if (iptr-> link-> jbound.x > bi.x) { dk += iptr-> link-> jbound.x - bi.x; }
			if (iptr-> link-> ibound.y < bj.y) { dk += bj.y - iptr-> link-> ibound.y; }
				else if (iptr-> link-> jbound.y > bi.y) { dk += iptr-> link-> jbound.y - bi.y; }
			if (iptr-> link-> ibound.z < bj.z) { dk += bj.z - iptr-> link-> ibound.z; }
				else if (iptr-> link-> jbound.z > bi.z) { dk += iptr-> link-> jbound.z - bi.z; }
			dk /= di;
			if (dk > REJECT_DIST_FACTOR) continue;

			gcount ++,
			iptr-> gridnum= current-> gridnum;
		}
	}
}

static void	tg_bound(tb_info *list)
{
	int istep, jstep;
	double dtmp;
	tr_object *tstep;
	vector *vstep, vl, vh;

	for (istep= cobjects, tstep= tolist; (istep --); tstep ++, list ++)
	{
		if (! (jstep= tstep-> ta.vector_c))
		{
			list-> gridnum= 0,
			list-> vcount= 0,
			M_zero(list-> size)  M_zero(list-> center)
			list-> link= NULL;
			continue;
		}
		vstep= tstep-> ta.vlist;
		vl= vh= *(vstep ++), jstep --;
		for (; (jstep --); vstep ++)
		{
			dtmp= vstep-> x;  if (dtmp < vl.x) vl.x= dtmp; else if (dtmp > vh.x) vh.x= dtmp;
			dtmp= vstep-> y;  if (dtmp < vl.y) vl.y= dtmp; else if (dtmp > vh.y) vh.y= dtmp;
			dtmp= vstep-> z;  if (dtmp < vl.z) vl.z= dtmp; else if (dtmp > vh.z) vh.z= dtmp;
		}
		tstep-> ibound= vl,
		tstep-> jbound= vh;

		list-> vcount= tstep-> ta.vector_c;
		M_add(list-> center, vl, vh)  M_scale(list-> center, 0.5)
		M_sub(list-> size, vh, vl)
		list-> gridnum= 0;
		list-> link= tstep;
	}
}

#define	Mb_l(AA, BB) \
	if (BB.x < AA.x) AA.x= BB.x; if (BB.y < AA.y) AA.y= BB.y; if (BB.z < AA.z) AA.z= BB.z;
#define	Mb_h(AA, BB) \
	if (BB.x > AA.x) AA.x= BB.x; if (BB.y > AA.y) AA.y= BB.y; if (BB.z > AA.z) AA.z= BB.z;

int	tgrid_partition(int *sections, tr_set **set)
{
	int istep, jstep, gstep;
	tr_set *setlist;
	tr_object **tptrlist;
	tb_info *base, *iptr, *imax;

	*sections= 0,
	*set= NULL;

	if (! cobjects) return 1;
	base= EMEM(cobjects, tb_info);
		if (NULL == base) RE

		/** bound objects */
	tg_bound(base);
		/** get largest object */
	for (istep= cobjects, iptr= base, imax= base; (istep --); iptr ++)
	{
		iptr-> gridnum= 0;
		if (iptr-> vcount > imax-> vcount) imax= iptr;
	}

		/** start with one group */
	gstep= 0;
	imax-> gridnum= ++gstep;

		/** match objects with existing groups */
	tg_add(imax, base);

	for (istep= cobjects, iptr= base; (istep); istep --, iptr ++)
	{
		if (iptr-> gridnum) continue;
		iptr-> gridnum= ++gstep;
		tg_add(iptr, base);
	}

	setlist= EMEM(gstep, tr_set);
	tptrlist= EMEM(cobjects, tr_object *);
		if ((NULL == setlist) || (NULL == tptrlist)) RE
	*set= setlist;
	for (istep= gstep; (istep); istep --, setlist ++)
	{
		setlist-> count= 0,
		setlist-> list= tptrlist;
		for (jstep= cobjects, iptr= base; (jstep --); iptr ++)
			if ((iptr-> gridnum == istep) && iptr-> link)
			{
				setlist-> count ++,
				*(tptrlist ++)= iptr-> link;
				if (1 == setlist-> count)
				{
					M_cpy(setlist-> ibound, iptr-> link-> ibound)
					M_cpy(setlist-> jbound, iptr-> link-> jbound)
				}
				else
				{
					Mb_l(setlist-> ibound, iptr-> link-> ibound)
					Mb_h(setlist-> jbound, iptr-> link-> jbound)
				}
			}

		if (! setlist-> count) setlist-> list= NULL;
	}

	*sections= gstep;
	EFREE(base);
	return 0;
}

