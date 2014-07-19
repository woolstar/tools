// list_base -*- C++ -*-

#ifndef	_CUDETAIL_LIST_BASE
#define _CUDETAIL_LIST_BASE

#include <memory>
#include <iterator>
#include <cstddef>

namespace ctl
{
	namespace __detail
	{
		class list_base
		{
			public:
				typedef unsigned char	data ;

				list_base() { }

				~ list_base() ;

				void	clear( void) ;

				bool empty() const noexcept { return false ; }
		} ;
	}
}

#endif

