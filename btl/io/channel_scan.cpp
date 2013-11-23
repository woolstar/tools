#include <io>

using btl::text_scanf ;
using btl::packet_scanf ;

	// text eol detecter

size_t	text_scanf::scan( const buffer & acur, scanner<> ascan )
{
	char c ;
	size_t use = 0 ;

	while ( ascan )
	{
		c= ** ascan ;

ioerr << "char " << btl::format("%c", c) << btl::format(" (%02x)", c) << btl::format("[ %08x]", eolv_) << ".\n" ;
		if (( c <= kEOLMax ) && ( eolv_ & ( 1 << c )))
		{
			if ( acur.size() || use) { return use ; }

			++ ascan ;
			if ( ! curv_ || ( curv_ & ( 1 << c ))) { curv_= ( 1 << c ) ;  return 0 ; }
				else { curv_ |= ( 1 << c ) ;  continue ; }
		}

		++ ascan, ++ use ;  curv_= 0 ;
	}

	return use ;
}

	// packet framer

void	packet_scanf::reset()
{
	in_header= true ;
}


