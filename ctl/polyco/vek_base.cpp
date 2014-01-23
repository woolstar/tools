#include <polyco>
#include <cstring>

using ctl::__detail::vector_base ;

	bool vector_base::strace = false ;


vector_base:: ~ vector_base()
{
	clear() ;
}

void	vector_base::clear( void )
{
}

void vector_base::shrink_fit( void )
{
}

