#include "aar.h"

#include	"aart_core.h"

aart::texture::texture(Type atype, const char * afile, aart_component * aparent) : aart_component(aparent)
{
	mtype= atype ;
	generalstring::copyKnown(mapname, afile) ;
}

bool	aart::texture::print(aart_output * adest)
{
	const char * ptype ;

	switch (mtype)
	{
		case eNone:
			return false ;
		case eColor:
			ptype= "color" ;
			break ;
		case eBump:
			ptype= "bump" ;
			break; 
		case eEnvir:
			ptype= "envir" ;
			break ;
	}

	adest-> printf("texture %s %s\n", ptype, mapname) ;
	return true ;
}

aart::texture::Type	aart::texture::atotype(const char * astr)
{
	if (! strncmp(astr, "color", 5))
		return eColor ;

	if (! strncmp(astr, "bump", 4))
		return eBump ;

	if (! strncmp(astr, "envi", 4))
		return eEnvir ;

	return eNone ;
}