#include <io>

using btl::text_scanf ;
using btl::packet_scanf ;

	// text eol detecter

btl::ScanAction	text_scanf::scan( const buffer & acur, scanner<> ascan, size_t & use )
{
	char c ;

	use = 0 ;

	while ( ascan )
	{
		c= ** ascan ;

		if (( c <= kEOLMax ) && ( eolv_ & ( 1 << c )))
		{
			if ( use ) { return eData ; }

			++ use, ++ ascan ;
			if ( acur.size() ) { return eComplete ; }
			if ( ! curv_ || ( curv_ & ( 1 << c ))) { curv_= ( 1 << c ) ;  return eComplete ; }
				else { curv_ |= ( 1 << c ) ;  return eNone ; }
		}

		++ ascan, ++ use ;  curv_= 0 ;
	}

	return eData ;
}

	// packet framer

void	packet_scanf::reset()
{
	in_header= true ;
}


