#include <buffer>
#include <new>
#include <cstring>

void	btl::expand_alloc::expand(int asize)
{
	size_t	cur, larger ;
	sized_storage * cbuf ;

	cbuf= & ( storage_[0] ) ;
	cur= limit_ - cbuf ;
	if ( asize < (cur / 4) ) { asize= cur / 4; }
	larger= cur + asize ;

	cbuf= new sized_storage[larger] ;
	cur= size() ;
	std::memcpy( cbuf, rawbuffer_, cur ) ;

	rawbuffer_= cbuf ;
	storage_.reset( cbuf ) ;  // frees old memory

	buffer::swap( cbuf, cbuf + cur ) ;
	limit_ = cbuf + larger ;
}

