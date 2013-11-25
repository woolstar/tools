#include <io>

using btl::text_scanf ;
using btl::packet_scanf ;

	// text eol detecter

bool	text_scanf::scan( build_if & acur, scanner<> & ascan )
{
	char c ;

	while ( ascan )
	{
		c= ** ascan, ++ ascan ;

		if (( c <= kEOLMax ) && ( eolv_ & ( 1 << c )))
		{
			if ( ! curv_ || ( curv_ & ( 1 << c ))) { curv_= ( 1 << c ) ;  return true ; }
				else { curv_ |= ( 1 << c ) ;  continue ; }
		}

		curv_= 0 ;
		acur.add( buffer( & c, 1 )) ;
	}

	return false ;
}

	// packet framer

void	packet_scanf::reset()
{
	in_header= true ;
}


