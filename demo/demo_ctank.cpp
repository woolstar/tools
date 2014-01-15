#include <polyco>
#include <io>

using btl::ioout ;
using btl::ioerr ;

#include "check.cpp"

	class	Base
	{
		public:
			Base() { ioerr << "C base\n" ; }

			virtual ~ Base() = 0 ;
			virtual void  action() = 0 ;
			virtual void  show() = 0 ;
	} ;

	class	test1 : public Base
	{
		public:
			test1(int aval) : m_val( aval) { ioerr << "C test1 <int>\n" ; }
			~ test1() { ioerr << "D test1\n" ; }

			void	action() override { m_val ++ ; }
			void	show() override { ioout << btl::format("[t1:show] %8d\n", m_val) ; }

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
			~ test2() { ioerr << "D test2\n" ; }

			void	action() override { }
			void	show() override { ioout << "[t2:show] m_code : " << m_code << "\n" ; }

		private:
			char m_code[10] ;
	} ;

	class fat
	{
		public:
			fat(const char * astr) { strncpy(str_, astr, sizeof( str_ ) ) ; }
			~ fat() { ioerr << "D fat\n" ; }

			char str_[128] ;
	} ;

	class test3 : public Base, public fat
	{
		public:
			test3( int aval, const char * ptr ) : fat( ptr), num_( aval) { }
			~ test3() { ioerr << "D test3\n" ; }

			void	action() override { }
			void	show() override { ioout << "[t3] n, fat " << str_ << "\n" ; }

			int	num_ ;
	} ;

	Base:: ~ Base() { ioerr << "D base\n" ;  }

	////

	void show( const ctl::tank<Base> & atk ) { for ( auto & br : atk ) { br.show() ; } }

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

	for ( auto spn= test.span() ; ( spn ) ; ++ spn ) { spn-> action() ; }
	show( test) ;

	test.clear() ;

	test.transfer_back( test1( 30) ) ;
	test << test2( "str magic" ) << test2( "and more" ) ;

	show( test) ;

	fprintf(stderr, "passed %d tests.\n", _passed) ;
	return 0 ;
}

