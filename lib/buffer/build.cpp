
#include <buffer.h>
#include <buffer_scanner.h>

using	btl::build_base ;
using	btl::scanner ;

build_base:: ~ build_base() { }

void	build_base::copy(buffer const & abuf)
{
	scanner<sized_storage>	scan( abuf) ;

	while ( scan.size_remaining() && ( fill_ < limit_ ) ) { *( fill_ ++)= * (scan ++ ) ; }
}

