#include "rrun.h"
#include "escene.h"

	/*****/

thing { int x, y, z; } vekI;

#define	LZ_corner   gmix= (v_wind-> z -gstep_z) / (v_wind-> z -v_fol-> z), dtmp= 1.0 -gmix; \
			scale_fill-> x= (dtmp *v_wind-> x) +(gmix *v_fol-> x), \
			scale_fill-> y= (dtmp *v_wind-> y) +(gmix *v_fol-> y);

#define	HZ_corner   gmix= (v_wind-> z -gframe) / (v_wind-> z -v_fol-> z), dtmp= 1.0 -gmix; \
			scale_fill-> x= (dtmp *v_wind-> x) +(gmix *v_fol-> x); \
			scale_fill-> y= (dtmp *v_wind-> y) +(gmix *v_fol-> y);  gid= scale_fill-> y;

#define	LY_corner   gmix= (v_wind-> y -gstep_y) / (v_wind-> y -v_fol-> y); \
			dtmp= 1.0 -gmix, dhold= (dtmp *v_wind-> x) +(gmix *v_fol-> x);  gid= dhold;

#define	HY_corner   gmix= (v_wind-> y -gframe) / (v_wind-> y -v_fol-> y); \
			dtmp= 1.0 -gmix, dhold= (dtmp *v_wind-> x) +(gmix *v_fol-> x);  gid= dhold;

#if 0
#define SAFE
static void trap_error()
	{ printf("eeep grid\n"); }
#endif

static svector	hold_y[FLAT_SPACE], hold_z[FLAT_SPACE];
extern svector	*workspace;

	/*****/

void	rgrid_size(GritP, UseDyne)
	er_grid *GritP;
	long *UseDyne;
{
	short
		loopg,
		loopi, 
		b_altface,
		gsi_z, gsi_y;
	shurt
		*sid_step;
	int
		gid, gframe,
		gstep_z, gstep_y, gstep_x,
		g_minz, g_miny;
	long
		loopjl,
		gszx, gszxy, 
		kv, ncel;
	realU
		dtmp, dhold,
		gmix;
	vekI
		glb, ghb;
	vector
		scale_off, scale_mult,
		*hv_step;
	svector
		*vek_ref, *vek_lookup,
		*scale_step, *scale_fill, *scale_follow,
		*v_wind, *v_fol, *v_minz, *v_miny;
	er_cell *cell_base, *cell_fill;
	er_face *step_f;
	er_uface *step_altf;
	tr_object
		**content_step,
		*content_lookup;

	scale_off= GritP-> vcbase, scale_mult= GritP-> vcscale;
	vek_ref= workspace -1;
	cell_base= GritP-> cells;
	gszx= GritP-> sx,
	gszxy= (long) GritP-> sx * GritP-> sy;
	ncel= 0;
	for (loopi= GritP-> cn, content_step= (tr_object **) GritP-> contain; (loopi--); content_step++)
	{
		content_lookup= *content_step;
		b_altface= 0, step_altf= NULL ;
		for (loopjl= content_lookup-> ta.vector_c,
				hv_step= content_lookup-> ta.vlist,
				scale_fill= workspace;
			(loopjl--); hv_step++, scale_fill++)
		{
			scale_fill-> x= scale_mult.x * (hv_step-> x -scale_off.x) ;
			scale_fill-> y= scale_mult.y * (hv_step-> y -scale_off.y) ;
			scale_fill-> z= scale_mult.z * (hv_step-> z -scale_off.z) ;
		}
		step_f= (er_face *) content_lookup-> ta.flist;
		for (loopjl= content_lookup-> ta.poly_c; loopjl--; step_f++)
		{
			if (b_altface) { step_f= (er_face *) step_altf; }
			if (! step_f-> bits) continue ;

			gsi_z= step_f-> bits & 0x3ff;
			sid_step= step_f-> indic_sides, scale_fill= hold_z;  
			vek_lookup= vek_ref + *sid_step,
				*scale_fill= *vek_lookup,
				glb.x= ghb.x= vek_lookup-> x,
				glb.y= ghb.y= vek_lookup-> y,
				glb.z= ghb.z= vek_lookup-> z;
			for (loopg= gsi_z -1, sid_step++, scale_fill++; (loopg--); sid_step++, scale_fill++)
			{
				vek_lookup= vek_ref + *sid_step,
				gid= scale_fill-> x= vek_lookup-> x;
					if (gid < glb.x) { glb.x= gid; } else if (gid > ghb.x) { ghb.x= gid; }
				gid= scale_fill-> y= vek_lookup-> y;
					if (gid < glb.y) { glb.y= gid; } else if (gid > ghb.y) { ghb.y= gid; }
				gid= scale_fill-> z= vek_lookup-> z;
					if (gid < glb.z) { glb.z= gid; } else if (gid > ghb.z) { ghb.z= gid; }
			}
			if ((glb.x < 0) || (glb.y < 0) || (glb.z < 0))
			{
				FILE * ferr= fopen("err.txt", "a") ;
				if (ferr) { fprintf(ferr, "failed base alignment\n" ) ;  fclose(ferr) ; }
				continue ;
			}

			if ((glb.x == ghb.x) && (glb.y == ghb.y) && (glb.z == ghb.z))
			{
				kv= glb.x +(glb.y *gszx) +(glb.z *gszxy);
				cell_fill= cell_base +kv, (cell_fill-> count_tiny) ++, ncel++;
				continue ;
			}

/******* go to it ******/

			for (gstep_z= glb.z, v_minz= NULL; (gstep_z <= ghb.z); gstep_z++)
			{
				if (glb.z== ghb.z) 
				{
					gsi_y= gsi_z;
					for (loopg= gsi_y, scale_step= hold_z, scale_fill= hold_y; 
							(loopg--); scale_step++, scale_fill++ )
						{ scale_fill-> x= scale_step-> x, scale_fill-> y= scale_step-> y; }
					v_wind= NULL;
					goto RasterY;
				}

				gframe= gstep_z +1;  scale_fill= hold_y;
				if (NULL== v_minz)
				{
					for (loopg= gsi_z, v_minz= hold_z, v_fol= hold_z +gsi_z -1; loopg; loopg--) 
					{
						if (v_minz-> z < gframe) break;
						v_fol= v_minz, v_minz++;
					}
					if (!loopg) { printf("panik (-1gz)\n");  break; }
					if ((loopg == gsi_z) && ((int) v_fol-> z == gstep_z))
					{
						for ( loopg= 0; 
								(loopg < gsi_z) && ((int) v_fol-> z < gframe); loopg++) 
							{ v_minz= v_fol, v_fol--; }
					}
					g_minz= loopg;

					v_wind= v_minz;
/*cm1*/     		HZ_corner	gsi_y= 1, glb.y= ghb.y= gid, scale_fill++;

					while (v_wind-> z < gframe)
					{
						gid= scale_fill-> y= v_wind-> y, scale_fill-> x= v_wind-> x;
						if (gid < glb.y) { glb.y= gid; } else
							{ if (gid > ghb.y) ghb.y= gid; }
						gsi_y++, scale_fill++;  v_fol= v_wind, v_wind++, loopg--;
						if (!loopg) { v_wind= hold_z, loopg= gsi_z; }
					}

/*cm2*/     		HZ_corner	gsi_y++, scale_fill++;
					if (gid < glb.y) { glb.y= gid; } else
						{ if (gid > ghb.y) ghb.y= gid; }
					goto RasterY;
				}
				v_fol= v_minz, v_wind= v_minz +1, loopg= g_minz -1;
					if (!loopg) { v_wind= hold_z, loopg= gsi_z; }
				while (v_wind-> z < gstep_z)
				{
					v_fol= v_wind, v_wind++, loopg--;
					if (!loopg) { v_wind= hold_z, loopg= gsi_z; }
				}

/*cn1*/			LZ_corner
				gsi_y= 1, scale_follow= scale_fill, scale_fill++;
				gid= scale_follow-> y, glb.y= ghb.y= gid;

				while ((int) v_wind-> z== gstep_z)
				{
					gid= scale_fill-> y= v_wind-> y, scale_fill-> x= v_wind-> x;
					if (gid < glb.y) { glb.y= gid; } else
						{ if (gid > ghb.y) ghb.y= gid; }
					gsi_y++, scale_fill++;  v_fol= v_wind, v_wind++, loopg--;
					if (!loopg) { v_wind= hold_z, loopg= gsi_z; }
				}

/*cn5*/			if ((int) v_wind-> z < gstep_z)
				{
					LZ_corner  gsi_y++, scale_follow= scale_fill++;  gid= scale_follow-> y;
					if (gid < glb.y) { glb.y= gid; } else
						{ if (gid > ghb.y) ghb.y= gid; }
					goto RasterY;
				}

/*cn2*/			HZ_corner	gsi_y++, scale_fill++;
				if (gid < glb.y) { glb.y= gid; } else
					{ if (gid > ghb.y) ghb.y= gid; }

				while ((int) v_wind-> z > gstep_z)
				{
					v_fol= v_wind, v_wind++, loopg--;
					if (!loopg) { v_wind= hold_z, loopg= gsi_z; }
				}

/*cn3*/			HZ_corner	gsi_y++, scale_fill++;
				if (gid < glb.y) { glb.y= gid; } else
					{ if (gid > ghb.y) ghb.y= gid; }

				while ((int) v_wind-> z == gstep_z)
				{
					gid= scale_fill-> y= v_wind-> y, scale_fill-> x= v_wind-> x;
					if (gid < glb.y) { glb.y= gid; } else
						{ if (gid > ghb.y) ghb.y= gid; }
					gsi_y++, scale_fill++;  v_fol= v_wind, v_wind++, loopg--;
					if (!loopg) { v_wind= hold_z, loopg= gsi_z; }
				}

/*cn4*/			LZ_corner
				gsi_y++, scale_follow= scale_fill++;
				gid= scale_follow-> y;
				if (gid < glb.y) { glb.y= gid; } else
					{ if (gid > ghb.y) ghb.y= gid; }

	RasterY:
				for (gstep_y= glb.y, v_miny= NULL; (gstep_y <= ghb.y); gstep_y++)
				{
					if (glb.y== ghb.y)
					{
						loopg= gsi_y -1, scale_step= hold_y;
						gid= scale_step-> x, scale_step++;
						glb.x= ghb.x= gid;
						for (; loopg--; scale_step++)
						{
							gid= scale_step-> x;
							if (gid < glb.x) { glb.x= gid; } else
								{ if (gid > ghb.x) ghb.x= gid; }
						}
						v_wind= NULL;
						goto RasterX;
					}

					gframe= gstep_y +1;
					if (v_miny== NULL)
					{
						for (loopg= gsi_y, v_miny= hold_y, v_fol= hold_y +gsi_y -1; (loopg); loopg--) 
						{
							if (v_miny-> y < gframe) break;
							v_fol= v_miny, v_miny++;
						}
						if (!loopg) { printf("grid tiling panik (-1gy)\n");  break; }
						if ((loopg == gsi_y) && ((int) v_fol-> y== gstep_y))
						{
							for ( loopg= 0; 
									(loopg < gsi_y) && (v_fol-> y < gframe); loopg++) 
								{ v_miny= v_fol, v_fol--; }
						}
						g_miny= loopg;

/*cm1*/		v_wind= v_miny;
						HY_corner  glb.x= ghb.x= gid;

						while (v_wind-> y < gframe)
						{
							gid= v_wind-> x;
							if (gid < glb.x) { glb.x= gid; } else
								{ if (gid > ghb.x) ghb.x= gid; }
							v_fol= v_wind, v_wind++, loopg--;
							if (!loopg) { v_wind= hold_y, loopg= gsi_y; }
						}

/*cm2*/					HY_corner
						if (gid < glb.x) { glb.x= gid; } else
							{ if (gid > ghb.x) ghb.x= gid; }

						goto RasterX;
					}

					v_fol= v_miny, v_wind= v_miny +1, loopg= g_miny -1;
						if (!loopg) { v_wind= hold_y, loopg= gsi_y; }
					while (v_wind-> y < gstep_y)
					{
						v_fol= v_wind, v_wind++, loopg--;
						if (!loopg) { v_wind= hold_y, loopg= gsi_y; }
					}

/*cy1*/				LY_corner  glb.x= ghb.x= gid;
					while ((int) v_wind-> y== gstep_y)
					{
						gid= v_wind-> x;
						if (gid < glb.x) { glb.x= gid; } else
							{ if (gid > ghb.x) ghb.x= gid; }
						v_fol= v_wind, v_wind++, loopg--;
						if (!loopg) { v_wind= hold_y, loopg= gsi_y; }
					}

/*cy5*/				if ((int) v_wind-> y < gstep_y)
					{
						LY_corner
						if (gid < glb.x) { glb.x= gid; } else
							{ if (gid > ghb.x) ghb.x= gid; }
						goto RasterX;
					}

					HY_corner  
					if (gid < glb.x) { glb.x= gid; } else
						{ if (gid > ghb.x) ghb.x= gid; }

					while ((int) v_wind-> y > gstep_y)
					{
						v_fol= v_wind, v_wind++, loopg--;
						if (!loopg) { v_wind= hold_y, loopg= gsi_y; }
					}

					HY_corner
					if (gid < glb.x) { glb.x= gid; } else
						{ if (gid > ghb.x) ghb.x= gid; }

					while ((int) v_wind-> y == gstep_y)
					{
						gid= v_wind-> x;
						if (gid < glb.x) { glb.x= gid; } else
							{ if (gid > ghb.x) ghb.x= gid; }
						v_fol= v_wind, v_wind++, loopg--;
						if (!loopg) { v_wind= hold_y, loopg= gsi_y; }
					}

					LY_corner
					if (gid < glb.x) { glb.x= gid; } else
						{ if (gid > ghb.x) ghb.x= gid; }

	RasterX:
					kv= (gstep_z *gszxy) +(gstep_y *gszx) +glb.x; 
					for (gstep_x= glb.x, cell_fill= cell_base +kv;
							(gstep_x <= ghb.x); gstep_x++, cell_fill++)
						{ (cell_fill-> count_large)++, ncel++; }
				}
			}
		}
	}

	*UseDyne= ncel;
}

void	rgrid_fill(GritP, DyneSpace, UseDyne)
	er_grid *GritP;
	er_face **DyneSpace;
	long *UseDyne;
{
	short
		loopg,
		loopi, 
		b_altface,
		gsi_z, gsi_y;
	shurt
		*sid_step;
	int
		gid, gframe,
		gstep_z, gstep_y, gstep_x,
		g_minz, g_miny;
	long
		loopjl,
		gszx, gszxy, 
		kv, ncel;
	realU
		dtmp, dhold,
		gmix;
	vekI
		glb, ghb;
	vekU
		scale_off, scale_mult,
		*hv_step;
	svector
		*scale_step, *scale_fill, *scale_follow,
		*vek_ref, *vek_lookup,
		*v_wind, *v_fol, *v_minz, *v_miny;
	er_cell *cell_base, *cell_fill;
	er_face *step_f;
	er_uface *step_altf;
	tr_object
		**content_step,
		*content_lookup;
	er_face 
		**dyne_assign;

#ifdef SAFE
	shurt
		check_ct;
#endif

/* /// assign fill pointers */
	ncel= (long) GritP->sx * GritP->sy * GritP->sz;
	for (dyne_assign= DyneSpace, cell_fill= GritP-> cells; (ncel--); cell_fill++)
	{
		if (cell_fill-> count_tiny || cell_fill-> count_large)
		{
			gid= (cell_fill-> count_tiny + cell_fill-> count_large);
			cell_fill-> things= dyne_assign, dyne_assign+= gid;
		}
		else cell_fill-> things= NULL;
	}

	scale_off= GritP-> vcbase, scale_mult= GritP-> vcscale;
	vek_ref= workspace -1;
	cell_base= GritP-> cells;
	gszx= GritP-> sx,
	gszxy= (long) GritP-> sx * GritP-> sy;
	ncel= 0;
	for (loopi= GritP-> cn, content_step= (tr_object **) GritP-> contain; (loopi--); content_step++)
	{
		content_lookup= *content_step;
		b_altface= 0;
		for (loopjl= content_lookup-> ta.vector_c,
				hv_step= content_lookup-> ta.vlist,
				scale_fill= workspace;
			(loopjl--); hv_step++, scale_fill++)
		{
			scale_fill-> x= scale_mult.x * (hv_step-> x -scale_off.x),
			scale_fill-> y= scale_mult.y * (hv_step-> y -scale_off.y),
			scale_fill-> z= scale_mult.z * (hv_step-> z -scale_off.z);
		}
		step_f= (er_face *) content_lookup-> ta.flist,
		step_altf= NULL;
		for (loopjl= content_lookup-> ta.poly_c; loopjl--; step_f++)
		{
			if (b_altface) { step_f= (er_face *) step_altf; }
			if (! step_f-> bits) continue ;

			gsi_z= step_f-> bits & 0x3ff;
			sid_step= step_f-> indic_sides, scale_fill= hold_z;  
			vek_lookup= vek_ref + *sid_step,
				*scale_fill= *vek_lookup,
				glb.x= ghb.x= vek_lookup-> x,
				glb.y= ghb.y= vek_lookup-> y,
				glb.z= ghb.z= vek_lookup-> z;
			for (loopg= gsi_z -1, sid_step++, scale_fill++; (loopg--); sid_step++, scale_fill++)
			{
				vek_lookup= vek_ref + *sid_step,
				gid= scale_fill-> x= vek_lookup-> x;
					if (gid < glb.x) { glb.x= gid; } else if (gid > ghb.x) { ghb.x= gid; }
				gid= scale_fill-> y= vek_lookup-> y;
					if (gid < glb.y) { glb.y= gid; } else if (gid > ghb.y) { ghb.y= gid; }
				gid= scale_fill-> z= vek_lookup-> z;
					if (gid < glb.z) { glb.z= gid; } else if (gid > ghb.z) { ghb.z= gid; }
			}
			if ((glb.x == ghb.x) && (glb.y == ghb.y) && (glb.z == ghb.z))
			{
				kv= glb.x +(glb.y *gszx) +(glb.z *gszxy);
				cell_fill= cell_base +kv;
#ifdef SAFE
				dyne_assign= cell_fill-> things;
				if (dyne_assign == NULL) trap_error();
				for (check_ct= cell_fill-> count_tiny; (*dyne_assign && check_ct--); dyne_assign++ );
				if (!check_ct) trap_error();
#else
				for (dyne_assign= cell_fill-> things; (*dyne_assign); dyne_assign++ );
#endif
				*dyne_assign= step_f, ncel++;
				continue ;
			}

/******* go to it ******/

			for (gstep_z= glb.z, v_minz= NULL; (gstep_z <= ghb.z); gstep_z++)
			{
				if (glb.z== ghb.z) 
				{
					gsi_y= gsi_z;
					for (loopg= gsi_y, scale_step= hold_z, scale_fill= hold_y; 
							(loopg--); scale_step++, scale_fill++ )
						{ scale_fill-> x= scale_step-> x, scale_fill-> y= scale_step-> y; }
					v_wind= NULL;
					goto RasterY;
				}

				gframe= gstep_z +1;  scale_fill= hold_y;
				if (NULL== v_minz)
				{
					for (loopg= gsi_z, v_minz= hold_z, v_fol= hold_z +gsi_z -1; (loopg); loopg--) 
					{
						if (v_minz-> z < gframe) break;
						v_fol= v_minz, v_minz++;
					}
					if (!loopg) { printf("panik (-2gz)\n");  break; }
					if ((loopg == gsi_z) && ((int) v_fol-> z == gstep_z))
					{
						for ( loopg= 0; (loopg < gsi_z) && ((int) v_fol-> z < gframe); loopg++) 
							{ v_minz= v_fol, v_fol--; }
					}
					g_minz= loopg;

					v_wind= v_minz;
/*cm1*/     HZ_corner	gsi_y= 1, glb.y= ghb.y= gid, scale_fill++;

					while (v_wind-> z < gframe)
					{
						gid= scale_fill-> y= v_wind-> y, scale_fill-> x= v_wind-> x;
						if (gid < glb.y) { glb.y= gid; } else
							{ if (gid > ghb.y) ghb.y= gid; }
						gsi_y++, scale_fill++;  v_fol= v_wind, v_wind++, loopg--;
						if (!loopg) { v_wind= hold_z, loopg= gsi_z; }
					}

/*cm2*/     HZ_corner	gsi_y++, scale_fill++;
					if (gid < glb.y) { glb.y= gid; } else
						{ if (gid > ghb.y) ghb.y= gid; }
					goto RasterY;
				}
				v_fol= v_minz, v_wind= v_minz +1, loopg= g_minz -1;
					if (!loopg) { v_wind= hold_z, loopg= gsi_z; }
				while (v_wind-> z < gstep_z)
				{
					v_fol= v_wind, v_wind++, loopg--;
					if (!loopg) { v_wind= hold_z, loopg= gsi_z; }
				}

/*cn1*/			LZ_corner
				gsi_y= 1, scale_follow= scale_fill, scale_fill++;
				gid= scale_follow-> y, glb.y= ghb.y= gid;

				while ((int) v_wind-> z== gstep_z)
				{
					gid= scale_fill-> y= v_wind-> y, scale_fill-> x= v_wind-> x;
					if (gid < glb.y) { glb.y= gid; } else
						{ if (gid > ghb.y) ghb.y= gid; }
					gsi_y++, scale_fill++;  v_fol= v_wind, v_wind++, loopg--;
					if (!loopg) { v_wind= hold_z, loopg= gsi_z; }
				}

/*cn5*/  if ((int) v_wind-> z < gstep_z)
				{
					LZ_corner  gsi_y++, scale_follow= scale_fill++;  gid= scale_follow-> y;
					if (gid < glb.y) { glb.y= gid; } else
						{ if (gid > ghb.y) ghb.y= gid; }
					goto RasterY;
				}

/*cn2*/			HZ_corner	gsi_y++, scale_fill++;
				if (gid < glb.y) { glb.y= gid; } else
					{ if (gid > ghb.y) ghb.y= gid; }

				while ((int) v_wind-> z > gstep_z)
				{
					v_fol= v_wind, v_wind++, loopg--;
					if (!loopg) { v_wind= hold_z, loopg= gsi_z; }
				}

/*cn3*/			HZ_corner	gsi_y++, scale_fill++;
				if (gid < glb.y) { glb.y= gid; } else
					{ if (gid > ghb.y) ghb.y= gid; }

				while ((int) v_wind-> z == gstep_z)
				{
					gid= scale_fill-> y= v_wind-> y, scale_fill-> x= v_wind-> x;
					if (gid < glb.y) { glb.y= gid; } else
						{ if (gid > ghb.y) ghb.y= gid; }
					gsi_y++, scale_fill++;  v_fol= v_wind, v_wind++, loopg--;
					if (!loopg) { v_wind= hold_z, loopg= gsi_z; }
				}

/*cn4*/			LZ_corner
				gsi_y++, scale_follow= scale_fill++;
				gid= scale_follow-> y;
				if (gid < glb.y) { glb.y= gid; } else
					{ if (gid > ghb.y) ghb.y= gid; }

	RasterY:
				for (gstep_y= glb.y, v_miny= NULL; (gstep_y <= ghb.y); gstep_y++)
				{
					if (glb.y== ghb.y)
					{
						loopg= gsi_y -1, scale_step= hold_y;
						gid= scale_step-> x, scale_step++;
						glb.x= ghb.x= gid;
						for (; loopg--; scale_step++)
						{
							gid= scale_step-> x;
							if (gid < glb.x) { glb.x= gid; } else
			{ if (gid > ghb.x) ghb.x= gid; }
						}
						v_wind= NULL;
						goto RasterX;
					}

					gframe= gstep_y +1;
					if (v_miny== NULL)
					{
						for (loopg= gsi_y, v_miny= hold_y, v_fol= hold_y +gsi_y -1; (loopg); loopg--) 
						{
							if (v_miny-> y < gframe) break;
							v_fol= v_miny, v_miny++;
						}
						if (!loopg) { printf("grid tiling panik (-2gy)\n");  break; }
						if ((loopg == gsi_y) && ((int) v_fol-> y== gstep_y))
						{
							for ( loopg= 0; (loopg < gsi_y) && (v_fol-> y < gframe); loopg++) 
								{ v_miny= v_fol, v_fol--; }
						}
						g_miny= loopg;

/*cm1*/		v_wind= v_miny;
						HY_corner  glb.x= ghb.x= gid;

						while (v_wind-> y < gframe)
						{
							gid= v_wind-> x;
							if (gid < glb.x) { glb.x= gid; } else
			{ if (gid > ghb.x) ghb.x= gid; }
							v_fol= v_wind, v_wind++, loopg--;
							if (!loopg) { v_wind= hold_y, loopg= gsi_y; }
						}

/*cm2*/		HY_corner
						if (gid < glb.x) { glb.x= gid; } else
							{ if (gid > ghb.x) ghb.x= gid; }

						goto RasterX;
					}

					v_fol= v_miny, v_wind= v_miny +1, loopg= g_miny -1;
						if (!loopg) { v_wind= hold_y, loopg= gsi_y; }
					while (v_wind-> y < gstep_y)
					{
						v_fol= v_wind, v_wind++, loopg--;
						if (!loopg) { v_wind= hold_y, loopg= gsi_y; }
					}

/*cy1*/				LY_corner  glb.x= ghb.x= gid;
					while ((int) v_wind-> y== gstep_y)
					{
						gid= v_wind-> x;
						if (gid < glb.x) { glb.x= gid; } else
							{ if (gid > ghb.x) ghb.x= gid; }
						v_fol= v_wind, v_wind++, loopg--;
						if (!loopg) { v_wind= hold_y, loopg= gsi_y; }
					}

/*cy5*/				if ((int) v_wind-> y < gstep_y)
					{
						LY_corner
						if (gid < glb.x) { glb.x= gid; } else
							{ if (gid > ghb.x) ghb.x= gid; }
						goto RasterX;
					}

					HY_corner  
					if (gid < glb.x) { glb.x= gid; } else
						{ if (gid > ghb.x) ghb.x= gid; }

					while ((int) v_wind-> y > gstep_y)
					{
						v_fol= v_wind, v_wind++, loopg--;
						if (!loopg) { v_wind= hold_y, loopg= gsi_y; }
					}

					HY_corner
					if (gid < glb.x) { glb.x= gid; } else
						{ if (gid > ghb.x) ghb.x= gid; }

					while ((int) v_wind-> y == gstep_y)
					{
						gid= v_wind-> x;
						if (gid < glb.x) { glb.x= gid; } else
							{ if (gid > ghb.x) ghb.x= gid; }
						v_fol= v_wind, v_wind++, loopg--;
						if (!loopg) { v_wind= hold_y, loopg= gsi_y; }
					}

					LY_corner
					if (gid < glb.x) { glb.x= gid; } else
						{ if (gid > ghb.x) ghb.x= gid; }

	RasterX:
					kv= (gstep_z *gszxy) +(gstep_y *gszx) +glb.x; 
					for (gstep_x= glb.x, cell_fill= cell_base +kv;
							(gstep_x <= ghb.x); gstep_x++, cell_fill++)
					{
#ifdef SAFE
						dyne_assign= cell_fill-> things +cell_fill-> count_tiny;
						if (dyne_assign == NULL) trap_error();
						for (check_ct= cell_fill-> count_large; (*dyne_assign && check_ct--); dyne_assign++ );
						if (!check_ct) trap_error();
#else
						for (dyne_assign= cell_fill-> things +cell_fill-> count_tiny; 
							(*dyne_assign); dyne_assign++ );
#endif
						*dyne_assign= step_f, ncel++;
					}
				}
			}
		}
	}

	*UseDyne= ncel;
}

