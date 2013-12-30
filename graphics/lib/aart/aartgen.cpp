#include "aar.h"

#include	<ctype.h>

#include	"aart_gen.h"
#include	"aart_post.h"

aart_component *	aart_gen::create(const char * aname)
{
	char tmpstr[64] ;
	aart_gen * step ;

	generalstring::captureKnown(tmpstr, aname) ;
	for (step= list; (step); step= step-> next)
		{ if (step-> test(tmpstr)) return step-> generate(tmpstr) ; }

	return NULL ;
}

aart_gen::aart_gen()
{
	next= list, list= this ;
}

aart_gen:: ~ aart_gen() { }

aart_gen *	aart_gen::list = NULL ;

	/// utility

	// matchword
bool	aart_gen::matchword(const char * astring, const char * bstring)
{
	char c ;

	while ((c= * astring) && ! isspace(c))
	{
		if (c != * bstring) return false ;
		astring ++, bstring ++ ;
	}

	return true ;
}

	////
	// library suckiness

class	post_generator : public aart_tgen<aart_post>
	{ protected: TEST_NAME("post") ; } ;

void	aart_gen::init()
{
	new post_generator() ;
}

void	aart_gen::close()
{
	aart_gen * step, * fut ;

	for (step= list, fut= NULL; (step); step= fut)
	{
		fut= step-> next ;
		delete step ;
	}
	list = NULL ;
}