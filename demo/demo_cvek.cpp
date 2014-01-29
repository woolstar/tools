#include <polyco>
#include <io>
#include <iterator>

using btl::ioout ;
using btl::ioerr ;

#include "check.cpp"
#include "ctltypes.h"

	template<typename T> T const& as_const( T& t ) { return t ; }	// decorate with constness

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

	show( test) ;  ioerr << "\n---------\n" ;
	{
		auto ix= test.begin() ;
		ix += 2 ;
		test.erase( ix ) ;
	}

	show( test) ;  ioerr << "\n---------\n" ;
	ctest( test ) ;  ioerr << "\n" ;

	test << test1( 25) << test1( 26) << test1(30) << test1(32) << test1( 64) ;
	show( test) ;  ioerr << "\n---------\n" ;

	{
		auto ix= test.cbegin() ;
		ix += 2 ;
		test.erase( ix, ix + 3 ) ;
	}
	show( test) ;  ioerr << "\n---------\n" ;

	test << test1( 100) << test1( 101) << test1( 102)  ;
	show( test) ;  ioerr << "\n---------\n" ;

	{
		auto ir= test.span() ;
		ir += 5 ;

		for ( const auto & x : ir.remain() ) { x.show() ; }  ioerr << "\n\n" ;
		test.erase( ir.remain() ) ;
	}
	show( test) ;  ioerr << "\n---------\n" ;

	fprintf(stderr, "passed %d tests.\n\n", _passed) ;
	ctl::vector<Base>::dotrace() ;

	return 0 ;
}

