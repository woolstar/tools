
#include <stdio.h>
#include <container>
#include <buffer>
#include <algorithm>

	static int	_passed = 0 ;

	template <typename T, typename U>
		static void	check(T va, U vb)
	{
		if ( va != vb ) { 
			fprintf(stderr, "test failed: %d != %d\n", (int) va, (int) vb) ;
			exit( 1) ;
		}

		++ _passed ;
	}

	struct	samplevalue : public btl::safe_list<samplevalue>::listable
	{
		samplevalue(int aval) : val_( aval) {}
		virtual ~ samplevalue() {}
		const int val_ ;

		bool operator==(const samplevalue & aval) const { return aval.val_ == val_ ; }
	} ;

	struct advancedvalue : public samplevalue
	{
		advancedvalue(int aval, const char * alabel)
			: samplevalue( aval), val_( aval), label_( alabel) { total_ ++ ; }
		~ advancedvalue() { total_ -- ; }

		const int val_ ;
		const btl::build_static<16> label_ ;

		static long	total_ ;
	} ;

	long	advancedvalue::total_ = 0 ;

	// test encumbered lists


int	main(int, char **)
{
	btl::safe_list<samplevalue>	testlist ;

	check( testlist.size(), 0) ;

	testlist << new samplevalue( 1) << new samplevalue( 2) << new samplevalue( 3) << new samplevalue( 4) ;

		// size test 
	check(testlist.size(), 4) ;

	{
			// elements iterator access
		auto p= testlist.begin() ;  check( p-> val_, 1) ;
		++ p ;  check( p-> val_, 2) ;
	}
	{
			// reverse iterator
		auto p= testlist.rbegin() ;  check( p-> val_, 4 ) ;
		++ p ;  check( p-> val_, 3 ) ;
	}

		// test types
	{
		auto p= testlist.begin() ;
		check ( ( * p == samplevalue( 1 )), true ) ;
	}

		// algo
	{
		int lookfor = 0 ;
		check( 1, std::count( testlist.begin(), testlist.end(), samplevalue( 3))) ; 

		lookfor= 4 ;
		check( 1, std::count_if( testlist.begin(), testlist.end(), 
					[lookfor](const samplevalue & aelem){ return aelem.val_ == lookfor ; } )) ;

		auto p= std::find_if( testlist.begin(), testlist.end(),
					[](const samplevalue & aelem){ return aelem.val_ == 2 ; } ) ;
		check( p != testlist.end(),  true ) ;
		check( p-> val_, 2 ) ;

		p= std::find( testlist.begin(), testlist.end(), samplevalue( 10)) ;
		check( p == testlist.end(), true ) ;

		lookfor= 10 ;
		p= std::find_if( testlist.begin(), testlist.end(),
					[lookfor](const samplevalue & aelem){ return aelem.val_ == lookfor ; } ) ;
		check( p == testlist.end(),  true ) ;

		int sum= 0 ;
		for ( auto & p : testlist ) { sum += p.val_ ; }
		check( 10, sum) ;
	}

	testlist << new advancedvalue( 21, "winner") << new advancedvalue( 22, "extra") ;
	check( testlist.size(), 6) ;

	{
		auto p= std::find( testlist.begin(), testlist.end(), advancedvalue( 21, "")) ;
		check( p != testlist.end(),  true ) ;
		check( p-> val_, 21) ;
	}

	check( advancedvalue::total_, 2 ) ;

	testlist.clear() ;
	check( testlist.size(), 0) ;
	check( advancedvalue::total_, 0 ) ;

	fprintf(stderr, "passed %d tests.\n", _passed) ;

}

