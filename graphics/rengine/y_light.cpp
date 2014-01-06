#include "ye.h"

extern "C"
{
	// rv light
	extern int	ct_lights ;
	extern ev_light	* magic_light ;

	extern bool	rvm_shadowcolumn(ev_light *, int, int),
		rvm_shadowcube(ev_light *, int, int),
		rvm_shadowbuffer(ev_light *, int, int) ;
	extern void	rvm_destroy(void) ;
} ;

	// generation

static void		installight(ev_light * adest, aart::light * asrc)
{
	double dtmp, dk ;

	dtmp= asrc-> bright ;
	C_cpy(adest-> color, dtmp * asrc-> mcolor) ;
	M_cpy(adest-> pt, asrc-> position) ;
	M_cpy(adest-> direc, asrc-> direction) ;

	if (asrc-> mshadow)
		adest-> bits |= ERLA_SHAD ; else adest-> bits &= ~ ERLA_SHAD ;

	switch (asrc-> mtype)
	{
		case aart::light::eBeam:
			adest-> tipe= ERL_BEAM;
			M_selfnorm(adest-> direc) ;
			break ;
		case aart::light::ePoint:
			adest-> tipe= ERL_POINT;
			break ;
		case aart::light::eSpot:
			adest-> tipe= ERL_SPOT;
			M_selfnorm(adest-> direc) ;

			dtmp= asrc-> spot ;
			dk= M_PI / 180.0 ;
			adest-> cone_edge= cos(dtmp * dk) ;
			if (adest-> cone_edge < 1e-5)
				adest-> cone_edge= 0.0 ;
			break ;
	}
}

bool	yzor::dolight()
{
	aart::light	* lightrec ;
	tbagstepper<aart::light *> * step ;
	ev_light * lightfill ;
	int icount ;

	if (NULL == mnode-> lightref)
		return false ;

	step= mnode-> lightref-> getstep() ;
	for (icount= 0; (step-> next(lightrec)); )
	{
		if ((aart::light::eUnknown != lightrec-> mtype)
				&& (lightrec-> bright != 0.0 ))
			icount ++ ;
	}

	if (! icount)
	{
		delete step ;
		return false ;
	}

		// make room for render lights
	magic_light= (ev_light *) malloc(icount * sizeof(ev_light)) ;
	if (NULL == magic_light)
	{
		delete step ;
		return false ;
	}

	step-> reset() ;
	for (lightfill= magic_light; (step-> next(lightrec)); lightfill ++)
	{
			// skip bad lights & black lights
		if ((aart::light::eUnknown == lightrec-> mtype)
				|| (lightrec-> bright == 0.0))
			continue ;

		lightfill-> tipe= 0,
		lightfill-> bits= 0;
		lightfill-> samples= 0,
		lightfill-> lshadmap= NULL,
		lightfill-> lprojmap= NULL,
		lightfill-> cache_1= NULL,
		lightfill-> cache_2= NULL;
		lightfill-> safedist= 1e-12;

		installight(lightfill, lightrec) ;
	}

	ct_lights= icount ;
	delete step ;

	return true ;
}