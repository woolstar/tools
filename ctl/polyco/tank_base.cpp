#include <polyco>
#include <cstring>

using ctl::__detail::tank_base ;

tank_base:: ~ tank_base()
{
	clear() ;
}

void	tank_base::clear( void )
{
	data * dptr, * dlimit ;
	tank_ctrl_base * ctrptr ;

	dptr= storage_.get() ;
	dlimit= dptr + use_ ;

	use_= 0 ;
	for ( ; ( dptr < dlimit ) ; dptr += ctrptr-> size_ )
	{
		ctrptr= ( tank_ctrl_base *) dptr ;
		ctrptr-> trace() ;
		ctrptr-> destroy() ;
	}
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

