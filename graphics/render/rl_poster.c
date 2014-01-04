#include "rrun.h"
#include	"rlight.h"

extern ray_element	*current;

void	rl_posterize(vector * lightdir, cl_scope * scope) ;

	// danger, if er_surface doesn't have diffuse & specular set this won't work.

	/* data block;
		v1 : diffuse border
		v2 : specular border
		v3 : spread
		v4 : 1 / (2 * spread)
	*/

void	rc_posterize(void)
{
	unsigned long shash;
	er_surface
		*cur_surf;
	ev_surfbsi
		*cur_surfplan;
	double dtmp,
		kv, kx, ij, ji ;
	colU
		col, cfinal,
		cbright, cscratch ;
	ray_element *cur= current ;

	cur_surfplan= cur-> target-> shading ;

	if (cur_surfplan-> mapct)
	{
		cur_surf= cur_surfplan-> surfmix;
		cv_texture(cur_surfplan, cur_surf, current-> target-> bits & RACE_TEXTURE, &shash);
	}
		else cur_surf= cur_surfplan-> surfp;

	cx_lightdark(cur_surf, &cbright, & cscratch, &shash, rl_posterize) ;

	kx= cur_surf-> v3 ;
	current-> result_a= 1.0 ;

		// test for extremes first
	dtmp= cbright.b - cur_surf-> v2 ;
	if (dtmp < kx)
	{
		kv= cbright.g - cur_surf-> v1 ;
		if (kv < -kx) { C_cpy(col, cur_surf-> col_a) }
		else if (kv < kx)
		{
				// diffuse | ambient border
			ij= cur_surf-> v4 * (kv + kx), ji= 1.0 - ij ;
			C_plus(col, ij * cur_surf-> col_d,  ji * cur_surf-> col_a) ;
		}
			else { C_cpy(col, cur_surf-> col_d) }

		if (dtmp > -kx)
		{
			ij= cur_surf-> v4 * (dtmp + kx), ji= 1.0 - ij ;
			C_plus(cfinal, ij * cur_surf-> col_s, ji * col) ;
		}
			else { C_cpy(cfinal, col) }
	}
	else
	{
			// it is hot enough for specular
		C_cpy(cfinal, cur_surf-> col_s) ;
	}

	current= cur ;
	cur-> result_b= cfinal.r ,
	cur-> result_c= cfinal.g ,
	cur-> result_d= cfinal.b ;
}

void	rl_posterize(vector * lightdir, cl_scope * scope)
{
	double dtmp, dtnp ;
	vector vlr, vwk;
	colU  cval ;

	vlr= *lightdir;
	dtmp= M_dot(vlr, scope-> skinnorm);
	if (dtmp < 0.0) return ;

	C_cpy(cval, dtmp * scope-> lit) ;
	scope-> lightshade.g += C_bright(cval) ;

	if (scope-> isurfattr & ER_SPECULAR)
	{
		dtmp *= 2.0;  M_sub(vwk, dtmp * scope-> skinnorm, vlr)
		dtnp= -M_dot(scope-> viewdir, vwk);
		if (dtnp > 0.0)
		{
			dtmp= pow(dtnp, scope-> skinsurf-> spec);
			C_cpy(cval, dtmp * scope-> lit) ;
			scope-> lightshade.b += C_bright(cval) ;
		}
	}
}
