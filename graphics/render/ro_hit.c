#include "rrun.h"

	/***** ray */

extern shurt	ray_num, ray_rdepth, ray_gdepth;
extern int		ct_grids ;
extern ray_element	*current;
extern er_grid			*magic_grid;

#if	SPHERE_SUPPORT
	extern int		ct_ball, ct_deform;
	extern er_smot			*magic_spot;
	extern er_sform		*magic_deform;
#endif

extern er_camera	magic_cam;

#if ENVIR_SUPPORT
	extern int		ct_fog, ct_env0, ct_env1 ;
	extern vekU	fogmath;
	extern colU	fogcol;

#	define	FAR		fogmath.z
#	define FOGCALC		if (ct_fog && (dform>fogmath.x)) { dtmp= fogmath.y* (dform-fogmath.x), ddot= 1.0-dtmp; \
					current-> result_b= (dtmp * fogcol.r) + (ddot * current-> result_b), \
					current-> result_c= (dtmp * fogcol.g) + (ddot * current-> result_c), \
					current-> result_d= (dtmp * fogcol.b) + (ddot * current-> result_d); }

#else
#	define	FAR		1e304
#	define	FOGCALC
#endif

extern void		c_color(), ci_color(), cv_shadow();
extern void		rclear_facc(er_grid *),
				calc_spot(er_face *, int);

	/***** local */

	void	cast_ray(), glass_ray();
static	void	cast_grid(), cast_cell(), glass_cell();

		int	hit_edged();
static int	test_edged();

static	double	thit;
static er_face	*fahit;
static er_smot	*bahit;
static er_sform	*bmhit;

static	short		vbits;
static	vector		vis, vrdir, vacc;
static	svector		svis, svrdir;

#ifdef MAC
	cell_active	**clist_handle,
				*clist_a, *clist_b;
#else
	cell_active	clist_a[1024], clist_b[1024];
#endif

#define BACKFACK	1
#define RZMASK		0x3ff


	/*****/

#ifdef MAC
int	rhit_alloc()
{
	clist_handle= (cell_active **) NewHandle(2048 * sizeof(cell_active));
	return MemError() || rcrit_alloc();
}

void	rhit_pagelock()
{
	HLock((Handle) clist_handle);
	clist_a= *clist_handle, clist_b= clist_a +1024;
}
void	rhit_pageunlock()
{
	HUnlock((Handle) clist_handle);
	clist_a = (void *) 0xFFFFFFFF, clist_b=clist_a;
		// PTW3: set to invalid to value to make sure doesn't get referenced after
		// the handle's been unlocked.
}
void	rhit_dispose()
{
	DisposeHandle((Handle) clist_handle);
	rcrit_dispose();
}
#endif

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

void	cast_ray()
{
	unsigned short i, j;
	double
		dtmp, dform,
		ddot ;
#if	SPHERE_SUPPORT
	double d1;
	vekU
		sdfm_p, sdfm_d,
		bdir;
	er_smot *bstep;
	er_sform *bdstep;
#endif
	er_grid *gstep;
	cell_active *cap;

	clist_a[0].p= NULL; 
	vbits= 0;

	ray_num++;
	if (!ray_num) { rclear_facc(magic_grid);  ray_num++; }

	setup_ray(current);

	for (i= ct_grids, gstep= magic_grid; i--; gstep++) { cast_grid(gstep); }

	fahit= NULL, thit= FAR;
	if (NULL!= clist_a-> p)
	{
		for (cap= clist_a; (cap-> p) && (cap-> tnear < thit); cap++)
			{ cast_cell(cap-> p); } 
		}
#if	SPHERE_SUPPORT
	if (NULL!= magic_spot)
	{
		bstep= magic_spot, bahit= NULL, bmhit= NULL;
		for (i= ct_ball; (i--); bstep++)
		{
			M_sub(bdir, bstep-> centr, vis)
			ddot= M_dot(bdir, vrdir), dtmp= M_selfdot(bdir);
			if (dtmp < bstep-> radius2)
			{
				dtmp= bstep-> radius2 +(ddot*ddot) -dtmp;
				ddot += sqrt(dtmp);
				if (ddot < thit)
					{ thit= ddot, bahit= bstep; }
				continue;
				}
			dtmp= bstep-> radius2 +(ddot*ddot) -dtmp;

			if (dtmp < 0.0) continue;
			ddot -= sqrt(dtmp);
			if ((ddot > 0.0) && (ddot < thit))
				{ thit= ddot, bahit= bstep; }
			}
		for (i= ct_deform, bdstep= magic_deform; (i--); bstep++, bdstep++)
		{
			/***
				/// hit bounding sphere or box
				/// transform ray origin and direction
				/// normalize direction
				/// intersect with unit sphere
				/// if hit, inverse transform
				/// calculate distance and normal
				*/
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
			dtmp= M_selfdot(sdfm_p);
			ddot= -M_dot(sdfm_p, sdfm_d), dtmp= 1.0 +(ddot*ddot) -M_selfdot(sdfm_p);
			if (dtmp < 0.0) continue;
			d1= M_selfdot(sdfm_p);
			if (d1 < 1.0) { ddot += sqrt(dtmp); }
				else { ddot -= sqrt(dtmp); }
			if (ddot > 0.0)
			{
				M_add(bdir, sdfm_p, ddot * sdfm_d)
				sdfm_p.x= M_dot(bdstep-> matx, bdir),
				sdfm_p.y= M_dot(bdstep-> maty, bdir),
				sdfm_p.z= M_dot(bdstep-> matz, bdir);
				M_plus(sdfm_p, bdstep-> matw)
				M_minus(sdfm_p, vis)
				dtmp= sqrt(M_selfdot(sdfm_p));
				if (dtmp < thit)
					{ thit= dtmp, bahit= bstep, bmhit= bdstep; }
				}
			}
		if (bahit)
		{
			current-> target= NULL,
			current-> sarget= bahit,
			M_add(current-> hit_p, current-> space_p, thit *current-> space_d)
			if (bmhit)
			{
				M_sub(sdfm_d, current-> hit_p, bahit-> centr)
				bdir.x= M_dot(bmhit-> nmatx, sdfm_d),
				bdir.y= M_dot(bmhit-> nmaty, sdfm_d),
				bdir.z= M_dot(bmhit-> nmatz, sdfm_d);
				}
			else
			{
				M_sub(bdir, bahit-> centr, vis)  dtmp= M_selfdot(bdir);
				if (dtmp < bahit-> radius2) { M_sub(bdir, bahit-> centr, current-> hit_p) }
					else { M_sub(bdir, current-> hit_p, bahit-> centr) }
				}
			M_selfnorm(bdir)
#		if BACKFACK
			dtmp= M_dot(vrdir, bdir);
			if (dtmp > 0.0) { bdir.x= -bdir.x, bdir.y= -bdir.y, bdir.z= -bdir.z; }
#	endif
			M_cpy(current-> hit_n, bdir)

			dform= thit;
			c_color();
			FOGCALC
			return ;
			}

		/***/
#if 0
		smears
#endif
		}
#endif	// sphere support

	if (fahit)
	{
		current-> sarget= NULL,
		current-> target= fahit;
		M_add(current-> hit_p, current-> space_p, thit *current-> space_d)
		i= fahit-> bits & RACE_MASK;
		if (!(i & RACE_NORMAL)) { M_cpy(current-> hit_n, fahit-> face_normal) }
		if (i)
		{
				/**** hit edged */
			j= hit_edged(fahit, thit);
			calc_spot(fahit, (int) j);

			if (i & RACE_NORMAL)
			{
				ddot= M_dot(vrdir, current-> hit_n);
#if (!BACKFACK)
				if (ddot > 0.0)
					{ M_cpy(current-> hit_n, fahit-> face_normal) }
#endif
				}
			}

#if BACKFACK
		dtmp= M_dot(svrdir, fahit-> face_normal);
		if (dtmp > 0.0)
		{
			if ((i & RACE_NORMAL) && (ddot < 0.0)) { M_cpy(current-> hit_n, - fahit-> face_normal) }
				else { M_cpy(current-> hit_n, -current-> hit_n) }
			}
#endif

		dform= thit;
		c_color();
		FOGCALC
		return;
		}

#if ENVIR_SUPPORT
	if (ct_fog)
	{
		current-> result_a= 0.0,
		current-> result_b= fogcol.r,
		current-> result_c= fogcol.g, 
		current-> result_d= fogcol.b;
		current-> hashing_code= 1;
		return;
		}

	if (ct_env0) renv_primary(&vrdir);
	else
	{
		current-> result_a= 0.0, current-> result_b= 0.0, 
		current-> result_c= 0.0, current-> result_d= 0.0;
		current-> hashing_code= 0;
		}
#else

	current-> result_a= 0.0, current-> result_b= 0.0, 
	current-> result_c= 0.0, current-> result_d= 0.0;
	current-> hashing_code= 0;

#endif
}

void	cast_subray()
{
	shurt i, j;
	double
		dtmp, dform ;
#if	SPHERE_SUPPORT
	double
		ddot, d1;
	vekU
		sdfm_p, sdfm_d,
		bdir;
	er_smot *bstep;
	er_sform *bdstep;
#endif
	er_grid *gstep;
	cell_active *cap;

	clist_a[0].p= NULL; 
	vbits= 0;

	ray_num++;
	if (!ray_num) { rclear_facc(magic_grid);  ray_num++; }

	setup_ray(current);

	for (i= ct_grids, gstep= magic_grid; i--; gstep++) { cast_grid(gstep); }

	fahit= NULL, thit= FAR;
	if (NULL!= clist_a[0].p)
	{
		for (cap= clist_a; (cap-> p) && (cap-> tnear < thit); cap++)
			{ cast_cell(cap-> p); } 
		}

#if	SPHERE_SUPPORT
	if (NULL!= magic_spot)
	{
		bstep= magic_spot, bahit= NULL, bmhit= NULL;
		for (i= ct_ball; (i--); bstep++)
		{
			M_sub(bdir, bstep-> centr, vis)
			ddot= M_dot(bdir, vrdir), dtmp= bstep-> radius2 +(ddot*ddot) -M_selfdot(bdir);
			if (dtmp < 0.0) continue;
			ddot -= sqrt(dtmp);
			if ((ddot > 0.0) && (ddot < thit))
				{ thit= ddot, bahit= bstep; }
			}
		for (i= ct_deform, bdstep= magic_deform; (i--); bstep++, bdstep++)
		{
			/***
				/// hit bounding sphere or box
				/// transform ray origin and direction
				/// normalize direction
				/// intersect with unit sphere
				/// if hit, inverse transform
				/\/ calculate distance and normal
				*/
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
			dtmp= M_selfdot(sdfm_p);
			ddot= -M_dot(sdfm_p, sdfm_d), dtmp= 1.0 +(ddot*ddot) -M_selfdot(sdfm_p);
			if (dtmp < 0.0) continue;
			d1= M_selfdot(sdfm_p);
			if (d1 < 1.0) { ddot += sqrt(dtmp); }
				else { ddot -= sqrt(dtmp); }
			if (ddot > 0.0)
			{
				M_add(bdir, sdfm_p, ddot * sdfm_d)
				sdfm_p.x= M_dot(bdstep-> matx, bdir),
				sdfm_p.y= M_dot(bdstep-> maty, bdir),
				sdfm_p.z= M_dot(bdstep-> matz, bdir);
				M_plus(sdfm_p, bdstep-> matw)
				M_minus(sdfm_p, vis)
				dtmp= sqrt(M_selfdot(sdfm_p));
				if (dtmp < thit)
					{ thit= dtmp, bahit= bstep, bmhit= bdstep; }
				}
			}
		if (bahit)
		{
			current-> target= NULL,
			current-> sarget= bahit,
			M_add(current-> hit_p, current-> space_p, thit *current-> space_d)
			if (bmhit)
			{
				M_sub(sdfm_d, current-> hit_p, bahit-> centr)
				bdir.x= M_dot(bmhit-> nmatx, sdfm_d),
				bdir.y= M_dot(bmhit-> nmaty, sdfm_d),
				bdir.z= M_dot(bmhit-> nmatz, sdfm_d);
				}
				else { M_sub(bdir, current-> hit_p, bahit-> centr) }
			M_selfnorm(bdir)
			M_cpy(current-> hit_n, bdir)
			dform= thit;
			c_color();
			FOGCALC
			return ;
			}

		/***/
#if 0
		smears
#endif
		}
#endif	// sphere support

	if (fahit)
	{
		current-> sarget= NULL,
		current-> target= fahit;
		M_add(current-> hit_p, current-> space_p, thit *current-> space_d)
		i= fahit-> bits & RACE_MASK;
		if (!(i & RACE_NORMAL)) { M_cpy(current-> hit_n, fahit-> face_normal) }
		if (i)
		{
				/**** hit edged */
			j= hit_edged(fahit, thit);
			calc_spot(fahit, (int) j);
			}

#	if BACKFACK
		dtmp= M_dot(vrdir, current-> hit_n);
		if (dtmp > 0.0)
		{
			current-> hit_n.x= -current-> hit_n.x,
			current-> hit_n.y= -current-> hit_n.y,
			current-> hit_n.z= -current-> hit_n.z;
			}
#	endif

		dform= thit;
		c_color();
		FOGCALC
		return;
		}

#if ENVIR_SUPPORT
	if (ct_fog)
	{
		current-> result_a= 0.0,
		current-> result_b= fogcol.r,
		current-> result_c= fogcol.g, 
		current-> result_d= fogcol.b;
		current-> hashing_code= 1;
		return;
		}

	if (ct_env1) renv_sub(&vrdir);
	else
	{
		current-> result_a= 0.0, current-> result_b= 0.0, 
		current-> result_c= 0.0, current-> result_d= 0.0;
		current-> hashing_code= 0;
		}
#else

	current-> result_a= 0.0, current-> result_b= 0.0, 
	current-> result_c= 0.0, current-> result_d= 0.0;
	current-> hashing_code= 0;

#endif
}

static void cast_grid(gp)
er_grid *gp;
{
	short
		blits;
	vekU
		wik;
	cell_active
		*csrc, *cacu;
	er_cell
		*cell_base, *cell_limit;
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
		if ((vis.x < gp-> lb.x) || (vis.x > gp-> hb.x)) return ;
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
		if (t2 < 0.0) return ;
		tl= t1, th= t2;
		}
	if (vbits & RAY_PARAY)
	{  if ((vis.y < gp-> lb.y) || (vis.y > gp-> hb.y)) return ; }
	else
	{
		if (vbits & RAY_NEGY)
			t2= (gp-> lb.y -vis.y) * vacc.y,
			t1= (gp-> hb.y -vis.y) * vacc.y;
		else
			t1= (gp-> lb.y -vis.y) * vacc.y,
			t2= (gp-> hb.y -vis.y) * vacc.y;
		if (t2 < 0.0) return ;
		if (t1 > tl) tl= t1;
		if (t2 < th) th= t2;
		if (th < tl) return ;
		}
	if (vbits & RAY_PARAZ)
	{  if ((vis.z < gp-> lb.z) || (vis.z > gp-> hb.z)) return ; }
	else
	{
		if (vbits & RAY_NEGZ)
			t2= (gp-> lb.z -vis.z) * vacc.z,
			t1= (gp-> hb.z -vis.z) * vacc.z;
		else
			t1= (gp-> lb.z -vis.z) * vacc.z,
			t2= (gp-> hb.z -vis.z) * vacc.z;
		if (t2 < 0.0) return ;
		if (t1 > tl) tl= t1;
		if (t2 < th) th= t2;
		if (th < tl) return ;
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
	{
		return ;
		}
	s= (long) stp_x + gp-> sx * (stp_y + gp-> sy * stp_z);
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

	cacu= clist_a, csrc= clist_b;
	if (cacu-> p)
	{
		while (cacu-> p) 
			{ csrc-> p= cacu-> p, csrc-> tnear= cacu-> tnear; csrc++, cacu++; }
		csrc-> p= NULL, cacu= clist_a, csrc= clist_b;
		} else csrc-> p= NULL;


#define MEMPROT	if ((cell_p < cell_base) || (cell_p > cell_limit)) break;
#define	TEST_CELL  MEMPROT	if (cell_p-> things) { while (csrc-> p && (csrc-> tnear < tl)) \
	{ cacu->p= csrc->p, cacu->tnear= csrc->tnear; csrc++, cacu++; } \
	cacu-> p= cell_p, cacu-> tnear= tl, cacu++; }
#define	ADVANCE_x	TEST_CELL  tl= tx, tx+= delta_x;
#define	ADVANCE_y	TEST_CELL  tl= ty, ty+= delta_y;
#define	ADVANCE_z	TEST_CELL  tl= tz, tz+= delta_z;

	s= 1, sx= gp-> sx, sxy= gp-> cell_subz; /*** sY= 1, sZ= 1; */
	delta_x= vacc.x * gp-> vcsize.x, delta_y= vacc.y * gp-> vcsize.y, delta_z= vacc.z * gp-> vcsize.z;
	if (vbits & RAY_NEGX) { s= -s,	delta_x= -delta_x; }
	if (vbits & RAY_NEGY) { sx= -sx,	delta_y= -delta_y; }
	if (vbits & RAY_NEGZ) { sxy= -sxy,	delta_z= -delta_z; }

	switch (blits)
	{
		case 0:
			while (tl < th)
			{
				if (tx < ty)
				{  if (tx < tz) { ADVANCE_x  cell_p+= s; }
					else { ADVANCE_z  cell_p+= sxy; }
					}
				else 
				{  if (ty < tz) { ADVANCE_y  cell_p+= sx; }
					else { ADVANCE_z  cell_p+= sxy; }
					}
				}
			break;
		case 1:
			while (tl < th)
			{
				if (ty < tz) { ADVANCE_y  cell_p+= sx; }
					else { ADVANCE_z  cell_p+= sxy; }
				}
			break;
		case 2:
			while (tl < th)
			{
				if (tx < tz) { ADVANCE_x  cell_p+= s; }
					else { ADVANCE_z  cell_p+= sxy; }
				}
			break;
		case 4:
			while (tl < th)
			{
				if (tx < ty) { ADVANCE_x  cell_p+= s; }
					else { ADVANCE_y  cell_p+= sx; }
				}
			break;
		case 3: while (tl < th) { ADVANCE_z  cell_p+= sxy; } break;
		case 5: while (tl < th) { ADVANCE_y  cell_p+= sx; } break;
		case 6: while (tl < th) { ADVANCE_x  cell_p+= s; } break;
		case 7:  TEST_CELL  break;
		}
	while (csrc-> p) { cacu->p= csrc->p, cacu->tnear= csrc->tnear; csrc++, cacu++; } 
	cacu-> p= NULL;  /***** NULL terminated list after all. */
	}

static void cast_cell(cp)
er_cell *cp;
{
	shurt i;
	er_face 
		*pilf, **confa;
	realU
		dv, dt;

	confa= (er_face **) cp-> things;
	for (i= cp-> count_tiny; (i); i--, confa++)
	{
		pilf= *confa;
		dv= M_dot(svrdir, pilf-> face_normal);
#if BACKFACK
		if ((dv < 1e-12) && (dv > -1e-12)) continue;
		dt= M_dot(svis, pilf-> face_normal) +pilf-> plate_d;
		dt/= -dv;  if (dt < 0.0) continue;
		if (dv >= 0.0) { /*** backface adjustment */ dt += 1e-10; }
		if ((dt > thit) || test_edged(pilf, dt)) continue ;
#else
		if (dv >= 0.0) continue ;
		dt= M_dot(svis, pilf-> face_normal) +pilf-> plate_d;
		dt/= -dv;
		if ((dt < 0.0) || (dt > thit) || test_edged(pilf, dt)) continue ;
#endif
		fahit= pilf, thit= dt;
		}
	for (i= cp-> count_large; (i); i--, confa++)
	{
		pilf= *confa;
#ifdef HARD
		if (!pilf) 
			{ printf("\n >> warning	\n");  return; }
#endif
		if (pilf-> ray_count== ray_num) continue ;
		pilf-> ray_count= ray_num;
		dv= M_dot(svrdir, pilf-> face_normal);
#if BACKFACK
		if ((dv < 1e-12) && (dv > -1e-12)) continue;
		dt= M_dot(svis, pilf-> face_normal) +pilf-> plate_d, dt/= -dv;
		if (dt < 0.0) continue;
		if (dv >= 0.0) { /*** backface adjustment */ dt += 1e-10; }
		if ((dt > thit) || test_edged(pilf, dt)) continue ;
#else
		if (dv >= 0.0) continue ;
		dt= M_dot(svis, pilf-> face_normal) +pilf-> plate_d;
		dt/= -dv;
		if ((dt < 0.0) || (dt > thit) || test_edged(pilf, dt)) continue ;
#endif
		fahit= pilf, thit= dt;
		}
	}

int hit_edged(fip, tat)
er_face *fip;
realU tat;
{
	register short
		j, k, saz;
	realU
		ptx, pty, dx;
	er_side *sip;

	j= fip-> bits;
	if (( j & RZMASK) < 4) return 1;
	switch (j & RACE_PLANEM)
	{
		case RACE_PLANEX: ptx= vis.y + tat * vrdir.y, pty= vis.z + tat * vrdir.z;  break;
		case RACE_PLANEY: ptx= vis.z + tat * vrdir.z, pty= vis.x + tat * vrdir.x;  break;
		case RACE_PLANEZ: ptx= vis.x + tat * vrdir.x, pty= vis.y + tat * vrdir.y;  break;
		}
	saz= (j & RZMASK);
	
	for (sip= fip-> bound_sidea +saz, saz-= 3; saz; saz--, sip++)
	{
		j= sip-> bits;
		k= ((j & RSIDE_ABOVE) == RSIDE_ABOVE);
		if (j & RSIDE_Y) { dx= sip-> m * pty + sip-> b -ptx; }
			else { dx= sip-> m * ptx + sip-> b -pty; }
		if ((dx > 0.0) == k) return (j & RZMASK);
		}
	return 1;
	}

	/**********/

void	glass_ray() 
{ 
	shurt i, j;
	realU
		dtmp, dform ;
#if	SPHERE_SUPPORT
	double
		ddot, dinsid;
	vekU
		sdfm_p, sdfm_d,
		bdir;
	er_smot *bstep;
	er_sform *bdstep;
#endif
	er_grid *gstep;
	cell_active *cap;

	clist_a[0].p= NULL; 
	vbits= 0;

	ray_num++;
	if (!ray_num) { rclear_facc(magic_grid);  ray_num++; }

	setup_ray(current);

	for (i= ct_grids, gstep= magic_grid; i--; gstep++) { cast_grid(gstep); }
	thit= 1e304, fahit= NULL;
	if (NULL!= clist_a-> p)
	{
		for (cap= clist_a; (cap-> p) && (cap-> tnear < thit); cap++)
			{ glass_cell(cap-> p); } 
		}
#if	SPHERE_SUPPORT
	if (NULL!= magic_spot)
	{
		bstep= magic_spot, bahit= NULL;
		for (i= ct_ball; (i--); bstep++)
		{
			M_sub(bdir, bstep-> centr, vis)
			ddot= M_dot(bdir, vrdir), dtmp= M_selfdot(bdir);
			if (dtmp < bstep-> radius2)
			{
				dtmp= bstep-> radius2 +(ddot*ddot) -dtmp;
				ddot += sqrt(dtmp);
				if (ddot < thit)
					{ thit= ddot, bahit= bstep; }
				continue;
				}
			dtmp= bstep-> radius2 +(ddot*ddot) -dtmp;

			if (dtmp < 0.0) continue;
			ddot -= sqrt(dtmp);
			if ((ddot > 0.0) && (ddot < thit))
									{ thit= ddot, bahit= bstep; }
			}
		for (i= ct_deform, bdstep= magic_deform; (i--); bstep++, bdstep++)
		{
			/***
				/// hit bounding sphere or box
				/// transform ray origin and direction
				/// normalize direction
				/// intersect with unit sphere
				/// if hit, inverse transform
				/\/ calculate distance and normal
				*/
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
			ddot= -M_dot(sdfm_p, sdfm_d), dtmp= 1.0 +(ddot*ddot) -M_selfdot(sdfm_p);
			if (dtmp < 0.0) continue;
			dinsid= M_selfdot(sdfm_p);
			if (dinsid < 1.0) { ddot += sqrt(dtmp); }
									else { ddot -= sqrt(dtmp); }
			if (ddot > 0.0)
			{
				M_add(bdir, sdfm_p, ddot * sdfm_d)
				sdfm_p.x= M_dot(bdstep-> matx, bdir),
				sdfm_p.y= M_dot(bdstep-> maty, bdir),
				sdfm_p.z= M_dot(bdstep-> matz, bdir);
				M_plus(sdfm_p, bdstep-> matw)
				M_minus(sdfm_p, vis)
				dtmp= sqrt(M_selfdot(sdfm_p));
				if (dtmp < thit)
					{ thit= dtmp, bahit= bstep, bmhit= bdstep; }
				}
			}

		if (bahit)
		{
			current-> target= NULL,
			current-> sarget= bahit,
			M_add(current-> hit_p, current-> space_p, thit *current-> space_d)
			if (bmhit)
			{
				M_sub(sdfm_d, current-> hit_p, bahit-> centr)
				bdir.x= M_dot(bmhit-> nmatx, sdfm_d),
				bdir.y= M_dot(bmhit-> nmaty, sdfm_d),
				bdir.z= M_dot(bmhit-> nmatz, sdfm_d);
				}
				else { M_sub(bdir, current-> hit_p, bahit-> centr) }
			M_selfnorm(bdir)
			M_cpy(current-> hit_n, bdir)
			dform= thit;
			ci_color();
			FOGCALC
			return ;
			}
		}
#endif	// sphere support

	if (NULL!= fahit)
	{
		current-> sarget= NULL,
		current-> target= fahit;
		M_add(current-> hit_p, current-> space_p, thit *current-> space_d)
		i= fahit-> bits & RACE_MASK;
		if (!(i & RACE_NORMAL)) { M_cpy(current-> hit_n, fahit-> face_normal) }
		if (i)
		{
				/**** hit edged */
			j= hit_edged(fahit, thit);
			calc_spot(fahit, (int) j);
			}
#	if BACKFACK
		dtmp= M_dot(vrdir, current-> hit_n);
		if (dtmp < 0.0)
		{
			current-> hit_n.x= -current-> hit_n.x,
			current-> hit_n.y= -current-> hit_n.y,
			current-> hit_n.z= -current-> hit_n.z;
			}
#	endif

		dform= thit;
		ci_color();
		FOGCALC
		return ;
		}

#if ENVIR_SUPPORT
	if (ct_fog)
	{
		current-> result_a= 0.0,
		current-> result_b= fogcol.r,
		current-> result_c= fogcol.g, 
		current-> result_d= fogcol.b;
		current-> hashing_code= 1;
		}
	else
	{
		current-> result_a= 0.0, current-> result_b= 0.0, 
		current-> result_c= 0.0, current-> result_d= 0.0;
		current-> hashing_code= 0;
		}
#else

	current-> result_a= 0.0, current-> result_b= 0.0, 
	current-> result_c= 0.0, current-> result_d= 0.0;
	current-> hashing_code= 0;

#endif
}

static void glass_cell(cp)
er_cell *cp;
{
	shurt i;
	er_face 
		*pilf, **confa;
	realU
		dv, dt;
	
	confa= (er_face **) cp-> things;
	for (i= cp-> count_tiny; (i); i--, confa++)
	{
		pilf= *confa;
		dv= M_dot(svrdir, pilf-> face_normal);
#if BACKFACK
		if ((dv < 1e-12) && (dv > -1e-12)) continue;
		dt= M_dot(vis, pilf-> face_normal) +pilf-> plate_d, dt/= -dv;
		if (dt < 0.0) continue;
		if (dv < 0.0) { /*** backface adjustment */ dt += 1e-10; }
		if ((dt > thit) || test_edged(pilf, dt)) continue ;
#else
		if (dv < 1e-12) continue ;
		dt= M_dot(vis, pilf-> face_normal) +pilf-> plate_d;
		dt/= -dv;
		if ((dt < 0.0) || (dt > thit) || test_edged(pilf, dt)) continue ;
#endif
		fahit= pilf, thit= dt;
		}
	for (i= cp-> count_large; (i); i--, confa++)
	{
		pilf= *confa;
#ifdef HARD
		if (!pilf) 
			{ printf("\n >> warning	\n");  return; }
#endif
		if (pilf-> ray_count== ray_num) continue ;
		pilf-> ray_count= ray_num;
		dv= M_dot(svrdir, pilf-> face_normal);
#if BACKFACK
		if ((dv < 1e-12) && (dv > -1e-12)) continue;
		dt= M_dot(vis, pilf-> face_normal) +pilf-> plate_d, dt/= -dv;
		if (dt < 0.0) continue;
		if (dv < 0.0) { /*** backface adjustment */ dt += 1e-10; }
		if ((dt > thit) || test_edged(pilf, dt)) continue ;
#else
		if (dv < 1e-12) continue ;
		dt= M_dot(vis, pilf-> face_normal) +pilf-> plate_d;
		dt/= -dv;
		if ((dt < 0.0) || (dt > thit) || test_edged(pilf, dt)) continue ;
#endif
		fahit= pilf, thit= dt;
		}
	}

	/**********/

static int test_edged(fip, tat)
er_face *fip;
realU tat;
{
	register short j, k;
	realU
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

