// tank_base -*- C++ -*-

#ifndef	_CUDETAIL_TANK_BASE
#define _CUDETAIL_TANK_BASE

#include <memory>

namespace ctl
{
	namespace __detail
	{
		class tank_base
		{
			public:
				tank_base() { }
				tank_base(size_t asize) : storage_( new unsigned char[ asize ] ), total_( asize ) { }

				~ tank_base() ;

				void	reserve(size_t) ;
				void	shrink(size_t) ;

					// storage
				size_t	use_ = 0, total_ = 0 ;
				std::unique_ptr<unsigned char []>	storage_ ;
		} ;
	} ;
} ;

#endif

