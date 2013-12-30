#include "aar.h"

#include	"aart_core.h"
#include	"aart_node.h"

aart::geometry::geometry(const char * astr, aart_component * aparent) : aart_element(astr, aparent)
{
	M_zero(position) ;
	M_load(scale, 1.0) ;

	* config= '\0', * motion= '\0' ;
	doserv= false ;
}

#define	EPSILON(uu) (-1e-8 < uu) && (uu < 1e-8)
#define	NEPSILON(uu) (uu < -1e-8) || (1e-8 < uu)

bool	aart::geometry::parse(const char * astr, FILE * fsrc)
{
	unsigned long	lcode= atocode(astr) ;

	switch (lcode)
	{
		case 'POSI':
			if (! parsevec(position, astr))
				return false ;

			break ;
		case 'SCAL':
			if (! parsevec(scale, astr))
				return false ;

			if (EPSILON(scale.x) && EPSILON(scale.y) && EPSILON(scale.z))
			{
				generallog::message("geometry scale set to zero", GL::eFatal) ;
				return false ;
			}

			break ;

		case 'QUAT':	// chew quaternions
			break ;

		case 'CONF':
			generalstring::skip_word(astr) ;
			if (* astr)
			{
				char tmpstr[256], * p ;
				generalstring::paramKnown(tmpstr, astr) ;
				generalstring::copyKnown(config, tmpstr) ;
				if (p= strrchr(tmpstr, '/')) *(p ++)= '\0' ;
					else if (p= strrchr(tmpstr, '\\')) *(p ++)= '\0' ;

				if (p && parent)
				{
					aart_node::geometryset * par= (aart_node::geometryset *) parent ;
					par->paths.add(tmpstr) ;
				}

				if (* motion) doserv= true ;
			}
			break; 
		case 'MOTI':
			generalstring::skip_word(astr) ;
			if (* astr)
			{
				generalstring::paramKnown(motion, astr) ;
				if (* config) doserv= true ;
			}
			break ;

		case 'GEOM':
		case 'USE ':
			generalstring::skip_word(astr) ;
			if (* astr)
			{
				char tmpstr[256] ;

				generalstring::paramKnown(tmpstr, astr) ;
				src.add(tmpstr) ;
			}
				else return false ;

			break ;

		case 'IS  ':
			generalstring::skip_word(astr) ;
			if (* astr)
			{
				char tmpstr[64] ;

				generalstring::ncaptureword(tmpstr +1, astr, sizeof(tmpstr) -1) ;
				* tmpstr= '^' ;
				src.add(tmpstr) ;
			}
			break ;

		default:
			ignore(astr) ;
	}

	return true ;
}

bool	aart::geometry::print(aart_output * adest)
{
	int istep, isize ;
	adest-> printf("{actor'%s\n", mname) ;

	if (* config) { adest-> printf("config\t%s\n", config) ; }
	if (* motion) { adest-> printf("motion\t%s\n", motion) ; }
	for (istep= 0, isize= src.getcount(); (istep < isize); istep ++)
	{
		const char * p= src[istep] ;

		if ('^' == * p) adest-> printf("is %s\n", p +1) ;
			else adest-> printf("geom %s\n", p) ;
	}
	if (NEPSILON(position.x) || NEPSILON(position.y) || NEPSILON(position.z))
		adest-> printf("position %g %g %g\n", M_expand(position)) ;

	if ((1.0 != scale.x) || (1.0 != scale.y) || (1.0 != scale.z))
		adest-> printf("scale %g %g %g\n", M_expand(scale)) ;
	if (other.getcount())
		adest-> print("# ---\n") ;

	return aart_component::print(adest) ;
}
