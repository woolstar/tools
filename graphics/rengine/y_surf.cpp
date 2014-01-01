#include "ye.h"

extern "C"
{
		// cp map
	extern ev_smap *cpmap_lookup(const char *, int);

		// rl liquid
	extern void	rl_liquid(vector * lightdir, void * scope) ;
		// rl poster
	extern void	rc_posterize(void) ;
	extern void	rl_posterize(vector * lightdir, void * scope) ;

		// y setup
	extern color	c_ambient ;
} ;

void	applybase(er_surface * ptr, scolor & abase)
{
	ptr-> attrib= ER_AMBIENT | ER_DIFFUSE ;

	C_blend(ptr-> col_a, 0.5 * abase, c_ambient) ;
	C_cpy(ptr-> col_d, 0.75 * abase) ;
	ptr-> alpha= 1.0 ;
	ptr-> bend= 1.0 ;
}

void	applysurf(er_surface * ptr, aart::surface * asrc, ev_surfbsi * basisptr )
{
	long	ka, kb, kn ;
	double dtmp, dval, c2 ;

	ka= asrc-> muse ;
	kb= asrc-> clink ;
	kn= 0 ;

	ptr-> alpha= asrc-> alpha ;
	if (ka & aart::surface::eAmbient)
	{
		kn |= ER_AMBIENT ;
		C_blend(ptr-> col_a, asrc-> ambient, c_ambient) ;
	}
	if (ka & aart::surface::eGlow)
	{
		kn |= ER_LUMEN ;
		C_cpy(ptr-> col_l, asrc-> glow) ;
	}
	if (ka & aart::surface::eDiffuse)
	{
		kn |= ER_DIFFUSE ;
		C_cpy(ptr-> col_d, asrc-> diffuse) ;
	}
	if (ka & aart::surface::eSpecular)
	{
		kn |= ER_SPECULAR ;
		C_cpy(ptr-> col_s, asrc-> specular) ;
		dtmp= 1.0 - asrc-> gloss ;
		if (dtmp < 0.001) dtmp= 0.001 ;
		ptr-> spec= 1.0 / (dtmp * dtmp) ;
	}
	if (ka & aart::surface::eReflect)
	{
		kn |= ER_REFLECT ;
		C_cpy(ptr-> col_r, asrc-> reflect) ;
	}
	if (ka & aart::surface::eGlass)
	{
		C_cpy(ptr-> col_t, asrc-> glass) ;

		dtmp= asrc-> refraction ;
		if (dtmp > 1.000001)
		{
			kn |= ER_GLASS ;
			ptr-> indx= dtmp, ptr-> rindx= 1.0 / dtmp ;
			c2= dtmp * dtmp, ptr-> sqindx= c2, ptr-> rqindx= 1.0 / c2 ;
			ptr-> in_t1= c2 + 1.0 ;
			dval= 1.0 / (8 * c2) + 0.5 / (1 + c2 * (2 + c2)) ;
			ptr-> in_t2= (1 - dval * (3 * c2 + 1)) / dval ;
			dtmp= C_bright(asrc-> glass) ;
			if (dtmp < 1.0) { ptr-> ut= dtmp, ptr-> uit= 1.0 - dtmp ; }
				else ptr-> ut= 1.0, ptr-> uit= 0.0 ;
		}
		else
		{
			kn |= ER_VAPOR ;
			ptr-> indx= ptr-> rindx= ptr-> sqindx= ptr-> rqindx= 1.0 ;
		}
	}

	if (asrc-> isinvis)
		kn |= ERS_INVIS ;

	switch (asrc-> smode)
	{
		case aart::surface::eAlphaLite:
			kn |= ERS_ALITE ;
			break ;
		case aart::surface::eLiquid:
			kn |= ER_SPECULAR ;
			ptr-> v1= cos(M_PI * (asrc-> za + asrc-> zb) / 180.0),
			ptr-> v2= cos(M_PI * asrc-> za / 180.0) ;
			dtmp= ptr -> v2 - ptr-> v1 ;
			ptr-> v3= (dtmp > 0.0) ? 1.0 / dtmp : 0.0 ;
			basisptr-> lighting= rl_liquid ;
			break ;
		case aart::surface::ePosterize:
			kn |= ER_DIFFUSE | ER_SPECULAR ;
			ptr-> v1= asrc-> za, ptr-> v2= asrc-> zb ;
			dtmp= asrc-> zc ;
			if (dtmp > 0.0) { ptr-> v3= dtmp, ptr-> v4= 0.5 / dtmp ; } else { ptr-> v3= 0.0, ptr-> v4= 0.0 ; }
			basisptr-> shader= rc_posterize ;
			basisptr-> lighting= rl_posterize ;
			break ;
	}

	ptr-> attrib= kn ;
	ptr-> bend= 1.0 ;
}

bool	applytexture(ev_mapping * destmap, ev_mapdat * destcore, er_surface * asrc, const aart::texture * atexture)
{
	long	kb ;
	ev_smap *epptr;

	switch (atexture-> mtype)
	{
		case aart::texture::eColor:
			epptr= cpmap_lookup(atexture-> mapname, EMP_COLOR);
			if (NULL == epptr)
				return false ;

			destmap-> mapt= 1,
			destmap-> mapcode= TRS_TCOLOR,
			destmap-> mapstencil= 0,
			destmap-> pixmapref= epptr;

			kb= asrc-> attrib ;
			if (kb & ER_AMBIENT)
			{
				C_cpy(destcore-> ambmix, c_ambient) ;
				destcore-> cx_a= C_bright(asrc-> col_a) ;
			}
			if (kb & ER_DIFFUSE) destcore-> cx_d= C_bright(asrc-> col_d) ;
			if (kb & ER_SPECULAR) destcore-> cx_s= C_bright(asrc-> col_s) ;
			if (kb & ER_REFLECT) destcore-> cx_r= C_bright(asrc-> col_r) ;
			if (kb & ER_GLASS) destcore-> cx_t= C_bright(asrc-> col_t) ;
			destcore-> mapfin= kb ;
			break ;

		case aart::texture::eBump:
			epptr= cpmap_lookup(atexture-> mapname, EMP_BMP);
			if (NULL == epptr)
				return false ;

			destmap-> mapt= 4 ;
			destmap-> mapcode= TRS_TBUMP ;
			destmap-> mapstencil= 0 ;
			destmap-> pixmapref= epptr ;
			M_zero(destcore-> imatx)  destcore-> imatx.x= 1.0;
			M_zero(destcore-> imaty)  destcore-> imaty.y= 1.0;
			M_zero(destcore-> imatz)  destcore-> imatz.z= 1.0;
			break ;

		case aart::texture::eEnvir:
			epptr= cpmap_lookup(atexture-> mapname, EMP_COLOR);
			if (NULL == epptr)
				return false ;

			destmap-> mapt= 1,
			destmap-> mapcode= TRS_TENVIR,
			destmap-> mapstencil= 0,
			destmap-> pixmapref= epptr;

			asrc->attrib |= ER_ENVIR ;
			destcore-> pjcode= ERT_PENV;
			M_zero(destcore-> matx) M_zero(destcore-> maty)
				M_zero(destcore-> matz) M_zero(destcore-> matw)
			destcore-> pscalex= 0.5 / M_PI,
			destcore-> pscaley= 1.0 / M_PI;
			destcore-> plat= 0.5, destcore-> plng= 0.5;
			break ;

		default:
			return false ;
	}

	return true ;
}

static void	initprojection(ev_mapdat * aproj)
{
	aproj-> mapct= 0, aproj-> mapfin= 0,
	aproj-> xsolid= 0,
	aproj-> cx_a= aproj-> cx_d= aproj-> cx_s=
		aproj-> cx_r= aproj-> cx_t= aproj-> cx_i= 0.0,
	aproj-> pjcode= aproj-> prepx= aproj-> prepy= 0,
	aproj-> pscalex= aproj-> pscaley= 0.0;
	aproj-> matx.x= 1.0, aproj-> matx.y= 0.0, aproj-> matx.z= 0.0,
	aproj-> maty.x= 0.0, aproj-> maty.y= 1.0, aproj-> maty.z= 0.0,
	aproj-> matz.x= 0.0, aproj-> matz.y= 0.0, aproj-> matz.z= 1.0,
	aproj-> matw.x= 0.0, aproj-> matw.y= 0.0, aproj-> matw.z= 0.0;
}

static unsigned int	projtype(aart::texture::Type atype)
{
	switch (atype)
	{
		case aart::texture::eColor:
		case aart::texture::eBump:
			return 1 ;
		case aart::texture::eEnvir:
			return 2 ;
		default:
			return 0 ;
	}
}

void	applytexturing(ev_surfbsi * adest, tbagstepper<aart::texture *> * astep, ev_mapdat * & aproj, ev_mapping * & amap)
{
	aart::texture * textureptr ;
	ev_mapdat	* firstmap ;
	unsigned int	pjtype, pjpast, pjcount ;

//	initprojection(aproj) ;
//	aproj-> maplist= amap ;
	firstmap= NULL ;
	pjtype= pjpast= 0, pjcount= 0 ;

	adest-> surfmix= adest-> surfp +1 ;
	memcpy(adest-> surfmix, adest-> surfp, sizeof(er_surface)) ;

	while (astep-> next(textureptr))
	{
		pjtype= projtype(textureptr-> mtype) ;
		if (! pjtype) continue ;

		if (pjtype != pjpast)
		{
			if (pjpast) aproj ++, pjcount ++ ; else firstmap= aproj, pjcount= 1 ;
			initprojection(aproj) ;
			aproj-> maplist= amap ;
			pjpast= pjtype ;
		}
		if (applytexture(amap, aproj, adest-> surfp, textureptr))
		{
			amap ++ ;
			aproj-> mapct ++ ;
		}
	}

	if (firstmap && pjcount)
	{
		adest-> maplist= firstmap ;
		adest-> mapct= pjcount ;
		aproj ++;
	}

	delete astep ;
}

