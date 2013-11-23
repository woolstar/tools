#include <io>

void	btl::channel_if::pass(const buffer & abuf) const
{
	if ( next()) { next()-> data( abuf) ; }
}

	//

void	btl::feeder::data( const buffer & abuf )
{
	auto r( list_.span()) ;

	if ( r ) { r-> data( abuf) ; }
}

