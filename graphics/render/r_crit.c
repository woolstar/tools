#include "rrun.h"

	/***** ray */

extern shurt	ray_num, ray_rdepth, ray_gdepth;
extern int		ct_fog, ct_env0, ct_env1,
				ct_grids, ct_ball, ct_deform;
extern ray_element	*current;
extern er_grid			*magic_grid;

#if	SPHERE_SUPPORT
extern er_smot			*magic_spot;
extern er_sform		*magic_deform;
#endif

extern er_camera	magic_cam;

extern vekU	fogmath;
extern colU	fogcol;

extern void		c_color(), ci_color(), cv_shadow();
extern void		rclear_facc(er_grid *);

	/***** local */

	void	cast_ray(), glass_ray();
	int	crit_ray();

static	int	crit_grid(),
		crit_cell(), crit_edged();

static double	tmin, tmax;
static er_sform	*bmhit;

	colU		opaq_spf;

static unsigned int	vbits, opaq_space;

static	vector		vis, vrdir, vacc;
static	svector		svis, svrdir;
static	ev_light	*leshad;

#ifdef MAC
static ray_opaque	*opaq_list, *opaq_setp;
#else
static ray_opaque	opaq_list[32], *opaq_setp;

#endif

#define BACKFACK	1
#define RZMASK		0x3ff

#define FOGCALC		if (ct_fog && (dform>fogmath.x)) { dtmp= fogmath.y* (dform-fogmath.x), ddot= 1.0-dtmp; \
					current-> result_b= (dtmp * fogcol.r) + (ddot * current-> result_b), \
					current-> result_c= (dtmp * fogcol.g) + (ddot * current-> result_c), \
					current-> result_d= (dtmp * fogcol.b) + (ddot * current-> result_d); }

#ifdef MAC
int	rcrit_alloc()
{
	opaq_list= EMEM(32, ray_opaque);
	return MemError();
}
void	rcrit_dispose()
{
	EFREE(opaq_list);
}
#endif

	/**********/

static void	setup_ray(ray_element *ptr)
{
	M_cpy(vis, ptr-> space_p)  M_cpy(svis, vis)
	M_cpy(vrdir, ptr-> space_d)  M_cpy(svrdir, vrdir)
	if (0.0 == vrdir.x) { vbits+= RAY_PARAX; }
		else { vacc.x= 1.0 / vrdir.x;  if (vrdir.x < 0.0) vbits+= RAY_NEGX; }
	if (0.0 == vrdir.y) { vbits+= RAY_PARAY; }
		else { vacc.y= 1.0 / vrdir.y;  if (vrdir.y < 0.0) vbits+= RAY_NEGY; }
	if (0.0 == vrdir.z) { vbits+= RAY_PARAZ; }
		else { vacc.z= 1.0 / vrdir.z;  if (vrdir.z < 0.0) vbits+= RAY_NEGZ; }
}

int	crit_ray(ev_light *lip, double tsmin)
{ 
	unsigned int istep;
	double
		dv, dt;
	er_grid *gstep;
	er_face *pilf, *selftarg;

#if	SPHERE_SUPPORT
	unsigned short i, j, ka, kb;
	long shadhash;
	double dtmp, dform, ddot ;
	vekU
		sdfm_p, sdfm_d,
		bdir;
	ev_surfbsi *piloq;
	er_surface *pisoq;
	er_smot *bstep;
	er_sform *bdstep;
#endif

	ray_num++, vbits= 0;
	if (!ray_num) { rclear_facc(magic_grid);  ray_num++; }
	selftarg= current-> target;
		if (selftarg) selftarg-> ray_count= ray_num;

	setup_ray(current);

	tmin= tsmin, tmax= current-> dist;
	if ((pilf= lip-> cache_1) && (pilf != selftarg))
	{
		dv= M_dot(svrdir, pilf-> face_normal);
		if ((dv > 1e-12) || (dv < -1e-12))
		{
			dt= M_dot(svis, pilf-> face_normal) +pilf-> plate_d, dt/= -dv;
			if ((dt > tmin) && (dt < tmax) && !crit_edged(pilf, dt)) return 1;
			if (crit_cell(lip-> cache_2, 0)) return 1;
			leshad= lip;
		}
	} else leshad= lip;

	opaq_setp= opaq_list, opaq_space= 0, C_set(opaq_spf)
	for (istep= ct_grids, gstep= magic_grid; istep--; gstep++) 
		{ if (crit_grid(gstep)) return 1; }

	lip-> cache_1= NULL;

#if	SPHERE_SUPPORT
	if (NULL!= magic_spot)
	{
		bstep= magic_spot;
		for (istep= ct_ball; (istep--); bstep++)
		{
			if (bstep == current-> sarget) continue;
			ka= bstep-> attrib;
			if (ka & RACE_INVIS) continue;
			M_sub(bdir, bstep-> centr, vis)
			ddot= M_dot(bdir, vrdir), dtmp= M_selfdot(bdir);
			if (dtmp < bstep-> radius2)
			{
				/** /// calc distance to edge */
				if (ka & RACE_LENS)
				{
					if (opaq_space == 32) return 1;
					dtmp= bstep-> radius2 +(ddot*ddot) -dtmp, dt= ddot +sqrt(dtmp);
					opaq_setp-> kinmd= 2, opaq_setp-> tributs= 0, opaq_setp-> dist= dt;
					opaq_setp-> surfat= bstep-> shading, opaq_setp-> spherptr= bstep;
					opaq_setp++, opaq_space++;
					continue;
					}
				return 1;
				}
			dtmp= bstep-> radius2 +(ddot*ddot) -dtmp;
			if (dtmp < 0.0) continue;
			dv= sqrt(dtmp), dt= ddot - dv;
			if ((dt > tmin) && (dt < tmax))
			{
				if (ka & RACE_LENS)
				{
					if (opaq_space == 32) return 1;
					opaq_setp-> kinmd= 2, opaq_setp-> dist= dt,
					opaq_setp-> surfat= bstep-> shading, opaq_setp-> spherptr= bstep;
					dt= ddot + dv;
					if (dt < tmax)
					{
						opaq_setp-> tributs= 2,
						opaq_setp-> dost= dt;
						}
						else { opaq_setp-> tributs= 1; }
					opaq_setp++, opaq_space++;

					continue;
					}
				return 1;
				}
			}
		for (istep= ct_deform, bdstep= magic_deform; (istep--); bstep++, bdstep++)
		{
			if (bstep == current-> sarget) continue;
			ka= bstep-> attrib;
			if (ka & RACE_INVIS) continue;
			M_sub(bdir, vis, bstep-> centr)
			ddot= M_dot(bdir, vrdir), dtmp= bstep-> radius2 +(ddot*ddot) -M_selfdot(bdir);
			if (dtmp < 0.0) continue;

			sdfm_p.x= M_dot(bdstep-> imatx, bdir),
				sdfm_p.y= M_dot(bdstep-> imaty, bdir),
				sdfm_p.z= M_dot(bdstep-> imatz, bdir);
			sdfm_d.x= M_dot(bdstep-> imatx, vrdir),
				sdfm_d.y= M_dot(bdstep-> imaty, vrdir),
				sdfm_d.z= M_dot(bdstep-> imatz, vrdir);

			dtmp= M_selfdot(sdfm_d);  if (dtmp == 0.0) continue;
			dform= 1.0 /sqrt(dtmp), M_scale(sdfm_d, dform)
			ddot= -M_dot(sdfm_p, sdfm_d), dtmp= M_selfdot(sdfm_p);
			if (dtmp < 1.0)
			{
				/** /\/ calc distance to edge */
				if (ka & RACE_LENS)
				{
					if (opaq_space == 32) return 1;
					opaq_setp-> kinmd= 3, opaq_setp-> tributs= 0,
					opaq_setp-> spherptr= bstep, opaq_setp-> warptr= bdstep,
					opaq_setp-> surfat= bstep-> shading;
#if 0
					opaq_setp-> dist= x,
#endif
					opaq_setp++, opaq_space++;

					continue;
					}
				return 1;
				}
			dtmp= 1.0 +(ddot*ddot) -dtmp;
			if (dtmp < 0.0) continue;
			dv= sqrt(dtmp), dt= ddot -dv;
			if (dt > 0.0)
			{
				M_add(bdir, sdfm_p, dt * sdfm_d)
				sdfm_p.x= M_dot(bdstep-> matx, bdir),
				sdfm_p.y= M_dot(bdstep-> maty, bdir),
				sdfm_p.z= M_dot(bdstep-> matz, bdir);
				M_plus(sdfm_p, bdstep-> matw)
				M_minus(sdfm_p, vis)
				dtmp= sqrt(M_selfdot(sdfm_p));
				if (dtmp < tmax)
				{
					if (ka & RACE_LENS)
					{
						if (opaq_space == 32) return 1;
						opaq_setp-> kinmd= 3, opaq_setp-> tributs= 1,
						opaq_setp-> spherptr= bstep, opaq_setp-> warptr= bdstep,
						opaq_setp-> surfat= bstep-> shading;
						opaq_setp-> dist= dtmp;

							/*** check for second side hit, big math */
						M_add(bdir, sdfm_p, dt * sdfm_d)
						sdfm_p.x= M_dot(bdstep-> matx, bdir),
						sdfm_p.y= M_dot(bdstep-> maty, bdir),
						sdfm_p.z= M_dot(bdstep-> matz, bdir);
						M_plus(sdfm_p, bdstep-> matw)
						M_minus(sdfm_p, vis)
						dtmp= sqrt(M_selfdot(sdfm_p));
						if (dtmp < tmax)
						{
							opaq_setp-> tributs= 2,
							opaq_setp-> dost= dtmp;
							}
							else { opaq_setp-> tributs= 1; }
						opaq_setp++, opaq_space++;
						continue;
						}
					return 1;
					}
				}
			}
		}

	/***
		/// for each item
		/// check if no texture and fake transp, set and use -
		/// split up by type
		///// check dgs specific texture poing
		///// do cv_shadow() and normal gen
		/// bend and mix
		**/

	if (opaq_space)
	{
		while (opaq_setp != opaq_list)
		{
			opaq_setp--;

			j= opaq_setp-> kinmd,
			piloq= opaq_setp-> surfat;
			ka= piloq-> mapcomb,
			kb= piloq-> surfp-> attrib & ER_VAPOR;

			switch (j)
			{
				case 1:
					pilf= opaq_setp-> facptr,
					j= pilf-> bits & RACE_MASK;
					if (!ka)
					{
						if (kb)
						{
							C_mult(opaq_spf, piloq-> surfp-> col_t)
							continue;
							}
						if (!(j & RACE_NORMAL)) { M_cpy(sdfm_d, pilf-> face_normal) }
						else
						{
							dtmp= opaq_setp-> dist;
							M_add(current-> hit_p, current-> space_p, dtmp * current-> space_d)
							i= hit_edged(pilf, dtmp);
							calc_spot(pilf, (int) i);
							M_cpy(sdfm_d, current-> hit_n)
							}
						pisoq= piloq-> surfp;
						break;
						}
					dtmp= opaq_setp-> dist;
					M_add(current-> hit_p, current-> space_p, dtmp * current-> space_d)
					if (j)
					{
						i= hit_edged(pilf, dtmp);
						calc_spot(pilf, (int) i);
						}
					pisoq= piloq-> surfmix;
					cv_shadow(piloq, pisoq, (j & RACE_TEXTURE), &shadhash);
					if (kb) { C_mult(opaq_spf, pisoq-> col_t)  continue; }
					if (!(j & RACE_NORMAL)) { M_cpy(sdfm_d, pilf-> face_normal) }
						else { M_cpy(sdfm_d, current-> hit_n) }
					break;
				case 2:
					bstep= opaq_setp-> spherptr, pisoq= piloq-> surfp;
					if (!opaq_setp-> tributs)
					{
							/*** inside through the second layer of the sphere */
						dt= opaq_setp-> dist;
						if (kb)
						{
							if (!ka) { C_mult(opaq_spf, pisoq-> col_t)  continue; }
							M_add(current-> hit_p, current-> space_p, dt * current-> space_d)
							pisoq= piloq-> surfmix;
							cv_shadow(piloq, pisoq, 0, &shadhash);
							C_mult(opaq_spf, pisoq-> col_t)
							continue;
							}
						M_add(current-> hit_p, current-> space_p, dt * current-> space_d)
						if (ka)
						{
							pisoq= piloq-> surfmix;
							cv_shadow(piloq, pisoq, 0, &shadhash);
							}
						M_sub(sdfm_d, current-> hit_p, bstep-> centr)
						M_selfnorm(sdfm_d)
						}
					else
					{
						if (kb)
						{
								/*** vapor second and first layer */
							kb= opaq_setp-> tributs;
							if (!ka)
							{
								C_mult(opaq_spf, pisoq-> col_t)
								if (kb == 2) C_mult(opaq_spf, pisoq-> col_t)
								continue;
								}
							if (kb == 2)
							{
								M_add(current-> hit_p, current-> space_p, opaq_setp-> dost * current-> space_d)
								pisoq= piloq-> surfmix;
								cv_shadow(piloq, pisoq, 0, &shadhash);
								C_mult(opaq_spf, pisoq-> col_t)
								}
							M_add(current-> hit_p, current-> space_p, opaq_setp-> dist * current-> space_d)
							pisoq= piloq-> surfmix;
							cv_shadow(piloq, pisoq, 0, &shadhash);
							C_mult(opaq_spf, pisoq-> col_t)
							continue;
							}
						if (opaq_setp-> tributs == 2)
						{
								/*** difficult second sphere standard layer */
							M_add(current-> hit_p, current-> space_p, opaq_setp-> dost * current-> space_d)
							if (ka)
							{
								pisoq= piloq-> surfmix;
								cv_shadow(piloq, pisoq, 0, &shadhash);
								}
							M_sub(sdfm_d, current-> hit_p, bstep-> centr)
							M_selfnorm(sdfm_d)
							ddot= M_dot(vrdir, sdfm_d);  if (ddot < 0.0) ddot= -ddot;
							dtmp= pisoq-> sqindx + ddot * ddot -1.0;  if (dtmp < 1e-8) return 1;
							dv= ddot * pisoq-> indx * sqrt(dtmp);
							dt= pisoq-> in_t1 * (ddot * ddot) + 4 * dv +pisoq-> in_t2;
							if (dt > 1e-8)
							{
								dtmp= 4 * dv / dt;
								C_mult(opaq_spf, dtmp * pisoq-> col_t)
								}
							}
							/*** near sphere layer follow */
						M_add(current-> hit_p, current-> space_p, opaq_setp-> dist * current-> space_d)
						if (ka)
						{
							pisoq= piloq-> surfmix;
							cv_shadow(piloq, pisoq, 0, &shadhash);
							}
						M_sub(sdfm_d, current-> hit_p, bstep-> centr)
						M_selfnorm(sdfm_d)
						}
					break;

				case 3:
					bstep= opaq_setp-> spherptr, bmhit= opaq_setp-> warptr;
					pisoq= piloq-> surfp;
					if (!opaq_setp-> tributs)
					{
							/*** inside through the second layer of the sphere */
						dt= opaq_setp-> dist;
						if (kb)
						{
							if (!ka) { C_mult(opaq_spf, pisoq-> col_t)  continue; }
							M_add(current-> hit_p, current-> space_p, dt * current-> space_d)
							pisoq= piloq-> surfmix;
							cv_shadow(piloq, pisoq, 0, &shadhash);
							C_mult(opaq_spf, pisoq-> col_t)
							continue;
							}
						M_add(current-> hit_p, current-> space_p, dt * current-> space_d)
						if (ka)
						{
							pisoq= piloq-> surfmix;
							cv_shadow(piloq, pisoq, 0, &shadhash);
							}
						M_sub(sdfm_p, current-> hit_p, bstep-> centr)
						sdfm_d.x= M_dot(bmhit-> nmatx, sdfm_p),
						sdfm_d.y= M_dot(bmhit-> nmaty, sdfm_p),
						sdfm_d.z= M_dot(bmhit-> nmatz, sdfm_p);
						M_selfnorm(sdfm_d)
						}
					else
					{
						if (kb)
						{
								/*** vapor second and first layer */
							kb= opaq_setp-> tributs;
							if (!ka)
							{
								C_mult(opaq_spf, pisoq-> col_t)
								if (kb == 2) C_mult(opaq_spf, pisoq-> col_t)
								continue;
							}
							if (kb == 2)
							{
								M_add(current-> hit_p, current-> space_p, opaq_setp-> dost * current-> space_d)
								pisoq= piloq-> surfmix;
								cv_shadow(piloq, pisoq, 0, &shadhash);
								C_mult(opaq_spf, pisoq-> col_t)
							}
							M_add(current-> hit_p, current-> space_p, opaq_setp-> dist * current-> space_d)
							pisoq= piloq-> surfmix;
							cv_shadow(piloq, pisoq, 0, &shadhash);
							C_mult(opaq_spf, pisoq-> col_t)
							continue;
						}
						if (opaq_setp-> tributs == 2)
						{
					/*** most difficult second ovoid layer */
							M_add(current-> hit_p, current-> space_p, opaq_setp-> dost * current-> space_d)
							if (ka)
							{
								pisoq= piloq-> surfmix;
								cv_shadow(piloq, pisoq, 0, &shadhash);
							}
							M_sub(sdfm_p, current-> hit_p, bstep-> centr)
							sdfm_d.x= M_dot(bmhit-> nmatx, sdfm_p),
							sdfm_d.y= M_dot(bmhit-> nmaty, sdfm_p),
							sdfm_d.z= M_dot(bmhit-> nmatz, sdfm_p);
							M_selfnorm(sdfm_d)
							ddot= M_dot(vrdir, sdfm_d);  if (ddot < 0.0) ddot= -ddot;
							dtmp= pisoq-> sqindx + ddot * ddot -1.0;  if (dtmp < 1e-8) return 1;
							dv= ddot * pisoq-> indx * sqrt(dtmp);
							dt= pisoq-> in_t1 * (ddot * ddot) + 4 * dv +pisoq-> in_t2;
							if (dt > 1e-8)
							{
								dtmp= 4 * dv / dt;
								C_mult(opaq_spf, dtmp * pisoq-> col_t)
							}
						}
							/*** near sphere layer follow */
						M_add(current-> hit_p, current-> space_p, opaq_setp-> dist * current-> space_d)
						if (ka)
						{
							pisoq= piloq-> surfmix;
							cv_shadow(piloq, pisoq, 0, &shadhash);
						}

						M_sub(sdfm_p, current-> hit_p, bstep-> centr)
						sdfm_d.x= M_dot(bmhit-> nmatx, sdfm_p),
						sdfm_d.y= M_dot(bmhit-> nmaty, sdfm_p),
						sdfm_d.z= M_dot(bmhit-> nmatz, sdfm_p);
						M_selfnorm(sdfm_d)
					}
					break;
			}
			ddot= M_dot(vrdir, sdfm_d);  if (ddot < 0.0) ddot= -ddot;
			dtmp= pisoq-> sqindx + ddot * ddot -1.0;  if (dtmp < 1e-8) return 1;
			dv= ddot * pisoq-> indx * sqrt(dtmp);
			dt= pisoq-> in_t1 * (ddot * ddot) + 4 * dv +pisoq-> in_t2;
			if (dt < 1e-8)
			{
				/*** one [continue] or the other [return 1] ??? */
				continue;
			}
			dtmp= 4 * dv / dt;
			C_mult(opaq_spf, dtmp * pisoq-> col_t)
		}
		if ((opaq_spf.r + opaq_spf.g + opaq_spf.b) < 0.004) return 1;
	}
#endif

	return 0; 
}

static int crit_grid(er_grid * gp)
{
	short
		blits;
	vekU
		wik;
	er_cell *cell_base, *cell_limit;

	register realU
		tl, th, t1, t2, tx, ty, tz,
		delta_x, delta_y, delta_z;
	register short
		stp_x, stp_y, stp_z /*** , sY, sZ*/;
	register long
		sxy, sx, s;
	register er_cell
		*cell_p;

	if (vbits & RAY_PARAX)
	{
		if ((vis.x < gp-> lb.x) || (vis.x > gp-> hb.x)) return 0;
		tl= -1e300, th= 1e300;
	}
	else
	{
		if (vbits & RAY_NEGX)
			t2= (gp-> lb.x -vis.x) * vacc.x,
			t1= (gp-> hb.x -vis.x) * vacc.x;
		else
			t1= (gp-> lb.x -vis.x) * vacc.x,
			t2= (gp-> hb.x -vis.x) * vacc.x;
		if (t2 < 1e-20) return 0;
		tl= t1, th= t2;
	}
	if (vbits & RAY_PARAY)
	{  if ((vis.y < gp-> lb.y) || (vis.y > gp-> hb.y)) return 0; }
	else
	{
		if (vbits & RAY_NEGY)
			t2= (gp-> lb.y -vis.y) * vacc.y,
			t1= (gp-> hb.y -vis.y) * vacc.y;
		else
			t1= (gp-> lb.y -vis.y) * vacc.y,
			t2= (gp-> hb.y -vis.y) * vacc.y;
		if (t2 < 1e-20) return 0;
		if (t1 > tl) tl= t1;
		if (t2 < th) th= t2;
		if (th < tl) return 0;
	}
	if (vbits & RAY_PARAZ)
	{  if ((vis.z < gp-> lb.z) || (vis.z > gp-> hb.z)) return 0; }
	else
	{
		if (vbits & RAY_NEGZ)
			t2= (gp-> lb.z -vis.z) * vacc.z,
			t1= (gp-> hb.z -vis.z) * vacc.z;
		else
			t1= (gp-> lb.z -vis.z) * vacc.z,
			t2= (gp-> hb.z -vis.z) * vacc.z;
		if (t2 < 1e-20) return 0;
		if (t1 > tl) tl= t1;
		if (t2 < th) th= t2;
		if (th < tl) return 0;
	}
	if (tl < 1e-12) tl= 1e-12;

	blits= 0;
	M_add(wik, vis, tl * vrdir)
	M_minus(wik, gp-> vcbase)
	M_times(wik, gp-> vcscale)
	stp_x= (int) wik.x, stp_y= (int) wik.y, stp_z= (int) wik.z;
	if ((stp_x < 0) || (stp_x >= gp-> sx) ||
			(stp_y < 0) || (stp_y >= gp-> sy) ||
			(stp_z < 0) || (stp_z >= gp-> sz))
		{ return 0; }
	s= stp_x + gp-> sx * (stp_y + gp-> sy * stp_z);
	cell_base= gp-> cells, cell_limit= gp-> cellsmax;
	cell_p= cell_base +s;

	if (vbits & RAY_NEGX)
		tx= (gp-> vcbase.x + stp_x * gp-> vcsize.x - vis.x) * vacc.x;
		else tx= (gp-> vcbase.x + (stp_x +1) * gp-> vcsize.x -vis.x) * vacc.x;
	if (vbits & RAY_NEGY)
		ty= (gp-> vcbase.y + stp_y * gp-> vcsize.y - vis.y) * vacc.y;
		else ty= (gp-> vcbase.y + (stp_y +1) * gp-> vcsize.y -vis.y) * vacc.y;
	if (vbits & RAY_NEGZ)
		tz= (gp-> vcbase.z + stp_z * gp-> vcsize.z - vis.z) * vacc.z;
		else tz= (gp-> vcbase.z + (stp_z +1) * gp-> vcsize.z -vis.z) * vacc.z;
	if (vbits & RAY_PARAX) tx= 1e300;
	if (vbits & RAY_PARAY) ty= 1e300;
	if (vbits & RAY_PARAZ) tz= 1e300;
	if ((tx -th) > -1e-12) blits|= 1;
	if ((ty -th) > -1e-12) blits|= 2;
	if ((tz -th) > -1e-12) blits|= 4;
	th-= 1e-10;

#define RITPROT	if ((cell_p < cell_base) || (cell_p > cell_limit)) return 0;
#define	RRIT_CELL  RITPROT if (cell_p-> things && crit_cell(cell_p, 1)) { return 1; }
#define	ADVANKE_x	RRIT_CELL  tl= tx, /** stp_x+= s, */  tx+= delta_x;
#define	ADVANKE_y	RRIT_CELL  tl= ty, /** stp_y+= sY, */ ty+= delta_y;
#define	ADVANKE_z	RRIT_CELL  tl= tz, /** stp_z+= sZ, */ tz+= delta_z;

	s= 1, sx= gp-> sx, sxy= gp-> cell_subz; /*** sY= 1, sZ= 1; */
	delta_x= vacc.x * gp-> vcsize.x, delta_y= vacc.y * gp-> vcsize.y, delta_z= vacc.z * gp-> vcsize.z;
	if (vbits & RAY_NEGX) { s= -s,	delta_x= -delta_x; }
	if (vbits & RAY_NEGY) { sx= -sx,	/*** sY= -1,*/ delta_y= -delta_y; }
	if (vbits & RAY_NEGZ) { sxy= -sxy,	/*** sZ= -1,*/ delta_z= -delta_z; }

	switch (blits)
	{
		case 0:
			while (tl < th)
			{
				if (tx < ty)
				{  if (tx < tz) { ADVANKE_x  cell_p+= s; }
						else { ADVANKE_z  cell_p+= sxy; }
				}
				else 
				{  if (ty < tz) { ADVANKE_y  cell_p+= sx; }
						else { ADVANKE_z  cell_p+= sxy; }
				}
			}
			break;
		case 1:
			while (tl < th)
			{
				if (ty < tz) { ADVANKE_y  cell_p+= sx; }
					else { ADVANKE_z  cell_p+= sxy; }
			}
			break;
		case 2:
			while (tl < th)
			{
				if (tx < tz) { ADVANKE_x  cell_p+= s; }
					else { ADVANKE_z  cell_p+= sxy; }
			}
			break;
		case 4:
			while (tl < th)
			{
				if (tx < ty) { ADVANKE_x  cell_p+= s; }
					else { ADVANKE_y  cell_p+= sx; }
			}
			break;
		case 3: while (tl < th) { ADVANKE_z  cell_p+= sxy; } break;
		case 5: while (tl < th) { ADVANKE_y  cell_p+= sx; } break;
		case 6: while (tl < th) { ADVANKE_x  cell_p+= s; } break;
		case 7:  RRIT_CELL  break;
	}

	return 0;
}

/*
static int crit_cache(er_cell *cp)
{
	unsigned int istep, critcount;
	er_face 
		*pilf, **confa;
	double dv, dt;

	critcount= 0;
	confa= (er_face **) cp-> things;
	for (istep= cp-> count_tiny; (istep); istep--, confa++)
	{
		pilf= *confa;
		dv= M_dot(svrdir, pilf-> face_normal);
		if ((dv < 1e-12) && (dv > -1e-12)) continue;
		dt= M_dot(svis, pilf-> face_normal) +pilf-> plate_d, dt/= -dv;
		if ((dt < tmin) || (dt > tmax) || (pilf-> bits & (RACE_INVIS | RACE_LENS))
			|| crit_edged(pilf, dt) || (pilf-> ray_count == ray_num)) continue ;
		leshad-> cache_1= pilf, leshad-> cache_2= cp;
		critcount ++;
	}
	for (istep= cp-> count_large; (istep); istep--, confa++)
	{
		pilf= *confa;
		if (pilf-> ray_count== ray_num) continue ;
		pilf-> ray_count= ray_num;
		dv= M_dot(svrdir, pilf-> face_normal);
		if ((dv < 1e-12) && (dv > -1e-12)) continue;
		dt= M_dot(svis, pilf-> face_normal) +pilf-> plate_d, dt/= -dv;
		if ((dt < tmin) || (dt > tmax) || (pilf-> bits & (RACE_INVIS | RACE_LENS))
			|| crit_edged(pilf, dt))
			continue ;
		leshad-> cache_1= pilf, leshad-> cache_2= cp;
		critcount ++;
	}

	return critcount;
}
*/

static int crit_cell(er_cell *cp, int transparent)
{
	unsigned int istep, critcount;
	double dv, dt;
	er_face 
		*pilf, **confa;

	critcount= 0;
	confa= (er_face **) cp-> things;
	for (istep= cp-> count_tiny; (istep); istep--, confa++)
	{
		pilf= *confa;
		dv= M_dot(svrdir, pilf-> face_normal);
		if ((dv < 1e-12) && (dv > -1e-12)) continue;
		dt= M_dot(svis, pilf-> face_normal) +pilf-> plate_d, dt/= -dv;
		if ((dt < tmin) || (dt > tmax) || (pilf-> bits & RACE_INVIS)
			|| crit_edged(pilf, dt) || (pilf-> ray_count == ray_num)) continue ;
		if (pilf-> bits & RACE_LENS)
		{
			if (! transparent) continue;
			if (opaq_space == 32) return 32;
			opaq_setp-> kinmd= 1, opaq_setp-> dist= dt,
			opaq_setp-> surfat= pilf-> shading, opaq_setp-> facptr= pilf;
			opaq_setp++, opaq_space++;
			continue;
		}
		leshad-> cache_1= pilf;
		critcount ++;
	}
	for (istep= cp-> count_large; (istep); istep--, confa++)
	{
		pilf= *confa;
		if (pilf-> ray_count== ray_num) continue ;
		pilf-> ray_count= ray_num;
		dv= M_dot(svrdir, pilf-> face_normal);
		if ((dv < 1e-12) && (dv > -1e-12)) continue;
		dt= M_dot(svis, pilf-> face_normal) +pilf-> plate_d, dt/= -dv;
		if ((dt < tmin) || (dt > tmax) || (pilf-> bits & RACE_INVIS) || crit_edged(pilf, dt))
			continue ;
		if (pilf-> bits & RACE_LENS)
		{
			if (! transparent) continue;
			if (opaq_space == 32) return 1;
			opaq_setp-> kinmd= 1, opaq_setp-> dist= dt,
			opaq_setp-> surfat= pilf-> shading, opaq_setp-> facptr= pilf;
			opaq_setp++, opaq_space++;
			continue;
		}
		leshad-> cache_1= pilf;
		critcount ++;
	}

	if (critcount) leshad-> cache_2= cp;
	return critcount;
}

static int crit_edged(er_face *fip, double tat)
{
	register unsigned int j ;
	int k;
	double
		ptx, pty, dx;
	er_side *sip;

	j= fip-> bits & RACE_PLANEM;
	switch (j)
	{
		case RACE_PLANEX: ptx= vis.y + tat * vrdir.y, pty= vis.z + tat * vrdir.z;  break;
		case RACE_PLANEY: ptx= vis.z + tat * vrdir.z, pty= vis.x + tat * vrdir.x;  break;
		case RACE_PLANEZ: ptx= vis.x + tat * vrdir.x, pty= vis.y + tat * vrdir.y;  break;
	}

	for (sip= fip-> bound_sidea; 1; sip++)
	{
		j= sip-> bits;
		k= ((j & RSIDE_ABOVE) == RSIDE_ABOVE);
		if (j & RSIDE_Y) { dx= sip-> m * pty + sip-> b -ptx; }
			else { dx= sip-> m * ptx + sip-> b -pty; }
		if ((dx > 0.0) == k) return 1;
		if (j & RSIDE_END) break;
	}
	return 0;
}

