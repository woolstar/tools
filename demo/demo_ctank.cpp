#include <polyco>
#include <io>

using btl::ioout ;
using btl::ioerr ;

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

	Base:: ~ Base() { }

int main()
{
	ctl::tank<Base>	test(1024) ;

	test.emplace_back<test1>(10) ;

	return 0 ;
}

