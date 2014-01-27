#include <polyco>
#include <io>
#include <iterator>

using btl::ioout ;
using btl::ioerr ;

#include "check.cpp"
#include "ctltypes.h"

	////

	void show( const ctl::vector<Base> & atk ) { for ( const auto & br : atk ) { br.show() ; } }

	void ctest( const ctl::vector<Base> & atk )
	{
		auto ir= atk.span() ;

		if ( ir ) { ir-> show() ; }
	}

int main()
{
	ctl::vector<Base>	test(1024) ;

	check( test.empty(), true ) ; 
	check( test.capacity(), 1023 ) ;

	test.emplace_back<test1>( 10) ;
	test << test1(20) << test1(21) << test1(22) ;
	check( test.size(), 4 ) ;

	auto x= test.begin() ;
	x += 2 ;
	test.erase( x ) ;

	show( test) ;  ioerr << "\n---------\n" ;
	ctest( test ) ;  ioerr << "\n" ;

	fprintf(stderr, "passed %d tests.\n\n", _passed) ;
	ctl::vector<Base>::dotrace() ;

	return 0 ;
}

