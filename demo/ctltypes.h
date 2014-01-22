
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

