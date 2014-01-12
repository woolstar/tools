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
				typedef unsigned char	data ;

				tank_base() { }
				tank_base(size_t asize) : storage_( new data[ asize ] ), total_( asize ) { }

				~ tank_base() ;

				void	reserve(size_t) ;
				void	shrink(size_t) ;

			protected:
				void	use(size_t asz) noexcept
						{ if ( ( use_ + asz ) <= total_ ) { use_ += asz ; } }

					// storage
				size_t	use_ = 0, total_ = 0 ;
				std::unique_ptr<data []>	storage_ ;
		} ;
	} ;
} ;

#endif

