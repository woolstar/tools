#include <polyco>
#include <io>
#include <iterator>

using btl::ioout ;
using btl::ioerr ;

#include "check.cpp"
#include "ctltypes.h"

	////

/*
	void show( const ctl::vector<Base> & atk ) { for ( const auto & br : atk ) { br.show() ; } }

	void ctest( const ctl::vector<Base> & atk )
	{
		auto ir= atk.span() ;

		if ( ir ) { ir-> show() ; }
	}
*/

int main()
{
	ctl::vector<Base>	test(1024) ;

	check( test.empty(), true ) ; 
	check( test.capacity(), 1023 ) ;

	test.emplace_back<test1>( 10) ;

	ioerr << "---------\n" ;

	fprintf(stderr, "passed %d tests.\n\n", _passed) ;
	ctl::vector<Base>::dotrace() ;

	return 0 ;
}

