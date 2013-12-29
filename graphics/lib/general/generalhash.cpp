#include <general.h>
#include "generalhash.h"

	// construct, start at zero
generalhash::generalhash()
{
	lv= 0 ;
	cycle= 0 ;
}

const int	generalhash::pattern[] = { 0, 4, 8, 12, 16, 20, 24, 23, 15, 7, 21, 13, 5, 19, 11, 3, 17, 9, 1, 2, 14, 18, 6, 10, 22, 3, 12, 21, 0, 9, 18, 5, 14, 23, 1, 10, 19, 24, 7, 16, 17, 4, 13, 22, 2, 11, 20, 6, 15, 8 } ;

	// data - add contents to checksum
void	generalhash::data(const void * adata, unsigned long alen)
{
	const unsigned char * aptr= (const unsigned char *) adata ;

	ASSERTPOINTER(aptr) ;
	ASSERT(alen > 0) ;

	for (; (alen); alen --)
	{
		lv ^= (* (aptr ++) << pattern[cycle ++]) ;
		if ((sizeof(pattern) / sizeof(int)) == cycle) cycle= 0 ;
	}
}


