#include "aar.h"
#include "aart_post.h"

aart_post::toon::toon(const char * aname, aart_component * apar) : aart_post::effect(aname, apar)
{
	cline_w= 0.0,
	cline_h= 0.0,
	cline_t= 0.0,
	tline_w= 0.0,
	tline_h= 0.0,
	tline_t= 0.0 ;

	memset(line, 0, sizeof(line)) ;
	luse= 0 ;
}

bool	aart_post::toon::parse(const char * aline, FILE *)
{
	char c= * aline ;
	int	itmp ;

	generalstring::skip(aline) ;
	switch (tolower(c))
	{
		case 'c':	// curvatureLine
			c= * aline ;
			switch (tolower(c))
			{
				case 'h':	// hardness
					if (! parseval(cline_h, aline)) return false ;  break ;
				case 't':	// threshold
					if (! parseval(cline_t, aline)) return false ;  break ;
				case 'w':	// width
					if (! parseval(cline_w, aline)) return false ;  break ;
				default:
					generallog::message("unexpected attribute for curvatureLine (expected width|threshold|hardness)", GL::eWarning) ;
					return false ;
			}
			break ;
		case 't':	// trickLine
			c= * aline ;
			switch (tolower(c))
			{
				case 'h':	// hardness
					if (! parseval(tline_h, aline)) return false ;  break ;
				case 't':	// threshold
					if (! parseval(tline_t, aline)) return false ;  break ;
				case 'w':	// width
					if (! parseval(tline_w, aline)) return false ;  break ;
				default:
					generallog::message("unexpected attribute for trickLine (expected width|threshold|hardness)", GL::eWarning) ;
					return false ;
			}
			break ;
		case 'l':	// lineColor
			itmp= atoi(aline) ;
			if ((itmp < 1) || (itmp > 16))
			{
				generallog::message("lineColor out of range (1-16)", GL::eWarning) ;
				return false ;
			}

			generalstring::skip(aline) ;
			itmp -- ;
			if ('[' != * aline)
				return false ;

			aline ++ ;
			if (parsecolor(line[itmp], aline)) { luse |= (1 << itmp) ; }
				else return false ;
			break ;

		default:
			if (strncmp(aline, "# ---", 5))
				ignore(aline) ;
	}

	return true ;
}

bool	aart_post::toon::print(aart_output * adest)
{
	int	istep ;

	adest-> printf("{effect toon %s\n", mname) ;

	adest->printf("curvatureLine width %g\n", cline_w) ;
	adest->printf("curvatureLine threshold %g\n", cline_t) ;
	adest->printf("curvatureLine hardness %g\n", cline_h) ;
	adest->printf("trickLine width %g\n", tline_w) ;
	adest->printf("trickLine threshold %g\n", tline_t) ;
	adest->printf("trickLine hardness %g\n", tline_h) ;

	for (istep= 0; (istep < 16); istep ++)
	{
		if (luse & (1 << istep))
			adest-> printf("lineColor %d [%g %g %g]\n", istep +1, C_expandl(line[istep])) ;
	}

	if (other.getcount())
		adest-> print("# ---\n") ;
	return aart_component::print(adest) ;
}
