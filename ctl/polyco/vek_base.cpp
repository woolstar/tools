#include <polyco>
#include <cstring>

using ctl::__detail::vector_base ;

	bool vector_base::strace = false ;


vector_base:: ~ vector_base()
{
	clear() ;
}

void	vector_base::clear( void )
{
}

void vector_base::reserve(size_t dsize, size_t nsize )
{
	size_t mtot= use_ + dsize ;

	if ( mtot > total_ )
	{
		size_t mgrow= 1.2 * total_ ;
		if ( mtot > mgrow ) { mgrow= mtot ; }
		unsigned char * pnew = new unsigned char [mgrow] ;
		
		relocate( storage_.get(), storage_.get() + use_, pnew ) ;
		storage_.reset( pnew) ;
		total_ = mgrow ;
	}

	offsets_.reserve( nsize ) ;
}

void vector_base::shrink( size_t )
{
}

