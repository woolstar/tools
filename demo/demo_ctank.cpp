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
			void	show() override { ioout << btl::format("%8d ", m_val) ; }

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
			void	show() override { ioout << "m_code : " << m_code << "\n" ; }

		private:
			char m_code[10] ;
	} ;

	Base:: ~ Base() { ioerr << "D base\n" ;  }

int main()
{
	ctl::tank<Base>	test(1024) ;

	check( test.empty(), true ) ; 
	check( test.capacity(), 1024 ) ;

	test.emplace_back<test1>(10) ;
	test.emplace_back<test2>("Sample") ;
	test.emplace_back<test1>(20) ;

	check( ! test.size(), false ) ;

	auto p= test.begin() ;
	auto pe= test.cend() ;
	while ( p != pe ) { ++ p ; }

	fprintf(stderr, "passed %d tests.\n", _passed) ;
	return 0 ;
}

