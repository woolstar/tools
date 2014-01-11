#include <polyco>
#include <cstring>

using ctl::__detail::tank_base ;

tank_base:: ~ tank_base()
{
}

void tank_base::reserve(size_t asize)
{
	size_t mtot= use_ + asize ;

	if ( mtot > total_ )
	{
		size_t mgrow= 1.2 * total_ ;
		if ( mtot > mgrow ) { mgrow= mtot ; }
		unsigned char * pnew = new unsigned char [mgrow] ;
		
		// XX
		std::memcpy(pnew, storage_.get(), use_ ) ;
		storage_.reset( pnew) ;
		total_ = mgrow ;
	}
}

void tank_base::shrink( size_t )
{
}
