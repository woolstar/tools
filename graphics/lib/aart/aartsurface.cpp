#include "aar.h"

#include	"aart_core.h"
#include	<ctype.h>

aart::surface::surface(const char * astr, aart_component * aparent) : aart_element(astr, aparent)
{
	muse= 0, mlink= 0 ;

	C_zero(ambient) ;
	C_zero(diffuse) ;
	C_zero(specular) ;
	C_zero(reflect) ;

	gloss= 0.0 ;
	refraction= 1.0 ;
	alpha= 1.0 ;
	isinvis= false ;

	layer= 0 ;
	smode= eStandard ;
}

aart::surface:: ~ surface()
{
}

int	aart::surface::parsez(const char * astr)
{
	int ct, clim ;
	float * fp ;
	char c ;

	generalstring::skip_word(astr) ;
	for (ct= 0, clim= 3, fp= & za; (clim -- && (c= * astr)); ct ++, fp ++)
	{
		if (! isdigit(c) && ('-' != c)) break ;
		* fp= (float) atof(astr) ;
		generalstring::skip(astr) ;
	}
	return ct ;
}

#define	EPSILON(uu) (uu < -1e-8) || (1e-8 < uu)
#define	CSET(vv) (vv.r > 1e-8) || (vv.g > 1e-8) || (vv.b > 1e-8)

bool	aart::surface::parse(const char * astr, FILE *)
{
	unsigned long	lcode= atocode(astr) ;

	switch (lcode)
	{
		case 'AMBI':
			if (! parsecolor(ambient, astr))
				return false ;
			if (CSET(ambient))
				muse |= eAmbient ;
			break ;
		case 'DIFF':
			if (! parsecolor(diffuse, astr))
				return false ;
			if (CSET(diffuse))
				muse |= eDiffuse ;
			break ;
		case 'SPEC':
			if (! parsecolor(specular, astr))
				return false ;
			if (CSET(specular))
				muse |= eSpecular ;
			break ;
		case 'REFL':
			if (! parsecolor(reflect, astr))
				return false ;
			if (CSET(reflect))
				muse |= eReflect ;
			break ;
		case 'GLOW':
			if (! parsecolor(glow, astr))
				return false ;
			if (CSET(glow))
				muse |= eGlow ;
			break ;
		case 'GLAS':
		case 'TRAN':
			if (! parsecolor(glass, astr))
				return false ;
			if (CSET(glass))
				muse |= eGlass ;
			break ;
		case 'EMIS':
			break ;

			// attributes
		case 'GLOS':
			if (! parseval(gloss, astr))
				return false ;
			break ;
		case 'INDE':
			if (! parseval(refraction, astr))
				return false ;
			break ;
		case 'ALPH':
			if (! parseval(alpha, astr))
				return false ;
			break ;
		case 'INVI':
			isinvis= true ;
			break ;

			// special modes
		case 'ALIT':	// alite (alpha light)
			if (! parseval(alpha, astr))
				return false ;
			smode= eAlphaLite ;
			break ;
		case 'LIQU':	// liquid
			if (2 != parsez(astr))
				return false ;
			// check params XXXX
			smode= eLiquid ;
			break ;
		case 'POST':	// posterize (toon)
			if (3 != parsez(astr))
				return false ;
			// check params XXXX
			smode= ePosterize ;
			break ;

			// textures
		case 'TEXT':
			generalstring::skip(astr) ;
			if (! parse_texture(astr))
				return false ;
			break ;
		default:
			if (strncmp(astr, "# ---", 5))
				ignore(astr) ;
	}

	return true ;
}

bool	aart::surface::print(aart_output * adest)
{
	double dtmp ;

	adest-> printf("{surface'%s\n", mname) ;

	if (muse & eAmbient)
		adest-> printf("ambient %g %g %g\n", C_expandl(ambient)) ;
	if (muse & eGlow)
		adest-> printf("glow %g %g %g\n", C_expandl(glow)) ;

	if (muse & eDiffuse)
		adest-> printf("diffuse %g %g %g\n", C_expandl(diffuse)) ;
	if (muse & eSpecular)
	{
		adest-> printf("specular %g %g %g\n", C_expandl(specular)) ;
		if (gloss > 1e-8)
			adest-> printf("gloss %g\n", gloss) ;
	}
	if (muse & eReflect)
		adest-> printf("reflect %g %g %g\n", C_expandl(reflect)) ;
	if (muse & eGlass)
	{
		adest-> printf("glass %g %g %g\n", C_expandl(glass)) ;
		if (refraction > 1.0)
			adest-> printf("index %g\n", refraction) ;
	}

	dtmp= alpha - 1.0 ;
	if ((eAlphaLite != smode) && EPSILON(dtmp)) adest-> printf("alpha %g\n", alpha) ;
	switch (smode)
	{
		case eAlphaLite:
			adest-> printf("alite %g\n", alpha) ;  break ;
		case eLiquid:
			adest-> printf("liquid %g %g\n", za, zb) ;  break ;
		case ePosterize:
			adest-> printf("posterize %g %g %g\n", za, zb, zc) ;  break ;
	}
	if (isinvis)
		adest-> print("invis\n") ;

	if (other.getcount())
		adest-> print("# ---\n") ;
	return aart_component::print(adest) ;
}

bool	aart::surface::parse_texture(const char * aparam)
{
	char	tmpfile[256] ;
	aart::texture::Type	utype ;
	aart::texture	* ptr ;

	utype= aart::texture::atotype(aparam) ;
	if (aart::texture::eNone == utype)
		return false ;

	generalstring::skip(aparam) ;
	generalstring::paramKnown(tmpfile, aparam) ;
	ptr= new aart::texture(utype, tmpfile, this) ;
	add(ptr) ;
	return true ;
}

tbagstepper<aart::texture *> *		aart::surface::getstep()
{
	return new tbagstepper<aart::texture *>(& contains) ;
}

	// leavescope
	//	called at completion of parsing
	//	time to work out attribute linking
void	aart::surface::leavescope()
{
}

