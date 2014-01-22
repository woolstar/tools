#include <polyco>
#include <io>
#include <iterator>

using btl::ioout ;
using btl::ioerr ;

#include "check.cpp"
#include "ctltypes.h"

	////

	void show( const ctl::tank<Base> & atk ) { for ( const auto & br : atk ) { br.show() ; } }

	void ctest( const ctl::tank<Base> & atk )
	{
		auto ir= atk.span() ;

		if ( ir ) { ir-> show() ; }
	}

int main()
{
	ctl::tank<Base>	test(1024) ;

	check( test.empty(), true ) ; 
	check( test.capacity(), 1024 ) ;

	test.emplace_back<test1>(10) ;
	test.emplace_back<test2>("Sample") ;
	test.emplace_back<test1>(20) ;
	test.emplace_back<test3>(50, "no good deed un punished") ;

	check( ! test.size(), false ) ;

	ioerr << "First in line: " ;  test.front().show() ;
	{
		Base & rbase= test.at( 2) ;
		ioerr << ".. at slot 2: " ;  rbase.show() ;
	}

	for ( auto spn= test.span() ; ( spn ) ; ++ spn ) { spn-> action() ; }
	show( test) ;
	ioerr << "---------\n" ;

	{
		auto ptr= test.begin() ;
		++ ptr ;
		++ ptr ;
		test.emplace<test1>(ptr, 40) ;
	}
	show( test) ;
	ioerr << "---------\n" ;

	test.clear() ;

	test.transfer_back( test1( 30) ) ;
	test << test2( "str magic" ) << test2( "and more" ) ;
	test << test1(100) << test1(100) << test1(100) << test1(100) << test1(100) << test1(100) ;
	{
		auto itr= test.begin() ;
		std::advance( itr, 3 ) ;
		itr= test.erase( itr) ;
		itr-> action() ;
		std::advance( itr, 2 ) ;
		test.erase( itr, test.end() ) ;
	}

	show( test) ;
	ioerr << "---------\n" ;

	fprintf(stderr, "passed %d tests.\n", _passed) ;
	ctl::tank<Base>::dotrace() ;

	return 0 ;
}

