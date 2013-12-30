#include "aar.h"

#include "aart_element.h"
#include "aart_link.h"

aart_link::aart_link(aart_element * aele) : mjoin(aele), dataptr(NULL)
{
	ASSERTPOINTER(mjoin) ;

	mjoin-> mlink= this ;
}

aart_link:: ~ aart_link()
{
	if (mjoin)
		mjoin-> mlink= NULL ;
}
