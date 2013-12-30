#include "aar.h"

#include	"aart_core.h"

aart::light::light(const char * astr, aart_component * aparent) : aart_element(astr, aparent)
{
	mtype= eUnknown ;

	bright= 1.0 ;
	C_set(mcolor) ;

	M_zero(position) ;
	M_zero(direction) ;

	spot= 0.0 ;
	mshadow= false ;
}

bool	aart::light::parse(const char * astr, FILE *)
{
	unsigned long	lcode= atocode(astr) ;

	switch (lcode)
	{
			// type
		case 'SPOT' :
			mtype= eSpot ;
			break ;
		case 'POIN':
			mtype= ePoint ;
			break ;
		case 'BEAM':
			mtype= eBeam ;
			break ;

			// data
		case 'BRIG':	// bright
			generalstring::skip_word(astr) ;
			if (! * astr)
				return false ;
			bright= (float) atof(astr) ;
			break ;
		case 'COLO':
			if (! parsecolor(mcolor, astr))
				return false ;
			break; 
		case 'POSI':
			if (! parsevec(position, astr))
				return false ;
			break; 
		case 'DIRE':
			if (! parsevec(direction, astr))
				return false ;
			break; 

			// options
		case 'CONE':
			generalstring::skip_word(astr) ;
			if (! * astr)
				return false ;
			spot= (float) atof(astr) ;
			break ;
		case 'SHAD':
			mshadow= true ;
			break; 

		default:
			if (strncmp(astr, "# ---", 5))
				ignore(astr) ;
	}

	return true ;
}

bool	aart::light::print(aart_output * adest)
{
	if (eUnknown == mtype)
		return false ;

	adest-> printf("{light'%s\n", mname) ;

	switch (mtype)
	{
		case eBeam:
			adest-> print("beam\n") ;
			adest-> printf("direction %g %g %g\n", M_expand(direction)) ;
			break ;
		case ePoint:
			adest-> print("point\n") ;
			adest-> printf("position %g %g %g\n", M_expand(position)) ;
			break ;
		case eSpot:
			adest-> print("beam\n") ;
			adest-> printf("position %g %g %g\n", M_expand(position)) ;
			adest-> printf("direction %g %g %g\n", M_expand(direction)) ;
			adest-> printf("cone %g\n", spot) ;
			break ;
	}

	adest-> printf("bright %g\n", bright) ;
	adest-> printf("color %g %g %g\n", C_expandl(mcolor)) ;

	if (mshadow)
		adest-> print("shadow\n") ;

	if (other.getcount())
		adest-> print("# ---\n") ;

	return aart_component::print(adest) ;
}
