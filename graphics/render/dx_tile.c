#include "rrun.h"
#include "rqdata.h"

#define	PADD		0x1000
#define PEND		1

#define	EGforward	1
#define EGback		3
#define	EGleft		4
#define EGright		8
#define EGflat		0x10
#define egProp		0x13

		ray_element pseudo_ray;

extern	ray_element	*current;

	/** t_camera.c */
extern tr_camera	*curcam;

	/** dx_mempage.c */
extern dx_point	*workpoint;
extern dx_edge	*workedge, *workefar;

extern void		c_color(void);


void	dx_tileinit(void)
{
	bzero(& pseudo_ray, sizeof(ray_element));
}

#if 0
	Edge record
		plead, pfollow - lowest and highest point markers

		ystart, yfall - starting and completing rows
		kv - current proportion along line
		vstep - edge fraction per scanline (1/y)
		iz0, diz, bz, ivz - inverted-z start, difference, non-i current z, current z
		ir0, dir, br, ivr - inverted-percent start, inverted r delta, invert offset, current r.

		kp, dp - screen point current, scaled step
		v0, kv, dv - world point start, current, step
		n0, kn, dn - normal
		t0, kt, dt - texture
		c0, kc, dc - color

#endif

static 	dx_point *dbase, *dbound;

static int	gtile_setup(dx_edge *eptr, int polyattr, double ycurrent)
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
	M_sub(eptr-> dv, dplead-> v, dpfollow-> v)
	if (polyattr & RACE_NORMAL) { M_sub(eptr-> dn, dplead-> n, dpfollow-> n) }
	if (polyattr & RACE_TEXTURE) { M_sub(eptr-> dt, dplead-> t, dpfollow-> t) }
	dzeta= eptr-> dp.z;

	dk= (ycurrent - eptr-> ystart) * eptr-> vstep;
	eptr-> vk= dk;
	M_add(eptr-> kp, dpfollow-> p, dk * eptr-> dp)
	M_scale(eptr-> dp, eptr-> vstep)

	if ((-1e-6 < dzeta) && (dzeta < 1e-6))
	{
		eptr-> lwindbits |= EGflat;
		eptr-> iz0= 0.0,
		eptr-> ivz= 1.0 / dpfollow-> p.z,
		eptr-> ir0= 0.0,
		eptr-> dir= 1;
		eptr-> ivr= 0.0;

			/** constant z */
		eptr-> bz= dpfollow-> p.z;

		dtmp= eptr-> vstep;
		M_add(eptr-> kv, dpfollow-> v, dk * eptr-> dv)
		M_scale(eptr-> dv, dtmp)
		if (polyattr & RACE_NORMAL)
		{
			M_add(eptr-> kn, dpfollow-> n, dk * eptr-> dn)
			M_scale(eptr-> dn, dtmp)
			}
		if (polyattr & RACE_TEXTURE)
		{
			M_add(eptr-> kt, dpfollow-> t, dk * eptr-> dt)
			M_scale(eptr-> dt, dtmp)
			}

		return 0;
		}

	dvid= 1.0 / dpfollow-> p.z, dvod= 1.0 / dplead-> p.z;
	eptr-> iz0= dvid,
	eptr-> diz=dvod -dvid;
	eptr-> ir0= dzeta * dvid,
	eptr-> br= dpfollow-> p.z / dzeta;
	eptr-> dir= dzeta * (dvod -dvid);

	M_cpy(eptr-> v0, dpfollow-> v)
	if (polyattr & RACE_NORMAL) { eptr-> n0= dpfollow-> n; }
	if (polyattr & RACE_TEXTURE) { eptr-> t0= dpfollow-> t; }

	eptr-> ivz= dvid= dk * eptr-> diz + eptr-> iz0;
	if ((dvid < -1e-8) || (1e-8 < dvid))
		eptr-> bz= 1.0 / dvid; else eptr-> bz= 0.0;
	eptr-> ivr= dvod= (eptr-> ir0) ? (eptr-> ir0 + dk * eptr-> dir) : dk;

	if ((-1e-10 < dvod) && (dvod < 1e-10))
	{
		eptr-> kv= eptr-> v0;
		if (polyattr & RACE_NORMAL) eptr-> kn= eptr-> n0;
		if (polyattr & RACE_TEXTURE) eptr-> kt= eptr-> t0;
		return 0;
	}

	dtmp= 1.0 / dvod - eptr-> br;
	M_add(eptr-> kv, eptr-> v0, dtmp * eptr-> dv)
	if (polyattr & RACE_NORMAL)
		{ M_add(eptr-> kn, eptr-> n0, dtmp * eptr-> dn) }
	if (polyattr & RACE_TEXTURE)
		{ M_add(eptr-> kt, eptr-> t0, dtmp * eptr-> dt) }
	return 0;
	}

static int	gtile_step(dx_edge *eptr, int polybit, double yc)
{
	double dtmp, da;
	dx_point *dplead, *dpfollow;

	dplead= eptr-> plead, dpfollow= eptr-> pfollow;
	dtmp= dplead-> p.y;
	if (dtmp < yc)
		{ return gtile_setup(eptr, polybit, yc); }

	dtmp= eptr-> vstep;
	if (eptr-> lwindbits & EGflat)
	{
		M_plus(eptr-> kp, eptr-> dp)
		M_plus(eptr-> kv, eptr-> dv)
		if (polybit & RACE_NORMAL) { M_plus(eptr-> kn, eptr-> dn) }
		if (polybit & RACE_TEXTURE) { M_plus(eptr-> kt, eptr-> dt) }
		return 0;
	}
	
	eptr-> ivz += dtmp * eptr-> diz,
	eptr-> ivr += dtmp * eptr-> dir;
	M_plus(eptr-> kp, eptr-> dp)

	dtmp= eptr-> ivz;
	if ((-1e-10 > dtmp) || (dtmp > 1e-10)) { eptr-> bz= 1.0 / dtmp; }
	dtmp= eptr-> ivr;
	if ((-1e-10 > dtmp) || (dtmp > 1e-10)) { da= (1.0 / dtmp) -eptr-> br; } else da= 0.0;
	if (da > 0.999999)
	{
		M_cpy(eptr-> kv, dplead-> v)
		if (polybit & RACE_NORMAL) { eptr-> kn= dplead-> n; }
		if (polybit & RACE_TEXTURE) { eptr-> kt= dplead-> t; }
		return 0;
	}

	M_add(eptr-> kv, eptr-> v0, da * eptr-> dv)
	if (polybit & RACE_NORMAL)
		{ M_add(eptr-> kn, eptr-> n0, da * eptr-> dn) }
	if (polybit & RACE_TEXTURE)
		{ M_add(eptr-> kt, eptr-> t0, da * eptr-> dt) }

	return 0;
}

void	gtile_poly(int nsides, er_face *facerec)
{
	int
		istep, ibits,
		polybits, polywind,
		polyshade, polyshct,
		irowy, ivconv,
		viewmin, viewmax, viewbot;
	long l,
		lrowwidth, lpixpage,
		tiledg_attrib;
	unsigned long tilei_zb, tilei_zo;
	double
		ix, ifx, iy, iy0,lxa, lxb, ilowy,
		dtmp, dvid, dvod, dlef,
		tiledg_lscale,
		tilezb_scale, tilezb_offset,
		tileag_iz, tiledg_idz,
		tileag_ir, tiledg_idr, tiledg_rb;
	byte *bitrow, *bitpix, *bitpage;
	ul *zrow, *zpix;
	ul *refrow, * refpix ;
	svector *nmrow, *nmpix ;
	dx_point
		*dstep, *dlow,
		*dfol, *dfor, *dpeek, *dpook, *dtest;
	dx_edge
		*ebase, *ebound,
		*estep, *epai, *ea, *eb;
	vector
		tiledg_dvv;
	svector
		tilestatic_n, tiledg_dvn, tiledg_nh,
		tiledg_dvt, tiledg_t3h;

	dbase= workpoint, dbound= dbase +nsides;
	ilowy= dbase-> p.y, dlow= dbase;

	polyshade= curcam-> shadrate;
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

	pseudo_ray.target= facerec,
	pseudo_ray.sarget= NULL;
	pseudo_ray.space_p= curcam-> cpos;
	pseudo_ray.result_a= 1.0;
	pseudo_ray.result_b= 0.25,
	pseudo_ray.result_c= 0.75,
	pseudo_ray.result_d= 1.0;

#if 0
	polybits &= ~ RACE_NORMAL;
#endif

	if (!(polybits & RACE_NORMAL))
	{
		if (! polywind) { M_cpy(tilestatic_n, - facerec-> face_normal) }
			else { M_cpy(tilestatic_n, facerec-> face_normal) }
	}

	l= (iy - curcam-> view_iyb);
	lrowwidth= workset.width,
	lpixpage= workset.page;
	bitrow= workset.image, zrow= workset.depth, refrow= workset.ref, nmrow= workset.normal ;
	ix= 0;

	if (l > 0)
	{
		l *= lrowwidth ;
		bitrow += l, zrow += l, refrow += l, nmrow += l;
	}
	else
	{
		l= 0;
		iy= curcam-> view_iyb;
	}

	do
	{
		for (estep= ebase, istep= 0; (estep!= ebound); estep ++)
		{
			if (!estep-> lwindbits) continue;
				/** remove any dead edges, and advance crossing edges */
			
			if (gtile_step(estep, polybits, iy))
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

				if (gtile_setup(ea, polybits, iy) || gtile_setup(eb, polybits, iy)) continue;
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
				if (gtile_setup(ea, polybits, iy) || gtile_setup(eb, polybits, iy)) continue;
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
			M_sub(tiledg_dvv, eb-> kv, ea-> kv)
			if (polybits & RACE_NORMAL) { M_sub(tiledg_dvn, eb-> kn, ea-> kn) }
			if (polybits & RACE_TEXTURE) { M_sub(tiledg_dvt, eb-> kt, ea-> kt) }

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

			bitpix= bitrow +((int) ix),
			zpix= zrow +((int) ix);
			refpix= refrow +((int) ix);
			nmpix= nmrow +((int) ix);

			if (tiledg_attrib & EGflat)
			{
				dvid= ea-> kp.z, dvod= eb-> kp.z;
				dtmp= (dvod -dvid);
			}
			else
			{
				dvid= ea-> bz, dvod= eb-> bz;
				dtmp= (dvod -dvid);
			}

			polyshct= 0;
#define	IBOU_SET	if (ivconv < 0) ivconv= 0; else if (ivconv > 255) ivconv= 255;  *bitpage= ivconv;

			if ((-1e-3 < dtmp) && (dtmp < 1e-3))
			{
				if (dlef > 1e-3)
				{
					M_add(pseudo_ray.hit_p, ea-> kv, dlef * tiledg_dvv)
					if (polybits & RACE_NORMAL) { M_add(tiledg_nh, ea-> kn, dlef * tiledg_dvn) }
					if (polybits & RACE_TEXTURE) { M_add(tiledg_t3h, ea-> kt, dlef * tiledg_dvt) }
				}
				else
				{
					M_cpy(pseudo_ray.hit_p, ea-> kv)
					if (polybits & RACE_NORMAL) { tiledg_nh= ea-> kn; }
					if (polybits & RACE_TEXTURE) { tiledg_t3h= ea-> kt; }
				}

				M_scale(tiledg_dvv, tiledg_lscale)
				if (polybits & RACE_NORMAL)
				{
					M_scale(tiledg_dvn, tiledg_lscale)
					if (! polywind)
					{
						M_neg(tiledg_nh)
						M_neg(tiledg_dvn)
					}
				}
				if (polybits & RACE_TEXTURE) { M_scale(tiledg_dvt, tiledg_lscale) }

				tilei_zb= tilezb_scale * (dvid - tilezb_offset);
				if (!(tiledg_attrib & (EGback -EGforward))) tilei_zb += 6;

				for (istep= ifx -ix; (istep --); ix += 1.0, bitpix ++, zpix ++, refpix ++, nmpix ++)
				{
					tilei_zo= *zpix;
					if (polyshct) {polyshct --; }

					if (tilei_zb < tilei_zo)
					{
						if (!polyshct)
						{
								/*** color */
							M_sub(pseudo_ray.space_d, pseudo_ray.hit_p, pseudo_ray.space_p)
							M_selfnorm(pseudo_ray.space_d)
							if (polybits & RACE_NORMAL)
							{
								dtmp= M_selfdot(tiledg_nh);
								if (((dtmp < 0.97) && (dtmp > 1e-10)) || (dtmp > 1.04))
								{
									dtmp= 1.0 / sqrt(dtmp);
									M_cpy(pseudo_ray.hit_n, dtmp * tiledg_nh)
								}
								else { M_cpy(pseudo_ray.hit_n, tiledg_nh) }
							}
								else M_cpy(pseudo_ray.hit_n, tilestatic_n) ;

							if (polybits & RACE_TEXTURE)
								{ pseudo_ray.paint_u= tiledg_t3h.x, pseudo_ray.paint_v= tiledg_t3h.y; } 

							current= &pseudo_ray;
							c_color();
							polyshct= polyshade;
						}

							/*** draw */
						*zpix= tilei_zb;

						bitpage= bitpix;
						ivconv= 255 * pseudo_ray.result_b;  IBOU_SET	bitpage += lpixpage;
						ivconv= 255 * pseudo_ray.result_c;  IBOU_SET	bitpage += lpixpage;
						ivconv= 255 * pseudo_ray.result_d;  IBOU_SET	bitpage += lpixpage;
						ivconv= 255 * pseudo_ray.result_a;  IBOU_SET

						* refpix= (unsigned long) facerec ;
						M_cpy((* nmpix), pseudo_ray.hit_n) ;
					}

					M_plus(pseudo_ray.hit_p, tiledg_dvv)
					if (polybits & RACE_NORMAL) { M_plus(tiledg_nh, tiledg_dvn) }
					if (polybits & RACE_TEXTURE) { M_plus(tiledg_t3h, tiledg_dvt) }
				}

				continue;
			}

			tileag_iz= ea-> ivz;
			tiledg_idz= tiledg_lscale * (eb-> ivz - tileag_iz);

			tileag_ir= dtmp * tileag_iz;
			tiledg_idr= tiledg_lscale * dtmp * (eb-> ivz - tileag_iz),
			tiledg_rb= dvid / dtmp;

			if ((dlef > 1e-3) && (tiledg_lscale > 1e-10))
			{
				dtmp= dlef / tiledg_lscale;
				tileag_iz+= dtmp * tiledg_idz,
				tileag_ir += dtmp * tiledg_idr;
			}

			for (istep= ifx -ix; (istep --);
				bitpix ++, zpix ++, refpix ++, nmpix ++, tileag_iz += tiledg_idz, tileag_ir += tiledg_idr)
			{
				dtmp= 1.0 / tileag_iz;
				tilei_zb= tilezb_scale * (dtmp - tilezb_offset),
				tilei_zo= *zpix;
				if (polyshct) {polyshct --; }
				
				if (tilei_zo < tilei_zb) continue;

#if 0
	/** slight-test */
				if (352 == iy)
				{
					if (282 == (ifx -istep))
						printf(" *target*\n");
				}
					else continue;
#endif

				if (!polyshct)
				{
					dtmp= ((tileag_ir < -1e-10) || (1e-10 < tileag_ir))
						? ((1.0 / tileag_ir) - tiledg_rb) : 0.0;
				
					if (dtmp < 1e-4)
					{
						M_cpy(pseudo_ray.hit_p, ea-> kv)
						if (polybits & RACE_NORMAL) { tiledg_nh= ea-> kn; }
						if (polybits & RACE_TEXTURE)
						{
							tiledg_t3h= ea-> kt;
							pseudo_ray.paint_u= tiledg_t3h.x, pseudo_ray.paint_v= tiledg_t3h.y;
						}
					}
					else
					if (dtmp > 0.9999)
					{
						M_cpy(pseudo_ray.hit_p, eb-> kv)
						if (polybits & RACE_NORMAL) { tiledg_nh= eb-> kn; }
						if (polybits & RACE_TEXTURE)
						{
							tiledg_t3h= eb-> kt;
							pseudo_ray.paint_u= tiledg_t3h.x, pseudo_ray.paint_v= tiledg_t3h.y;
						}
					}
					else
					{
						M_add(pseudo_ray.hit_p, ea-> kv, dtmp * tiledg_dvv)
						if (polybits & RACE_NORMAL)
							{ M_add(tiledg_nh, ea-> kn, dtmp * tiledg_dvn) }
						if (polybits & RACE_TEXTURE)
						{
							M_add(tiledg_t3h, ea-> kt, dtmp * tiledg_dvt)
							pseudo_ray.paint_u= tiledg_t3h.x, pseudo_ray.paint_v= tiledg_t3h.y;
						}
					}

						/*** normal correction */
					if (polybits & RACE_NORMAL)
					{
						if (! polywind) { M_neg(tiledg_nh) }
						dtmp= M_selfdot(tiledg_nh);
						if (((dtmp < 0.97) && (dtmp > 1e-10)) || (dtmp > 1.04))
						{
							dtmp= 1.0 / sqrt(dtmp);
							M_cpy(pseudo_ray.hit_n, dtmp * tiledg_nh)
						}
							else { M_cpy(pseudo_ray.hit_n, tiledg_nh) }
					}
						else M_cpy(pseudo_ray.hit_n, tilestatic_n) ;

						/*** color */
					M_sub(pseudo_ray.space_d, pseudo_ray.hit_p, pseudo_ray.space_p)
					M_selfnorm(pseudo_ray.space_d)
					current= &pseudo_ray;

					c_color();

					polyshct= polyshade;
				}

					/*** draw */
				*zpix= tilei_zb;
				bitpage= bitpix;
				ivconv= 255 * pseudo_ray.result_b;  IBOU_SET	bitpage += lpixpage;
				ivconv= 255 * pseudo_ray.result_c;  IBOU_SET	bitpage += lpixpage;
				ivconv= 255 * pseudo_ray.result_d;  IBOU_SET	bitpage += lpixpage;
				ivconv= 255 * pseudo_ray.result_a;  IBOU_SET

				* refpix= (unsigned long) facerec ;
				M_cpy((* nmpix), pseudo_ray.hit_n) ;
			}
		}

		bitrow += lrowwidth,
		zrow += lrowwidth;
		refrow += lrowwidth ;
		nmrow += lrowwidth ;
		iy += 1.0;

	} while (ebase != ebound);
}
