#include <general.h>
#include "generaldata.h"

	// constructor
	// make initial size
garray::garray(unsigned int asize)
{
	if (asize < 4) asize= 4 ;

	container= (void **) malloc(asize * sizeof(void *)) ;
	if (NULL == container)
		mmax= 0; else mmax= asize ;

	mcount= 0;
}

garray:: ~ garray()
{
	if (NULL != container)
	{
		free(container) ;
		container= NULL ;
	}
}

	// add(ptr)
	// stick pointers in container (if fits)
bool	garray::add(void * aptr)
{
	if (mcount >= mmax)
	{
		unsigned int tmpsize ;
		if (mmax > 4096) tmpsize= mmax +1024 ;
			else tmpsize= 2 * mmax;
		size(tmpsize) ;
	}

	if (mcount < mmax)
	{
		container[mcount ++]= aptr ;
		return true ;
	}

	return false ;
}

void	garray::move(garray & adest)
{
	adest.clear() ;
	adest.size(mcount) ;
	adest.mcount= mcount ;
	memcpy(adest.container, container, mcount * sizeof(void *)) ;
	clear() ;
}

	// size()
	// expand to fit N items
void	garray::size(unsigned int asize)
{
	void ** tmpref ;
	if (asize <= mmax) return ;

	tmpref= (void **) realloc(container, asize * sizeof(void *)) ;
	if (tmpref != NULL)
	{
		container= tmpref ;
		mmax= asize ;
	}
}

	// get()
	// bounds check access, then lookup
void *	garray::get(unsigned int aindex) const
{
	if (aindex >= mcount) return NULL ;

	return container[aindex] ;
}

