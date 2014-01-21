// vector_base -*- C++ -*-

#ifndef	_CUDETAIL_VECTOR_BASE
#define _CUDETAIL_VECTOR_BASE

#include <memory>
#include <iterator>
#include <cstddef>

namespace ctl
{
	namespace __detail
	{
		class vector_base
		{
			public:
				typedef unsigned char	data ;

				vector_base() { }
				~ vector_base() ;

				void	clear( void) ;
				void	reserve(size_t) ;
				void	fit( void ) ;

				static void	dotrace( bool atrace = true ) { strace= atrace ; }

			protected:

				virtual void relocate( data * asrc, data * alim, data * zdest) = 0 ;

					// storage
				size_t	use_ = 0, total_ = 0 ;
				std::unique_ptr<data []>	storage_ ;
				std::vector<std::ptrdiff_t>	offsets_ ;

			private:
				static bool	strace ;
		} ;

	} ;
} ;

#endif

