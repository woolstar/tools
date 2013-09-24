#include <buffer.h>
#include <new>

bool	btl::expand_alloc::expand(int asize)
{
	size_t	cur, larger ;
	sized_storage * cbuf ;

	cbuf= reinterpret_cast<sized_storage *>( rawbuffer_ ) ;
	cur= limit_ - cbuf ;
	if ( asize < (cur / 4) ) { asize= cur / 4; }
	larger= cur + asize ;

	cbuf= new sized_storage[larger] ;

	rawbuffer_= cbuf ;
	storage_.reset( cbuf ) ;  // frees old memory

	fill_= cbuf + datasize_ ;
	limit_= cbuf + larger ;
}

