#include "rrun.h"
#include	"rlight.h"

extern ray_element	*current;

	// danger, if er_surface doesn't have specular set this won't work.

	/* data block:
		v1 : dark edge
		v2 : light edge
		v3 : 1 / (edge width)
	*/

void	rl_liquid(vector * lightdir, cl_scope * scope)
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

		/** do liquid highlight */
	dtmp *= 2.0;  M_sub(vwk, dtmp * scope-> skinnorm, vlr)
	dtnp= -M_dot(scope-> viewdir, vwk);
	if (dtnp < 0.0) return ;
	dtmp= dtnp;
	if (dtmp > scope-> skinsurf-> v2)
	{
		if (dtmp > scope-> skinsurf-> v1) 
		{
			scope-> lightshade.r += scope-> props.r * scope-> lit.r,
			scope-> lightshade.g += scope-> props.g * scope-> lit.g,
			scope-> lightshade.b += scope-> props.b * scope-> lit.b;
		}
		else
		{
			dtnp = scope-> skinsurf-> v3 * (dtmp -scope-> skinsurf-> v2), dtmp *= dtnp;
			scope-> lightshade.r += dtmp * scope-> props.r * scope-> lit.r,
			scope-> lightshade.g += dtmp * scope-> props.g * scope-> lit.g,
			scope-> lightshade.b += dtmp * scope-> props.b * scope-> lit.b;
		}
	}
}