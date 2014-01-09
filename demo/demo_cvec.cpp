#include <cupvec>
#include <io>

using btl::ioout ;

	class	Base
	{
		public:
			virtual ~ Base() = 0 ;
			virtual void  action() = 0 ;
			virtual void  show() = 0 ;
	} ;

	class	test1 : public Base
	{
		public:
			test1(int aval) : m_val( aval) { }
			~ test1() ;

			void	action() override { m_val ++ ; }
			void	show() override { ioout << btl::format("%8d ", m_val) ; }

			int	m_val ;
	} ;

int main()
{
	ctl::vector<Base>	test ;

	test.emplace_back<test1>(10) ;

	return 0 ;
}

