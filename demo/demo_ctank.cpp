#include <polyco>
#include <io>
#include <iterator>

using btl::ioout ;
using btl::ioerr ;

#include "check.cpp"

	class	Base
	{
		public:
			Base() { ioerr << "C base\n" ; }

			virtual ~ Base() = 0 ;
			virtual void  action() = 0 ;
			virtual void  show() const = 0 ;
	} ;

	class	test1 : public Base
	{
		public:
			test1(int aval) : m_val( aval) { ioerr << "C test1 <int>\n" ; }
			~ test1() { ioerr << "D test1, " ; }

			void	action() override { m_val ++ ; }
			void	show() const override { ioout << btl::format("[t1:show] %8d\n", m_val) ; }

			int	m_val ;
	} ;

	class	test2 : public Base
	{
		public:
			test2( const char * acode )
			{
				strncpy(m_code, acode, sizeof( m_code)) ;
				ioerr << "C test2 <char*>\n" ;
			}
			~ test2() { ioerr << "D test2, " ; }

			void	action() override { }
			void	show() const override { ioout << "[t2:show] m_code : " << m_code << "\n" ; }

		private:
			char m_code[10] ;
	} ;

	class fat
	{
		public:
			fat(const char * astr) { strncpy(str_, astr, sizeof( str_ ) ) ; }
			~ fat() { ioerr << "D fat, " ; }

			char str_[128] ;
	} ;

	class test3 : public Base, public fat
	{
		public:
			test3( int aval, const char * ptr ) : fat( ptr), num_( aval) { }
			~ test3() { ioerr << "D test3, " ; }

			void	action() override { }
			void	show() const override { ioout << "[t3] n, fat " << str_ << "\n" ; }

			int	num_ ;
	} ;

	Base:: ~ Base() { ioerr << "D base.\n" ;  }

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

