#include "rrun.h"
#include	"rlight.h"

extern int			ct_lights, val_rdepth, val_gdepth;
extern ev_light		*magic_light;
extern shurt	ray_num;

extern ray_element	*current;

// extern void	rl_liquid(vector *lightdir, cl_scope * scope) ;
// extern void rl_posterize(vector *lightdir, cl_scope * scope) ;


extern colU		opaq_spf;

#define RW_FRESNELL 1
#define	L_SHADOWSTEP	1e-5

static void	cl_dark(vector * lightdir, cl_scope * scope, double density)
{
	double dtmp;
	vekU vlr;

	if (!(scope-> isurfattr & ER_DIFFUSE)) return ;

	vlr= *lightdir;
	dtmp= M_dot(vlr, scope-> skinnorm);
	if (dtmp < 0.0) return;
	dtmp *= density;

	scope-> darkshade.r += dtmp * scope-> propd.r * scope-> lit.r,
		scope-> darkshade.g += dtmp * scope-> propd.g * scope-> lit.g,
		scope-> darkshade.b += dtmp * scope-> propd.b * scope-> lit.b;
}

static void	cl_shade(vector *lightdir, cl_scope * scope)
{
	double dtmp, dtnp;
	vekU vlr, vwk;

	vlr= *lightdir;
	dtmp= M_dot(vlr, scope-> skinnorm);
	if (dtmp < 0.0) return;
	if (scope-> isurfattr & ER_DIFFUSE)
	{
		scope-> lightshade.r += dtmp * scope-> propd.r * scope-> lit.r,
			scope-> lightshade.g += dtmp * scope-> propd.g * scope-> lit.g,
			scope-> lightshade.b += dtmp * scope-> propd.b * scope-> lit.b;
	}
	if (scope-> isurfattr & ER_SPECULAR)
	{
		dtmp *= 2.0;  M_sub(vwk, dtmp * scope-> skinnorm, vlr)
		dtnp= -M_dot(scope-> viewdir, vwk);
		if (dtnp < 0.0) return;
/*		if (isurfattr & ER_DUALSPEC)
		{
			dtmp= (skinsurf-> dualmix0 * pow(dtnp, skinsurf-> spec))
				+ (skinsurf-> dualmix1 * pow(dtnp, skinsurf-> dualspec));
		} else
*/
		dtmp= pow(dtnp, scope-> skinsurf-> spec);
		scope-> lightshade.r += dtmp * scope-> props.r * scope-> lit.r,
		scope-> lightshade.g += dtmp * scope-> props.g * scope-> lit.g,
		scope-> lightshade.b += dtmp * scope-> props.b * scope-> lit.b;
	}
}

#define	NE(zzz)	(zzz < 1e-4)


#define	cl_EFALL	if (ilb & ERLA_FALL) { dtmp= 0.0; \
	if (ilb & 0x40) dtmp+= light_step-> falloff.x * djunk * djunk; \
	if (ilb & 0x20) dtmp+= light_step-> falloff.y * djunk; \
	if (ilb & 0x10) dtmp+= light_step-> falloff.z; \
	if (dtmp > 1.0) { if (dtmp > 1e5) continue; dtmp= 1.0 / dtmp, lo.lit.r*= dtmp, lo.lit.g*= dtmp, lo.lit.b*= dtmp; } }

void	cx_lightdark(er_surface *lsurf, colU *litfill, colU * dkfill, long *lhash, void lightfunc(vector *, cl_scope *))
{
	int istep;
	long ilt, ilb, lighthash;
	double
		dtmp, djunk,
		dspotx, dspoty, mu, mv,
		c1, c2, c3;
	vector skinp, vlr;
	ev_smap *lmappix;
	ev_lightmaptable *lmapt;
	ev_light *light_step;
	ray_element
		*rcolorhold, rayshadow;
	cl_scope lo ;

	lo.isurfattr= lsurf-> attrib,
	rcolorhold= current;
	skinp= rcolorhold-> hit_p,
	lo.skinnorm= rcolorhold-> hit_n;
	lo.skinsurf= lsurf;

	if (lo.isurfattr & ER_DIFFUSE) { lo.propd= lsurf-> col_d; } else { C_zero(lo.propd) }
	if (lo.isurfattr & ER_SPECULAR)
	{
		lo.viewdir= rcolorhold-> space_d;
		lo.props= lsurf-> col_s;
	}
		else { C_zero(lo.props) }

	C_zero(lo.lightshade)
	C_zero(lo.darkshade)
	lighthash= 0;
	if (NE(lo.propd.r) && NE(lo.propd.g) && NE(lo.propd.b) && NE(lo.props.r) && NE(lo.props.g) && NE(lo.props.b))
		{ *litfill= lo.lightshade;  * dkfill= lo.darkshade;  return; }

	for (istep= ct_lights, light_step= magic_light; (istep--); light_step++)
	{
		ilt= light_step-> tipe, ilb= light_step-> bits;
		lo.lit= light_step-> color;

		if (ilt == ERL_POINT)
		{
			/***
				/// point light
				/// light direction calc
				/// shadow test
				/// point near/far
				/// distance falloff
				*/

			M_sub(vlr, light_step-> pt, skinp)
			djunk= sqrt(M_selfdot(vlr));
			if ((djunk < 1e-18) || ( M_dot(vlr, lo.skinnorm) < 1e-18)) continue;
				
			if (ilb & ERLA_NEAR)
				{ if (djunk > light_step-> falloff.z) continue; }
				else { cl_EFALL }
			dtmp= 1.0 / djunk,  M_scale(vlr, dtmp)
			if (ilb & ERLA_SHAD)
			{
				current= &rayshadow;
				current-> dist= djunk,
				M_add(current-> space_p, rcolorhold-> hit_p, L_SHADOWSTEP * rcolorhold-> hit_n)
				current-> space_d= vlr,
				current-> sarget= rcolorhold-> sarget,
				current-> target= rcolorhold-> target;
				if (crit_ray(light_step, 1e-12))
				{
					cl_dark(& vlr, & lo, 1.0);
					continue;
				}
				C_mult(lo.lit, opaq_spf)
				lighthash ^= (long) light_step;
			}
			if (ilb & ERLA_NEAR)
			{
				if (djunk > light_step-> falloff.x)
				{
					dtmp= light_step-> falloff.y * (light_step-> falloff.z -djunk);
					lo.lit.r*= dtmp, lo.lit.g*= dtmp, lo.lit.b*= dtmp;
				}
			}

			(* lightfunc)( &vlr, & lo);
			continue;
		}

		if (ilt == ERL_SPOT)
		{
			M_sub(vlr, light_step-> pt, skinp)

			if (ilb & ERLA_PROJ)
			{
				lmapt= light_step-> lprojmap,
				lmappix= lmapt-> maplist-> pixmapref;
				dtmp= -M_dot(vlr, light_step-> direc);
				if ((dtmp < 1e-10) || (M_dot(vlr, lo.skinnorm) < 1e-10)) continue;
				dspotx= -M_dot(vlr, light_step-> lunge),
				dspoty= -M_dot(vlr, light_step-> lup);
				djunk= 1.0 / (dtmp * light_step-> cone_scale);
				mu= lmapt-> pscalex * dspotx * djunk * light_step-> cone_edge + lmapt-> plat,
				mv= lmapt-> pscaley * dspoty * djunk * light_step-> cone_edge + lmapt-> plng;
				if ((mu < 0.0) || (mv < 0.0) || (mu > 1.0) || (mv > 1.0)) continue;

				texc_lookup(mu, mv, lmappix, lmappix-> arrayc, &c1, &c2, &c3, lhash);
				lo.lit.r *= c1, lo.lit.g *= c2, lo.lit.b *= c3;
				if (NE(lo.lit.r) && NE(lo.lit.g) && NE(lo.lit.b)) continue;

				djunk= sqrt(M_selfdot(vlr));
				dtmp= 1.0 / djunk,  M_scale(vlr, dtmp)
				cl_EFALL
				if (ilb & ERLA_SHAD)
				{
					current= &rayshadow;
					current-> dist= djunk,
					M_add(current-> space_p, rcolorhold-> hit_p, L_SHADOWSTEP * rcolorhold-> hit_n)
					current-> space_d= vlr,
					current-> sarget= rcolorhold-> sarget,
					current-> target= rcolorhold-> target;
					if (crit_ray(light_step, 1e-12))
					{
						cl_dark(& vlr, & lo, 1.0);
						continue;
					}
					C_mult(lo.lit, opaq_spf)
					lighthash ^= (long) light_step;
				}

				(* lightfunc)( &vlr, & lo);
				continue;
			}

			if (ilb & ERLA_SOFT)
			{
				cl_dark(& vlr, & lo, 1.0);
				continue;
			}

			/***
				/// spot light
				/// cone edge test
				/// shadow test
				/// cone edge falloff
				/// distance falloff
				*/

			djunk= sqrt(M_selfdot(vlr));
			if ((djunk < 1e-10) || ( M_dot(vlr, lo.skinnorm) < 1e-10) ||
				( -M_dot(vlr, light_step-> direc) < ( djunk * light_step-> cone_edge))) continue ;
			dtmp= 1.0 / djunk,  M_scale(vlr, dtmp)

			cl_EFALL
			if (ilb & ERLA_SHAD)
			{
				current= &rayshadow;
				current-> dist= djunk,
				M_add(current-> space_p, rcolorhold-> hit_p, L_SHADOWSTEP * rcolorhold-> hit_n)
				current-> space_d= vlr,
				current-> sarget= rcolorhold-> sarget,
				current-> target= rcolorhold-> target;
				if (crit_ray(light_step, 1e-12))
				{
					cl_dark(& vlr, & lo, 1.0);
					continue;
				}
				C_mult(lo.lit, opaq_spf)
				lighthash ^= (long) light_step;
			}
			else
			if (ilb & ERLA_BUFFER)
			{
				current= rcolorhold;
				if (c_depth(light_step))
				{
					cl_dark(& vlr, & lo, 1.0);
					continue;
				}

				dtmp= 0.3 * opaq_spf.r + 0.59 * opaq_spf.g + 0.11 * opaq_spf.b;
				cl_dark(& vlr, & lo, 1.0 - dtmp);
				C_mult(lo.lit, opaq_spf)
			}

			if (ilb & ERLA_CONE)
			{
				dtmp= -M_dot(vlr, light_step-> direc);
				if (dtmp < light_step-> cone_bound)
				{
					djunk= light_step-> cone_scale * (dtmp - light_step-> cone_edge);
					lo.lit.r*= djunk, lo.lit.g*= djunk, lo.lit.b*= djunk;
				}
			}
			else
				if (ilb & ERLA_POWER)
				{
					dtmp= -M_dot(vlr, light_step-> direc);
					djunk= pow(dtmp, light_step-> cone_scale);
					lo.lit.r*= djunk, lo.lit.g*= djunk, lo.lit.b*= djunk;
				}

			(* lightfunc)( &vlr, & lo);
			continue;
		}

		if (ilt == ERL_BEAM)
		{
			/***
				/// directional light
				/// shadow test
				*/
			M_cpy(vlr, light_step-> direc)

			if (ilb & ERLA_SHAD)
			{
				current= &rayshadow;
				current-> dist= 1e301,

				M_add(current-> space_p, rcolorhold-> hit_p, L_SHADOWSTEP * rcolorhold-> hit_n)
				M_plus(current-> space_p, - 1e-4 * rcolorhold-> space_d)

				current-> space_d= vlr,
				current-> sarget= rcolorhold-> sarget,
				current-> target= rcolorhold-> target;

				dtmp= 0.5 * (1.0 - M_dot(rcolorhold-> target-> face_normal, vlr)) + 0.5;
				dtmp*= light_step-> safedist;
				if (crit_ray(light_step, dtmp))
				{
					cl_dark(& vlr, & lo, 1.0);
					continue;
				}

				C_mult(lo.lit, opaq_spf)
				lighthash ^= (long) light_step;
			}
			else
			if (ilb & ERLA_BUFFER)
			{
				current= rcolorhold;
				if (c_depth(light_step)) 
				{
					cl_dark(& vlr, & lo, 1.0);
					continue;
				}
				else
					if (opaq_spf.r < 1.0) cl_dark(& vlr, & lo, 1.0 - opaq_spf.r);

				C_mult(lo.lit, opaq_spf)
			}

			(* lightfunc)( &vlr, & lo);
			continue;
		}
	}

	*litfill= lo.lightshade, *dkfill= lo.darkshade, *lhash ^= lighthash;
}

void	c_lightdark(er_surface *lsurf, colU *litfill, colU * dkfill, long *lhash)
{
	cx_lightdark(lsurf, litfill, dkfill, lhash, cl_shade) ;
}

void	ci_light(er_surface *lsurf, colU *litfill, long *lhash)
{
	ray_element rayflip;
	colU darkHack;

	rayflip= *current;
	M_neg(rayflip.hit_n)
	current= &rayflip;
	cx_lightdark(lsurf, litfill, & darkHack, lhash, cl_shade);		
}

