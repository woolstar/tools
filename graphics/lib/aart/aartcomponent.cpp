#include "aar.h"
#include	<ctype.h>

	// construct
	//	initialize parent to null
aart_component::aart_component()
{
	parent= NULL ;
}

aart_component::aart_component(aart_component * aparent)
{
	parent= aparent ;
}

aart_component:: ~ aart_component()
{
	if (parent) parent-> remove(this) ;
	contains.destroy() ;
}

	// add
	//	add component onto child list (contains)
void	aart_component::add(aart_component * asib)
{
	contains.append(asib) ;
}

	// remove
	//	remove child element from set (called by child destructor)
void	aart_component::remove(aart_component * achld)
{
	contains.remove(achld) ;
}

	// ignore
	//	stuff in the other array for storage
void	aart_component::ignore(const char * aline)
{
	other.add(aline) ;
}

	// parse
	//	default behavior
	//	stuff everything in ignore
bool	aart_component::parse(const char * astring, FILE * fsrc)
{
	ignore(astring) ;
	return true ;
}

	// print
	// provide common denominator output
bool	aart_component::print(aart_output * adest)
{
	tbagstepper<aart_component *> * step ;
	aart_component * ptr ;
	bool bres ;

	bres= true ;
	step= new tbagstepper<aart_component *>(& contains) ;
	while (step-> next(ptr))
		{ if (! ptr->print(adest)) break ;  }
	delete step ;

	other.print(adest) ;
	adest-> print("}\n") ;
	return true ;
}

	//// support

	// atocode
	//	strip up to four characters into a long
unsigned long	aart_component::atocode(const char * aword)
{
	char * refptr, * destptr ;
	unsigned long c, ltmp ;
	static unsigned long	sref = '\003\002\001\0' ;
	unsigned int ustep ;

	ltmp= 0;
	refptr= (char *) & sref, destptr= (char *) & ltmp ;
	for (ustep= 4; (ustep && (c= *(aword ++))); ustep --, refptr ++)
	{
		if (islower(c)) c= toupper(c) ;
			else if ((' ' == c) || ('\t' == c)) break ;
		destptr[* refptr]= (char) c;
	}
	while (ustep) { ustep --, destptr[* (refptr ++)]= ' '; }

	return ltmp ;
}