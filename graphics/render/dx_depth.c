#include "rrun.h"

#define	PADD		0x1000
#define PEND		1

#define	EGforward	1
#define EGback		3
#define	EGleft		4
#define EGright		8
#define EGflat		0x10
#define egProp		0x13

	/** t_camera.c */
extern tr_camera	*curcam;

	/** dx_mempage.c */
extern dx_point	*workpoint;
extern dx_point	*workpoint;
extern dx_edge	*workedge, *workefar;

static 	dx_point *dbase, *dbound;

static int	gdepth_setup(dx_edge *eptr, int polyattr, double ycurrent)
{
	double dtmp, dzeta, dvid, dvod, dk;
	dx_point *dplead, *dpfollow;

	dplead= eptr-> plead, dpfollow= eptr-> pfollow;
	dtmp= dplead-> p.y;
	if (dtmp < ycurrent)
	{
		if (eptr-> lwindbits & (EGback -EGforward))
		{
			do
			{
				if (PEND == dplead-> ltipe) return 1;
				dpfollow= dplead; if (dplead == dbase) dplead= dbound;  dplead --;
				} while (dplead-> p.y < ycurrent);
			}
		else
		{
			do
			{
				if (PEND == dplead-> ltipe) return 1;
				dpfollow= dplead, dplead ++;  if (dplead == dbound) dplead= dbase;
				} while (dplead-> p.y < ycurrent);
			}

		eptr-> plead= dplead,
		eptr-> pfollow= dpfollow;
		}
	
	dvid= dplead-> p.y - dpfollow-> p.y;
	eptr->lwindbits &= ~EGflat;
	eptr-> ystart= dpfollow-> p.y,
	eptr-> yfall= dplead-> p.y,
	eptr-> vstep= (dvid < 1e-8) ? 0.0 : 1.0 / dvid;

	M_sub(eptr-> dp, dplead-> p, dpfollow-> p)
	dzeta= eptr-> dp.z;

	dk= (ycurrent - eptr-> ystart) * eptr-> vstep;
	eptr-> vk= dk;
	M_add(eptr-> kp, dpfollow-> p, dk * eptr-> dp)
	M_scale(eptr-> dp, eptr-> vstep)

	if ((-1e-6 < dzeta) && (dzeta < 1e-6))
	{
		eptr-> lwindbits |= EGflat;
		eptr-> iz0= 0.0,
		eptr-> ivz= 1.0 / dpfollow-> p.z;

		dtmp= eptr-> vstep;
		M_add(eptr-> kv, dpfollow-> v, dk * eptr-> dv)

		return 0;
	}
	else
	{
		dvid= 1.0 / dpfollow-> p.z, dvod= 1.0 / dplead-> p.z;
		eptr-> iz0= dvid,
		eptr-> bz= dpfollow-> p.z,
		eptr-> diz=dvod -dvid;
	}

	eptr-> ivz= dvid= dk * eptr-> diz + eptr-> iz0;
	return 0;
}

static int	gdepth_step(dx_edge *eptr, int polybit, double yc)
{
	double dtmp;
	dx_point *dplead, *dpfollow;

	dplead= eptr-> plead, dpfollow= eptr-> pfollow;
	dtmp= dplead-> p.y;
	if (dtmp < yc)
		{ return gdepth_setup(eptr, polybit, yc); }

	dtmp= eptr-> vstep;
	if (eptr-> lwindbits & EGflat)
	{
		M_plus(eptr-> kp, eptr-> dp)
		return 0;
	}

	eptr-> ivz += dtmp * eptr-> diz,
	M_plus(eptr-> kp, eptr-> dp)

	dtmp= eptr-> ivz;
	if ((-1e-10 > dtmp) || (dtmp > 1e-10))
		{ eptr-> bz= 1.0 / dtmp; }
	return 0;
}

void	gdepth_poly(int nsides, er_face *facerec)
{
	int
		istep, ibits,
		polybits, polywind,
		irowy,
		viewmin, viewmax, viewbot;
	long l,
		lwidth,
		tiledg_attrib;
	unsigned long tilei_zb, tilei_zo;
	double
		ix, ifx, iy, iy0,lxa, lxb, ilowy,
		dtmp, dvid, dvod, dlef,
		tiledg_lscale,
		tilezb_scale, tilezb_offset,
		tileag_iz, tiledg_idz;
	ul *zrow, *zpix, *zruw, *zpox;
	dx_point
		*dstep, *dlow,
		*dfol, *dfor, *dpeek, *dpook, *dtest;
	dx_edge
		*ebase, *ebound,
		*estep, *epai, *ea, *eb;

	dbase= workpoint, dbound= dbase +nsides;
	ilowy= dbase-> p.y, dlow= dbase;

	viewmin= curcam-> view_ixl,
	viewmax= curcam-> view_ixr,
	viewbot= curcam-> view_iyt -1;

		/** catagorize leading and trailing edges. */
	dtmp= 0.0;
	for (istep= nsides, dstep= dbase, dfor= dbase +1, dfol= dbound -1; (istep --); dfol= dstep, dstep= dfor, dfor ++)
	{
		iy= dstep-> p.y; 
		if (iy < ilowy) { ilowy= iy, dlow= dstep; }
		if (dfor == dbound) dfor= dbase;

		dtmp+= dstep-> p.x * (dfor-> p.y -dfol-> p.y);
		dstep-> ltipe= 0;
		if (dfor-> p.y == iy)
		{
			if (dfol-> p.y == iy) { dstep-> ltipe= PEND;  continue; }
				/** more work for horizontal sections. */
			for (dpeek= dfor; (dpeek-> p.y == iy) && (dpeek != dstep); ) { dpeek ++;  if (dpeek == dbound) dpeek= dbase; }
			if (dpeek == dstep) return;
			if ((dfol-> p.y < iy) != (dpeek-> p.y < iy)) continue;
			dstep-> ltipe= (dfol-> p.y < iy) ? PEND : PADD;
			continue;
		}
		if (dfol-> p.y == iy)
		{
			if (dfor-> p.y == iy) { dstep-> ltipe= PEND;  continue; }
				/** more work for horizontal sections. */
			for (dpeek= dfol; (dpeek-> p.y == iy) && (dpeek != dstep); ) { if (dpeek == dbase) dpeek= dbound; dpeek --; }
			if (dpeek == dstep) return;
			if ((dpeek-> p.y < iy) != (dfor-> p.y < iy)) continue;
			dstep-> ltipe= (dfor-> p.y < iy) ? PEND : PADD;
			continue;
		}
		if ((dfol-> p.y < iy) != (dfor-> p.y < iy)) continue;
		dstep-> ltipe= (dfor-> p.y < iy) ? PEND : PADD;
	}

	polywind= (dtmp > 0.0);

		/** start the scanning at first horizontal row */
	irowy= ilowy, iy= irowy;
	if (iy < ilowy) iy += 1.0;

		/** ebase= first side, ebound= active span */
	ebase= ebound= workedge;
	polybits= facerec-> bits;
	for (estep= ebase; (estep != workefar); estep ++)
		{ estep-> lwindbits= 0; }

	tilezb_scale= curcam-> scale_z,
	tilezb_offset= curcam-> offset_z;

	lwidth= curcam-> page_width;
	l= (iy - curcam-> view_iyb);
	ix= 0;
	if (l < 0)
	{
		l= 0;
		zrow= curcam-> depthbuf, zruw= curcam-> backbuf;
		iy= curcam-> view_iyb;
	}
	else { zrow= curcam-> depthbuf +(l * lwidth), zruw= curcam-> backbuf + (l * lwidth); }

	do
	{
		for (estep= ebase, istep= 0; (estep!= ebound); estep ++)
		{
			if (!estep-> lwindbits) continue;
				/** remove any dead edges, and advance crossing edges */

			if (gdepth_step(estep, polybits, iy))
			{
				estep-> lwindbits= 0;
				estep-> pair-> lwindbits= 0;
				istep ++;
			}
		}

		if (istep)
		{
			for (estep= ebound; ((estep != ebase) && (!estep-> lwindbits)); estep --) ebound --;
		}

			/*** Add New Edge */
		for (istep= nsides, dstep= dbase, estep= ebase; (istep --); dstep ++)
		{
			ibits= dstep-> ltipe;
			if (!(ibits & PADD) || (dstep-> p.y > iy)) continue;
				/** find edge vacancies */

			epai= ebound;
			while ((estep != epai) && (estep-> lwindbits)) estep ++;  ea= estep;
			if (estep == epai) { estep-> lwindbits= 0, epai ++; }
			estep ++;

			while ((estep != epai) && (estep-> lwindbits)) estep ++;  eb= estep;
			if (estep == epai) { estep-> lwindbits= 0, epai ++; }
			dstep-> ltipe &= ~PADD;

				/** find which way it revolves. */
			dfol= ((dstep != dbase) ? dstep : dbound) -1,
			dfor= dstep +1;  if (dfor == dbound) dfor= dbase;
			dpeek= dfol, dpook= dfor;
			iy0=dstep->p.y;
			while (((dpeek-> p.x == ix) || (dpeek-> p.x == dpook-> p.x) || (dpeek-> p.y <= iy0)) && !(dpeek-> ltipe & PEND))
			{
				if (dpeek == dbase) dtest= dbound -1; else dtest= dpeek -1;
				if ((dstep == dtest) || (dpook == dtest)) break;
				dpeek= dtest;
			}
			while (((dpook-> p.x == ix) || (dpeek-> p.x == dpook-> p.x) || (dpook-> p.y <= iy0)) && !(dpook-> ltipe & PEND))
			{
				dtest= dpook +1;  if (dtest == dbound) dtest= dbase;
				if ((dstep == dtest) || (dpeek == dtest)) break;
				dpook= dtest;
			}
				
			dtmp= dpeek-> p.y -iy0;
			dvid= (dtmp > 1e-10) ? 1.0 / dtmp : 1e10,
			ix= (dpeek-> p.x -dstep-> p.x) * dvid;
			dtmp= dpook-> p.y -iy0;
		     dvid= (dtmp > 1e-10) ? 1.0 / dtmp : 1e10,
			ifx= (dpook-> p.x -dstep-> p.x)  * dvid;

#define	DPc 	dpeek= dfol, dpeek-> ltipe &= ~PADD;

			if (ix < ifx)
			{
				ea-> lwindbits= EGback;
				ea-> plead= dfol, ea-> pfollow= dstep;
				if (dfol-> ltipe & PADD)
				{
					while (dfol-> ltipe & PADD)
						{ DPc  if (dfol == dbase) dfol= dbound;  dfol --; }
					ea-> plead= dfol, ea-> pfollow= dpeek;
				}

				eb-> lwindbits= EGforward;
				eb-> plead= dfor, eb-> pfollow= dstep;
				if (dfor-> ltipe & PADD)
				{
					while (dfor-> ltipe & PADD)
						{ DPc  dfor ++;  if (dfor == dbound) dfor= dbase; }
					eb-> plead= dfor, eb-> pfollow= dpeek;
				}

				if (gdepth_setup(ea, polybits, iy) || gdepth_setup(eb, polybits, iy)) continue;
			}
			else
			{
				ea-> lwindbits= EGforward;
				ea-> plead= dfor, ea-> pfollow= dstep;
				if (dfor-> ltipe & PADD)
				{
					while (dfor-> ltipe & PADD)
						{ DPc  dfor ++;  if (dfor == dbound) dfor= dbase; }
					ea-> plead= dfor, ea-> pfollow= dpeek;
				}
				eb-> lwindbits= EGback;
				eb-> plead= dfol, eb-> pfollow= dstep;
				if (dfol-> ltipe & PADD)
				{
					while (dfol-> ltipe & PADD)
						{ DPc  if (dfol == dbase) dfol= dbound;  dfol --; }
					eb-> plead= dfol, eb-> pfollow= dpeek;
				}
				if (gdepth_setup(ea, polybits, iy) || gdepth_setup(eb, polybits, iy)) continue;
			}
			ebound= epai;

				/*** find their place in the list. */
			lxa= ea-> kp.x, lxb= eb-> kp.x;
			for (estep= ebase; (estep != ebase); estep ++)
			{
				if (!(estep-> lwindbits & EGleft)) continue;
				if (lxb < estep-> kp.x) continue;
				epai= estep-> pair;
				if (lxa > epai-> kp.x) continue;
				estep-> pair= ea, ea-> pair= estep, ea-> lwindbits |= EGright;
				eb-> pair= epai, epai-> pair= eb, eb-> lwindbits |= EGleft,
				epai-> lwindbits= (epai-> lwindbits & egProp) | EGright;
				estep= NULL;
				break;
			}

			if (estep)
			{
				ea-> lwindbits |= EGleft,
				eb-> lwindbits |= EGright;
				ea-> pair= eb,
				eb-> pair= ea;
			}
		}

			/*** if y is below the screen, break */
		if (iy > viewbot) break;

		for (estep= ebase; (estep!= ebound); estep ++)
		{
			if (!(estep-> lwindbits & EGleft)) continue;
				/** strip active edges */
			
			ea= estep, eb= estep-> pair;
			ix= floor(ea-> kp.x), ifx= floor(eb-> kp.x);
			dtmp= ea-> kp.x - ix;
			/* if (dtmp > 1e-8) ix += 1.0; */
			if ((ix > viewmax) || (ifx < viewmin) || (ix >= ifx)) continue;
			
			tiledg_attrib= estep-> lwindbits;

			dtmp= eb-> kp.x - ea-> kp.x;
			tiledg_lscale= (dtmp > 1e-4) ? (1.0 / dtmp) : 1.0;

			if (ix < viewmin)
			{
				dlef= tiledg_lscale * (viewmin -ix);
				ix= viewmin;
				/** skip ahead */
			}
			else
			{
				dlef= (ix - ea-> kp.x);
				if (dlef < 0.0) dlef += 1.0;
				if (dlef < 1e-24) { dlef= 0.0; }
					else dlef *= tiledg_lscale;
			}
			if (ifx > viewmax) { ifx= viewmax; }

			zpix= zrow +((int) ix),
			zpox= zruw +((int) ix);

			if (tiledg_attrib & EGflat)
			{
				dvid= ea-> kp.z, dvod= eb-> kp.z;
				dtmp= (dvod -dvid);
			}
			else
			{
				dvid= ea-> bz, dvod= eb-> bz,
				dtmp= (dvod -dvid);
			}

			if ((-1e-3 < dtmp) && (dtmp < 1e-3))
			{
				tilei_zb= tilezb_scale * (dvid - tilezb_offset);
				if (!(tiledg_attrib & (EGback -EGforward))) tilei_zb += 6;

				for (istep= ifx -ix; (istep --); ix += 1.0, zpix ++, zpox ++)
				{
					tilei_zo= *zpix;
					if (tilei_zb < tilei_zo) { *zpix= tilei_zb, *zpox= tilei_zo; }
						else if (tilei_zb < *zpox) { *zpox= tilei_zb; }
				}

				continue;
			}

			tileag_iz= ea-> ivz;
			tiledg_idz= tiledg_lscale * (eb-> ivz - tileag_iz);

			if ((dlef > 1e-3) && (tiledg_lscale > 1e-10))
			{
				dtmp= dlef / tiledg_lscale;
				tileag_iz+= dtmp * tiledg_idz;
			}

			for (istep= ifx -ix; (istep --); zpix ++, zpox ++, tileag_iz += tiledg_idz)
			{
				dtmp= 1.0 / tileag_iz;
				tilei_zb= tilezb_scale * (dtmp - tilezb_offset),
				tilei_zo= *zpix;
				if (tilei_zo > tilei_zb) { *zpix= tilei_zb, *zpox= tilei_zo; }
					else if (tilei_zb < *zpox) { *zpox= tilei_zb; }
			}
		}

		zrow += lwidth, zruw += lwidth;
		iy += 1.0;

	} while (ebase != ebound);
}

static int	gortho_setup(dx_edge *eptr, int polyattr, double ycurrent)
{
	double dtmp, dzeta, dvid, dk;
	dx_point *dplead, *dpfollow;

	dplead= eptr-> plead, dpfollow= eptr-> pfollow;
	dtmp= dplead-> p.y;
	if (dtmp < ycurrent)
	{
		if (eptr-> lwindbits & (EGback -EGforward))
		{
			do
			{
				if (PEND == dplead-> ltipe) return 1;
				dpfollow= dplead; if (dplead == dbase) dplead= dbound;  dplead --;
			} while (dplead-> p.y < ycurrent);
		}
		else
		{
			do
			{
				if (PEND == dplead-> ltipe) return 1;
				dpfollow= dplead, dplead ++;  if (dplead == dbound) dplead= dbase;
			} while (dplead-> p.y < ycurrent);
		}

		eptr-> plead= dplead,
		eptr-> pfollow= dpfollow;
	}

	dvid= dplead-> p.y - dpfollow-> p.y;
	eptr->lwindbits &= ~EGflat;
	eptr-> ystart= dpfollow-> p.y,
	eptr-> yfall= dplead-> p.y,
	eptr-> vstep= (dvid < 1e-8) ? 0.0 : 1.0 / dvid;

	M_sub(eptr-> dp, dplead-> p, dpfollow-> p)
	dzeta= eptr-> dp.z;

	dk= (ycurrent - eptr-> ystart) * eptr-> vstep;
	eptr-> vk= dk;
	M_add(eptr-> kp, dpfollow-> p, dk * eptr-> dp)
	M_scale(eptr-> dp, eptr-> vstep)

	eptr-> lwindbits |= EGflat;
	eptr-> iz0= 0.0,
	eptr-> ivz= 1.0 / dpfollow-> p.z;

	dtmp= eptr-> vstep;
	return 0;
}

void	gdepthortho_poly(int nsides, er_face *facerec)
{
	int
		istep, ibits,
		polybits, polywind,
		irowy,
		viewmin, viewmax, viewbot;
	long l,
		lwidth,
		tiledg_attrib;
	unsigned long tilei_zb, tilei_zo;
	double
		ix, ifx, iy, iy0,lxa, lxb, ilowy,
		dtmp, dvid, dvod, dlef,
		tiledg_lscale,
		tilezb_scale, tilezb_offset,
		tileag_iz, tiledg_idz;
	ul *zrow, *zpix, *zruw, *zpox;
	dx_point
		*dstep, *dlow,
		*dfol, *dfor, *dpeek, *dpook, *dtest;
	dx_edge
		*ebase, *ebound,
		*estep, *epai, *ea, *eb;

	dbase= workpoint, dbound= dbase +nsides;
	ilowy= dbase-> p.y, dlow= dbase;

	viewmin= curcam-> view_ixl,
	viewmax= curcam-> view_ixr,
	viewbot= curcam-> view_iyt -1;

		/** catagorize leading and trailing edges. */
	dtmp= 0.0;
	for (istep= nsides, dstep= dbase, dfor= dbase +1, dfol= dbound -1; (istep --); dfol= dstep, dstep= dfor, dfor ++)
	{
		iy= dstep-> p.y; 
		if (iy < ilowy) { ilowy= iy, dlow= dstep; }
		if (dfor == dbound) dfor= dbase;

		dtmp+= dstep-> p.x * (dfor-> p.y -dfol-> p.y);
		dstep-> ltipe= 0;
		if (dfor-> p.y == iy)
		{
			if (dfol-> p.y == iy) { dstep-> ltipe= PEND;  continue; }
				/** more work for horizontal sections. */
			for (dpeek= dfor; (dpeek-> p.y == iy) && (dpeek != dstep); ) { dpeek ++;  if (dpeek == dbound) dpeek= dbase; }
			if (dpeek == dstep) return;
			if ((dfol-> p.y < iy) != (dpeek-> p.y < iy)) continue;
			dstep-> ltipe= (dfol-> p.y < iy) ? PEND : PADD;
			continue;
		}
		if (dfol-> p.y == iy)
		{
			if (dfor-> p.y == iy) { dstep-> ltipe= PEND;  continue; }
				/** more work for horizontal sections. */
			for (dpeek= dfol; (dpeek-> p.y == iy) && (dpeek != dstep); ) { if (dpeek == dbase) dpeek= dbound; dpeek --; }
			if (dpeek == dstep) return;
			if ((dpeek-> p.y < iy) != (dfor-> p.y < iy)) continue;
			dstep-> ltipe= (dfor-> p.y < iy) ? PEND : PADD;
			continue;
		}
		if ((dfol-> p.y < iy) != (dfor-> p.y < iy)) continue;
		dstep-> ltipe= (dfor-> p.y < iy) ? PEND : PADD;
	}

	polywind= (dtmp > 0.0);

		/** start the scanning at first horizontal row */
	irowy= ilowy, iy= irowy;
	if (iy < ilowy) iy += 1.0;

		/** ebase= first side, ebound= active span */
	ebase= ebound= workedge;
	polybits= facerec-> bits;
	for (estep= ebase; (estep != workefar); estep ++)
		{ estep-> lwindbits= 0; }

	tilezb_scale= curcam-> scale_z,
	tilezb_offset= curcam-> offset_z;

	lwidth= curcam-> page_width;
	l= (iy - curcam-> view_iyb);
	ix= 0;
	if (l < 0)
	{
		l= 0;
		zrow= curcam-> depthbuf, zruw= curcam-> backbuf;
		iy= curcam-> view_iyb;
	}
	else { zrow= curcam-> depthbuf +(l * lwidth), zruw= curcam-> backbuf + (l * lwidth); }

	do
	{
		for (estep= ebase, istep= 0; (estep!= ebound); estep ++)
		{
			if (!estep-> lwindbits) continue;
				/** remove any dead edges, and advance crossing edges */

			/**/
			dfor= estep-> plead, dfol= estep-> pfollow;
			dtmp= dfor-> p.y;
			if (dtmp < iy)
			{
				if (gdepth_setup(estep, polybits, iy))
				{
					estep-> lwindbits= 0;
					estep-> pair-> lwindbits= 0;
					istep ++;
				}
			}
				else { M_plus(estep-> kp, estep-> dp) }
		}

		if (istep)
		{
			for (estep= ebound; ((estep != ebase) && (!estep-> lwindbits)); estep --) ebound --;
		}

			/*** Add New Edge */
		for (istep= nsides, dstep= dbase, estep= ebase; (istep --); dstep ++)
		{
			ibits= dstep-> ltipe;
			if (!(ibits & PADD) || (dstep-> p.y > iy)) continue;
				/** find edge vacancies */

			epai= ebound;
			while ((estep != epai) && (estep-> lwindbits)) estep ++;  ea= estep;
			if (estep == epai) { estep-> lwindbits= 0, epai ++; }
			estep ++;

			while ((estep != epai) && (estep-> lwindbits)) estep ++;  eb= estep;
			if (estep == epai) { estep-> lwindbits= 0, epai ++; }
			dstep-> ltipe &= ~PADD;

				/** find which way it revolves. */
			dfol= ((dstep != dbase) ? dstep : dbound) -1,
			dfor= dstep +1;  if (dfor == dbound) dfor= dbase;
			dpeek= dfol, dpook= dfor;
			iy0=dstep->p.y;
			while (((dpeek-> p.x == ix) || (dpeek-> p.x == dpook-> p.x) || (dpeek-> p.y <= iy0)) && !(dpeek-> ltipe & PEND))
			{
				if (dpeek == dbase) dtest= dbound -1; else dtest= dpeek -1;
				if ((dstep == dtest) || (dpook == dtest)) break;
				dpeek= dtest;
			}
			while (((dpook-> p.x == ix) || (dpeek-> p.x == dpook-> p.x) || (dpook-> p.y <= iy0)) && !(dpook-> ltipe & PEND))
			{
				dtest= dpook +1;  if (dtest == dbound) dtest= dbase;
				if ((dstep == dtest) || (dpeek == dtest)) break;
				dpook= dtest;
			}
				
			dtmp= dpeek-> p.y -iy0;
			dvid= (dtmp > 1e-10) ? 1.0 / dtmp : 1e10,
			ix= (dpeek-> p.x -dstep-> p.x) * dvid;
			dtmp= dpook-> p.y -iy0;
		     dvid= (dtmp > 1e-10) ? 1.0 / dtmp : 1e10,
			ifx= (dpook-> p.x -dstep-> p.x)  * dvid;

#define	DPc 	dpeek= dfol, dpeek-> ltipe &= ~PADD;

			if (ix < ifx)
			{
				ea-> lwindbits= EGback;
				ea-> plead= dfol, ea-> pfollow= dstep;
				if (dfol-> ltipe & PADD)
				{
					while (dfol-> ltipe & PADD)
						{ DPc  if (dfol == dbase) dfol= dbound;  dfol --; }
					ea-> plead= dfol, ea-> pfollow= dpeek;
				}

				eb-> lwindbits= EGforward;
				eb-> plead= dfor, eb-> pfollow= dstep;
				if (dfor-> ltipe & PADD)
				{
					while (dfor-> ltipe & PADD)
						{ DPc  dfor ++;  if (dfor == dbound) dfor= dbase; }
					eb-> plead= dfor, eb-> pfollow= dpeek;
				}

			}
			else
			{
				ea-> lwindbits= EGforward;
				ea-> plead= dfor, ea-> pfollow= dstep;
				if (dfor-> ltipe & PADD)
				{
					while (dfor-> ltipe & PADD)
						{ DPc  dfor ++;  if (dfor == dbound) dfor= dbase; }
					ea-> plead= dfor, ea-> pfollow= dpeek;
				}
				eb-> lwindbits= EGback;
				eb-> plead= dfol, eb-> pfollow= dstep;
				if (dfol-> ltipe & PADD)
				{
					while (dfol-> ltipe & PADD)
						{ DPc  if (dfol == dbase) dfol= dbound;  dfol --; }
					eb-> plead= dfol, eb-> pfollow= dpeek;
				}
			}
			if (gortho_setup(ea, polybits, iy) || gortho_setup(eb, polybits, iy)) continue;
			ebound= epai;

				/*** find their place in the list. */
			lxa= ea-> kp.x, lxb= eb-> kp.x;
			for (estep= ebase; (estep != ebase); estep ++)
			{
				if (!(estep-> lwindbits & EGleft)) continue;
				if (lxb < estep-> kp.x) continue;
				epai= estep-> pair;
				if (lxa > epai-> kp.x) continue;
				estep-> pair= ea, ea-> pair= estep, ea-> lwindbits |= EGright;
				eb-> pair= epai, epai-> pair= eb, eb-> lwindbits |= EGleft,
				epai-> lwindbits= (epai-> lwindbits & egProp) | EGright;
				estep= NULL;
				break;
			}

			if (estep)
			{
				ea-> lwindbits |= EGleft,
				eb-> lwindbits |= EGright;
				ea-> pair= eb,
				eb-> pair= ea;
			}
		}

			/*** if y is below the screen, break */
		if (iy > viewbot) break;

		for (estep= ebase; (estep!= ebound); estep ++)
		{
			if (!(estep-> lwindbits & EGleft)) continue;
				/** strip active edges */
			
			ea= estep, eb= estep-> pair;
			ix= floor(ea-> kp.x), ifx= floor(eb-> kp.x);
			dtmp= ea-> kp.x - ix;
			/* if (dtmp > 1e-8) ix += 1.0; */
			if ((ix > viewmax) || (ifx < viewmin) || (ix >= ifx)) continue;
			
			tiledg_attrib= estep-> lwindbits;

			dtmp= eb-> kp.x - ea-> kp.x;
			tiledg_lscale= (dtmp > 1e-4) ? (1.0 / dtmp) : 1.0;

			if (ix < viewmin)
			{
				dlef= tiledg_lscale * (viewmin -ix);
				ix= viewmin;
				/** skip ahead */
			}
			else
			{
				dlef= (ix - ea-> kp.x);
				if (dlef < 0.0) dlef += 1.0;
				if (dlef < 1e-24) { dlef= 0.0; }
					else dlef *= tiledg_lscale;
			}
			if (ifx > viewmax) { ifx= viewmax; }

			zpix= zrow +((int) ix),
			zpox= zruw +((int) ix);

			dvid= ea-> kp.z, dvod= eb-> kp.z;
			dtmp= (dvod -dvid);

			if ((-1e-3 < dtmp) && (dtmp < 1e-3))
			{
				tilei_zb= tilezb_scale * (dvid - tilezb_offset);
				if (!(tiledg_attrib & (EGback -EGforward))) tilei_zb += 6;

				for (istep= ifx -ix; (istep --); ix += 1.0, zpix ++, zpox ++)
				{
					tilei_zo= *zpix;
					if (tilei_zb < tilei_zo) { *zpix= tilei_zb, *zpox= tilei_zo; }
						else if (tilei_zb < *zpox) { *zpox= tilei_zb; }
				}

				continue;
			}

			tileag_iz= ea-> kp.z;
			tiledg_idz= tiledg_lscale * (eb-> kp.z - tileag_iz);

			if ((dlef > 1e-3) && (tiledg_lscale > 1e-10))
			{
				dtmp= dlef / tiledg_lscale;
				tileag_iz+= dtmp * tiledg_idz;
			}

			for (istep= ifx -ix; (istep --); zpix ++, zpox ++, tileag_iz += tiledg_idz)
			{
				tilei_zb= tilezb_scale * (tileag_iz - tilezb_offset),
				tilei_zo= *zpix;
				if (tilei_zo > tilei_zb) { *zpix= tilei_zb, *zpox= tilei_zo; }
					else if (tilei_zb < *zpox) { *zpox= tilei_zb; }
			}
		}

		zrow += lwidth, zruw += lwidth;
		iy += 1.0;

	} while (ebase != ebound);
}

