#include <io>

void	btl::channel_if::pass(const buffer & abuf) const
{
ioerr << "In pass\n" ;

	if ( next()) { next()-> data( abuf) ; }
}

	//

void	btl::feeder::data( const buffer & abuf )
{
	auto r( list_.span()) ;

ioerr << "In feeder data.\n" ;

	if ( r ) { r-> data( abuf) ; }
}

