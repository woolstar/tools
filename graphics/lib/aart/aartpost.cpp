#include "aar.h"
#include "aart_post.h"

#include	<ctype.h>

	////

	// construct
	//	setup defaults
aart_post::aart_post(const char *)
{
	adapt= 0 ;
	rdepth= 1, tdepth= 1 ;

	filter= eNone ;
	filterwindow= 0 ;

	strcpy(destpath, "image*0001.tga") ;
	* mcamera= '\0' ;

	textureflip= false ;
	base= this ;
}

aart_post:: ~ aart_post()
{
	if (this == base) base= NULL ;
}


	// default post block
aart_post	* aart_post::base= NULL ;

	// retrieval
const aart_post *	aart_post::getpost(void)
{
	if (NULL == base)
		base= new aart_post("") ;

	return base ;
}

const aart_post *	aart_post::getpost(aart_node *)
{
	if (NULL == base)
		base= new aart_post("") ;

	return base ;
}

aart::camera *	aart_post::getactive(tbagstepper<aart::camera *> * astep) const
{
	aart::camera * tmpcam ;

	ASSERTPOINTER(astep) ;

	tmpcam= NULL ;
	if (astep-> next(tmpcam))
	{
		if (* mcamera)
		{
			while (tmpcam && strcmp(mcamera, tmpcam-> getname()))
				{ if (! astep-> next(tmpcam)) tmpcam= NULL ; }
		}
	}

	delete astep ;
	return tmpcam ;
}

	//// internal

	// parse
bool	aart_post::parse(const char * astr, FILE *)
{
	unsigned long	lcode= atocode(astr) ;

	switch (lcode)
	{
		case 'ADAP':
			generalstring::skip(astr) ;
			if (isdigit(* astr))
				adapt= atoi(astr) ;
			break ;

		case 'ACTI':
			generalstring::skip(astr) ;
			generalstring::captureKnown(mcamera, astr) ;
			break ;

		case 'DEPT':
			generalstring::skip(astr) ;
			if (! isdigit(* astr))
				return false ;

			rdepth= atoi(astr) ;
			generalstring::skip(astr) ;
			if (isdigit(* astr))
				tdepth= atoi(astr); else tdepth= rdepth ;

			break ;

		case 'FILT':
			generalstring::skip(astr) ;
			lcode= atocode(astr) ;
			switch (lcode)
			{
				case 'BOX':  filter= eBox, filterwindow= 0 ;  break; 
				case 'GAUS':
					filter= eGauss ;
					generalstring::skip(astr) ;
					filterwindow= atoi(astr) ;
					break ;
				case 'LANC':
				case 'SYNC':
					filter= eSync;
					generalstring::skip(astr) ;
					filterwindow= atoi(astr) ;
					break ;
				default:
					return false ;
			}
			break; 

		case 'TARG':
			generalstring::skip(astr) ;
			if (! strncmp(astr, "down", 4)) textureflip= true ;
				else if (! strncmp(astr, "up", 4)) textureflip= false ;
			break ;

		case 'OUTP':
			generalstring::skip(astr) ;
			generalstring::paramK(destpath, astr) ;

			break ;

		default:
			ignore(astr) ;
	}

	return true ;
}

bool	aart_post::print(aart_output * adest)
{
	adest-> print("{post\n") ;
	if (adapt)
		adest-> printf("adapt %d\n", adapt) ;
	if ((1 != rdepth) || (1 != tdepth))
		adest-> printf("depth %d %d\n", rdepth, tdepth) ;
	if (* mcamera)
		adest-> printf("activecamera %s\n", mcamera) ;

	switch (filter)
	{
		case eBox:  adest-> print("filter box\n"); break;
		case eGauss:  adest-> printf("filter gauss %d\n", filterwindow); break;
		case eSync:  adest-> printf("filter sync %d\n", filterwindow); break;
	}
	adest-> printf("output %s\n", destpath) ;

	if (textureflip)
		adest-> print("targa down\n") ;
	else
		adest-> print("targa up\n") ;

	if (other.getcount())
		adest-> print("# ---\n") ;
	return aart_component::print(adest) ;
}

aart_component *	aart_post::find(const char * aname)
{
	aart_component * ptr = NULL ;
	const char * asear ;

	if (! strncmp(aname, "effect", 6))
	{
		generalstring::skip(aname) ;
		asear= aname ;
		generalstring::skip(asear) ;
		if (! * asear) asear= aname ;

		switch (* aname)
		{
			case 't':	// toon
				ptr= new toon(asear, this) ;
				add(ptr) ;
				break ;
		}
	}

	return ptr ;
}