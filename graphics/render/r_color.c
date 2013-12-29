#include "rrun.h"
#include	"rlight.h"

#define	EXLIGHT	1

extern int		ct_lights,
				val_rdepth, val_gdepth;
extern shurt	ray_num;

#if !EXLIGHT
extern ev_light		*magic_light;
#endif

extern shurt		ray_rdepth, ray_gdepth;
extern ray_element	*current;

#if !EXLIGHT
extern colU		opaq_spf;
#endif

	/** r light */
extern void	cx_lightdark(er_surface *, colU *litfill, colU * dkfill, long *, void (vector *, cl_scope *)) ;
extern void	c_lightdark(er_surface *, colU *litfill, colU * dkfill, long *) ;
extern void	ci_light(er_surface *, colU *litfill, long *) ;

#if 0
static void	tdump(float u, float v, float r, float g, float b)
{
	FILE * f= fopen("texture.txt", "a");
	if (! f) return ;
	fprintf(f, "U: %g %g, C: %g %g %g\n", u, v, r, g, b);
	fclose(f);
}
#endif

extern void	cast_ray(), cast_subray(), glass_ray(),
		cv_texture();

#define RW_FRESNELL 1

void	c_color(void)
{
	int ia;
	unsigned long shash;
	er_surface
		*cur_surf;
	ev_surfbsi
		*cur_surfplan;

	double
		dtmp, djunk,
		dsroot, fres1, fres2;
	vekU vrd, vnm, vwk;
	colU
		col, lite, dark,
		ceepr, ceepg;

	ray_element
		*cur_relement,
		sub_spawn;

	cur_relement= current,
	shash= (lung) cur_relement-> target;

#if	SPHERE_SUPPORT
		/*** /-/ texture mapp */
	if (shash)
	{
		cur_surfplan= cur_relement-> target-> shading,
		ia= cur_relement-> target-> bits & RACE_TEXTURE;
	}
	else	... ignore spheres
	{
		shash= (lung) cur_relement-> sarget;
		cur_surfplan= cur_relement-> sarget-> shading, ia= 0;
	}
#else
	cur_surfplan= cur_relement-> target-> shading,
	ia= cur_relement-> target-> bits & RACE_TEXTURE;
#endif

	if (cur_surfplan->shader)
	{
		(* cur_surfplan-> shader)() ;
		return ;
	}

	if (cur_surfplan-> mapct)
	{
		cur_surf= cur_surfplan-> surfmix;
		cv_texture(cur_surfplan, cur_surf, ia, &shash);
	}
		else cur_surf= cur_surfplan-> surfp;

	vrd= cur_relement-> space_d,
	vnm= cur_relement-> hit_n;

	ia= cur_surf-> attrib;
	if (ia & ER_LUMEN) { C_cpy(col, cur_surf-> col_l) } else C_zero(col);
	if (ia & ER_AMBIENT) { C_add(col, cur_surf-> col_a) }
	cur_relement-> result_a= (ia & ER_ALPHA) ? cur_surf-> alpha : 1.0;

	if (ia & ER_TDS)
	{
		ceepg= cur_surf-> col_t,
		ceepr= cur_surf-> col_r;
	}

	if (ia & ERS_ALITE)
	{
		c_lightdark(cur_surf, &lite, & dark, &shash);
		C_add(col, lite)

		dtmp= 0.3 * col.r + 0.59 * col.g + 0.11 * col.b;
		djunk= 0.3 * dark.r + 0.59 * dark.g + 0.11 * dark.b;
		dsroot= dtmp + djunk;
		if (dsroot < 1e-10) { fres1= 0.0; }
			else { fres1= dtmp * cur_surf-> alpha / dsroot; }
		if (fres1 < 0.0) fres1= 0.0; else if (fres1 > 1.0) fres1= 1.0;

		cur_relement-> result_a= 1.0 - fres1;
		cur_relement-> result_b= 0.0,
		cur_relement-> result_c= 0.0,
		cur_relement-> result_d= 0.0;
		cur_relement-> hashing_code= shash, current= cur_relement;
		return;
	}

	if (ia & ER_MDS)
	{
		if (cur_surfplan->lighting) cx_lightdark(cur_surf, &lite, & dark, &shash, cur_surfplan->lighting) ;
			else c_lightdark(cur_surf, &lite, & dark, &shash);
		C_add(col, lite)
	}

	if ((ia & ER_GLASS) && (ray_gdepth < val_gdepth))
	{
		M_cpy(vwk, cur_surf-> rindx * vrd)
		dtmp= M_dot(vrd, vnm);
		if (dtmp > 1e-8) goto cSendColor;
		djunk= (cur_surf-> sqindx + dtmp * dtmp -1.0);
		if (djunk < 1e-8)
		{
			/*** critical hit */
			dtmp= -2.0 * dtmp;
			M_add(vwk, vrd, dtmp * vnm)
			ray_gdepth++;
			current= &sub_spawn, current-> hashing_code= 0, current-> space_d= vwk;
			M_add(current-> space_p, cur_relement-> hit_p, 1e-5 * vnm)
			cast_subray();
			if (current-> hashing_code)
			{
				shash^= current-> hashing_code;
				col.r = current-> result_b,
					col.g = current-> result_c,
					col.b = current-> result_d;
			}
			else
				{ current-> result_a= current-> result_b= current-> result_c= current-> result_d= 0.0; }
			ray_gdepth--;
			goto cSendColor;
		}

		djunk= -cur_surf-> rindx * (dtmp +(dsroot= sqrt(djunk)));
		M_plus(vwk, djunk * vnm);
#if RW_FRESNELL
		dsroot*= -dtmp * cur_surf-> indx;
		djunk= cur_surf-> in_t1 * (dtmp * dtmp) + 4 * dsroot +cur_surf-> in_t2;
		fres1= 4 * dsroot, fres2= djunk -fres1;
		if (djunk > 1e-8) { fres1/= djunk, fres2/= djunk; }
		fres1= cur_surf-> ut * fres1 + cur_surf-> uit,
		fres2= cur_surf-> ut * fres2 + cur_surf-> uit;
#else
		fres1= fres2= 1.0;
#endif

		ray_gdepth++;
		current= &sub_spawn, current-> hashing_code= 0, current-> space_d= vwk;
		M_add(current-> space_p, cur_relement-> hit_p, -1e-5 * vnm)
		glass_ray();
		if (current-> hashing_code)
		{
			shash^= current-> hashing_code;
			col.r+= current-> result_b * fres1 * ceepg.r,
				col.g+= current-> result_c * fres1 * ceepg.g,
				col.b+= current-> result_d * fres1 * ceepg.b;
		}
		ray_gdepth--;

		if ((ia & ER_REFLECT) && (ray_rdepth < val_rdepth))
		{
			dtmp= -2.0 * M_dot(vrd, vnm);
			if (dtmp < 0.0)
			{
				if (ia & ER_ENVIR)
				{
					col.r += ceepr.r * cur_surf-> col_i.r,
						col.g += ceepr.g * cur_surf-> col_i.g,
						col.b += ceepr.b * cur_surf-> col_i.b;
				}
				goto cSendColor;
			}
			ray_rdepth++;
			M_add(vwk, vrd, dtmp * vnm)
			current= &sub_spawn, current-> hashing_code= 0, current-> space_d= vwk;
			M_add(current-> space_p, cur_relement-> hit_p, 1e-5 * vnm)

			cast_subray();
			if (current-> hashing_code)
			{
				shash^= current-> hashing_code;
				col.r += current-> result_b * fres2 * ceepr.r,
					col.g += current-> result_c * fres2 * ceepr.g,
					col.b += current-> result_d * fres2 * ceepr.b;

				if ((current-> result_a < 0.999) && (ia & ER_ENVIR))
				{
					dtmp= 1.0 - current-> result_a;
					col.r += dtmp * ceepr.r * cur_surf-> col_i.r,
						col.g += dtmp * ceepr.g * cur_surf-> col_i.g,
						col.b += dtmp * ceepr.b * cur_surf-> col_i.b;
				}
			}
			else
			{
				if (ia & ER_ENVIR)
				{
					col.r += ceepr.r * cur_surf-> col_i.r,
						col.g += ceepr.g * cur_surf-> col_i.g,
						col.b += ceepr.b * cur_surf-> col_i.b;
				}
			}
			ray_rdepth--;
		}
	}
	else
	{ if (ia & (ER_REFLECT | ER_VAPOR))
		{
			fres1= cur_surf-> alpha;
			if ((ia & ER_REFLECT) && (ray_rdepth < val_rdepth))
			{
				dtmp= -2.0 * M_dot(vrd, vnm);
				if (dtmp < 1e-10) goto cSendColor;
				M_add(vwk, vrd, dtmp * vnm)
				ray_rdepth++;
				current= &sub_spawn, current-> hashing_code= 0, current-> space_d= vwk;
				M_add(current-> space_p, cur_relement-> hit_p, 1e-5 * vnm)
				cur_relement-> target-> ray_count= ray_num +1;

				cast_subray();
				if (current-> hashing_code)
				{
					shash^= current-> hashing_code;
					col.r += current-> result_b * ceepr.r,
						col.g += current-> result_c * ceepr.g,
						col.b += current-> result_d * ceepr.b;
					if ((current-> result_a < 0.999) && (ia & ER_ENVIR))
					{
						dtmp= 1.0 - current-> result_a;
						col.r += dtmp * ceepr.r * cur_surf-> col_i.r,
							col.g += dtmp * ceepr.g * cur_surf-> col_i.g,
							col.b += dtmp * ceepr.b * cur_surf-> col_i.b;
					}
				}
				else
				{
					if (ia & ER_ENVIR)
					{
						col.r += ceepr.r * cur_surf-> col_i.r,
							col.g += ceepr.g * cur_surf-> col_i.g,
							col.b += ceepr.b * cur_surf-> col_i.b;
					}
				}
				ray_rdepth--;
			}
			if ((ia & ER_VAPOR) && (ray_gdepth < val_gdepth))
			{
				ray_gdepth++;
				current= &sub_spawn, current-> hashing_code= 0, current-> space_d= vrd;
				M_add(current-> space_p, cur_relement-> hit_p, -1e-5 * vnm)

				cast_ray();
				if (current-> hashing_code)
				{
					shash^= current-> hashing_code;
					col.r += current-> result_b * ceepg.r,
						col.g += current-> result_c * ceepg.g,
						col.b += current-> result_d * ceepg.b;
/*					if (ia & ER_ALVAP)
					{
						dtmp= 1.0 - (fres1 * (1.0 -current-> result_a)),
						cur_relement-> result_a= dtmp;
					}
					else
					{
*/
						dtmp= 0.333333333333 * (ceepg.r + ceepg.g + ceepg.b);
						cur_relement-> result_a= dtmp * (current-> result_a -1.0) +1.0;
//					}
				}
				else
				{
/*					if (ia & ER_ALVAP) { cur_relement-> result_a= 1.0 -fres1; }
					else
					{
*/
						dtmp= 0.333333333333 * (ceepg.r + ceepg.g + ceepg.b);
						cur_relement-> result_a= 1.0 -dtmp;
//					}
				}
				
				ray_gdepth--;
			}
	} }

cSendColor:
	cur_relement-> result_b= col.r, cur_relement-> result_c= col.g, cur_relement-> result_d= col.b;
	cur_relement-> hashing_code= shash, current= cur_relement;

}

	/*** internal color */
void	ci_color()
{
	int ia;
	unsigned long shash;
	double dtmp, djunk, fres;
	vekU vrd, vnm, vlnm, vwk;
	colU
		col, lite, dk,
		ceepg, ceepr;
	er_surface *cur_surf;
	ev_surfbsi *cur_surfplan;
	ray_element
		*cur_relement,
		sub_spawn;

	cur_relement= current,
	shash= (lung) cur_relement-> target;

	/*** /// texture mapp */
	if (shash)
	{
		cur_surfplan= cur_relement-> target-> shading,
		ia= cur_relement-> target-> bits & RACE_TEXTURE;
	}
	else
	{
		shash= (lung) cur_relement-> sarget;
		cur_surfplan= cur_relement-> sarget-> shading, ia= 0;
	}
	if (cur_surfplan-> mapct)
	{
		cur_surf= cur_surfplan-> surfmix;
		cv_texture(cur_surfplan, cur_surf, ia, &shash);
	}
	else cur_surf= cur_surfplan-> surfp;

	vrd= cur_relement-> space_d,
	vnm= cur_relement-> hit_n, vlnm= vnm;

	cur_relement-> result_a= 1.0;
	ia= cur_surf-> attrib;
	if (ia & ER_LUMEN) { C_cpy(col, cur_surf-> col_l) } else C_zero(col);
	if (ia & ER_AMBIENT)
	{
		col.r += cur_surf-> col_a.r,
		col.g += cur_surf-> col_a.g,
		col.b += cur_surf-> col_a.b;
	}

	if (ia & ER_TDS)
	{
		ceepg= cur_surf-> col_t,
		ceepr= cur_surf-> col_r;
	}
	if (!(ia & ER_GLASS))
		{ M_cpy(vlnm, -vnm) }

	if (!(ia & ER_MDS)) goto ciEffectsColor;

ciEffectsColor:

	if (ia & ER_GLASS)
	{
		M_cpy(vwk, cur_surf-> indx * vrd)
		dtmp= M_dot(vrd, vnm);
		if (dtmp < 1e-8) goto ciSendColor;
		djunk= (cur_surf-> rqindx + dtmp * dtmp -1.0);
		if (djunk < 1e-8)
		{
			if (ray_gdepth > val_gdepth) goto ciSendColor;

			/*** critical hit */
			dtmp= -2.0 * dtmp;
			M_add(vwk, vrd, dtmp * vnm)
			ray_gdepth++;
			current= &sub_spawn, current-> hashing_code= 0, current-> space_d= vwk;
			M_add(current-> space_p, cur_relement-> hit_p, -1e-5 * vnm)
			glass_ray();
			if (current-> hashing_code)
			{
				shash^= current-> hashing_code;
				col.r = current-> result_b,
					col.g = current-> result_c,
					col.b = current-> result_d;
			}
			else
				{ current-> result_a= current-> result_b= current-> result_c= current-> result_d= 0.0; }
			ray_gdepth--;
			goto ciSendColor;
		}

		djunk= cur_surf-> indx * (-dtmp +sqrt(djunk));
		M_plus(vwk, djunk * vnm);

		current= &sub_spawn, current-> hashing_code= 0, current-> space_d= vwk;
		M_add(current-> space_p, cur_relement-> hit_p, 1e-5 * vnm)
		cast_ray();
		if (current-> hashing_code)
		{
			shash^= current-> hashing_code;
			col.r+= current-> result_b * ceepg.r,
				col.g+= current-> result_c * ceepg.g,
				col.b+= current-> result_d * ceepg.b;
		}

		if (ia & ER_MDS)
		{
			current= cur_relement;
			ci_light(cur_surf, &lite, &shash);
			C_add(col, lite)
		}
		if ((ia & ER_REFLECT) && (ray_rdepth < val_rdepth))
		{
			dtmp= -2.0 * M_dot(vrd, vnm);
			if (dtmp > -1e-10) goto ciSendColor;
			M_add(vwk, vrd, dtmp * vnm)
			ray_rdepth++;
			current= &sub_spawn, current-> hashing_code= 0, current-> space_d= vwk;
			M_add(current-> space_p, cur_relement-> hit_p, -1e-5 * vnm)
	
			glass_ray();
			if (current-> hashing_code)
			{
				shash^= current-> hashing_code;
				col.r += current-> result_b * ceepr.r,
					col.g += current-> result_c * ceepr.g,
					col.b += current-> result_d * ceepr.b;
			}
			ray_rdepth--;
		}
	}
	else
	{
		if (ia & ER_MDS)
		{
			current= cur_relement;
			c_lightdark(cur_surf, &lite, & dk, &shash);
			C_add(col, lite)
		}

		if (ia & ER_VAPOR)
		{
			fres= cur_surf-> alpha;
			current= &sub_spawn, current-> hashing_code= 0, current-> space_d= vrd;
			M_add(current-> space_p, cur_relement-> hit_p, 1e-5 * vnm)
			cast_subray();
			if (current-> hashing_code)
			{
				shash^= current-> hashing_code;
				col.r+= current-> result_b * ceepg.r,
					col.g+= current-> result_c * ceepg.g,
					col.b+= current-> result_d * ceepg.b;
/*				if (ia & ER_ALVAP)
				{
					dtmp= 1.0 - (fres * (1.0 -current-> result_a)),
					cur_relement-> result_a= dtmp;
				}
*/
			}
//			else if (ia & ER_ALVAP) { cur_relement-> result_a= 1.0 -fres; }
		}
		if ((ia & ER_REFLECT) && (ray_rdepth < val_rdepth))
		{
			dtmp= -2.0 * M_dot(vrd, vnm);
			if (dtmp > -1e-10) goto ciSendColor;
			M_add(vwk, vrd, dtmp * vnm)
			ray_rdepth++;
			current= &sub_spawn, current-> hashing_code= 0, current-> space_d= vwk;
			M_add(current-> space_p, cur_relement-> hit_p, -1e-5 * vnm)
	
			cast_subray();
			if (current-> hashing_code)
			{
				shash^= current-> hashing_code;
				col.r += current-> result_b * ceepr.r,
					col.g += current-> result_c * ceepr.g,
					col.b += current-> result_d * ceepr.b;
				}
			ray_rdepth--;
		}
	}
ciSendColor:
	cur_relement-> result_b= col.r, cur_relement-> result_c= col.g, cur_relement-> result_d= col.b;
	cur_relement-> hashing_code= shash, current= cur_relement;

}
