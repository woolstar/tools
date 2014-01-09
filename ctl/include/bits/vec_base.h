// vec_base -*- C++ -*-

#ifndef	_CUDETAIL_VEC_BASE
#define _CUDETAIL_VEC_BASE

#include <memory>

namespace ctl
{
	namespace __detail
	{
		class vector_base
		{
			public:
				vector_base() { }
				vector_base(size_t asize) : storage_( new unsigned char[ asize ] ), total_( asize ) { }

				void	size(size_t) ;
				void	shrink(size_t) ;

					// storage
				size_t	use_ = 0, total_ = 0 ;
				std::unique_ptr<unsigned char []>	storage_ ;
		} ;
	} ;
} ;

#endif

