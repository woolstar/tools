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

				vector_base() : offsets_ { 0 } { }
				~ vector_base() ;

				void	clear( void) ;

				static void	dotrace( bool atrace = true ) { strace= atrace ; }

				typedef	std::vector<std::ptrdiff_t>	off_t ;

			protected:

				virtual void relocate( data * asrc, data * alim, data * zdest) = 0 ;

				void	reserve(size_t adat, size_t anum) ;
				void	shrink(size_t) ;

					// storage
				size_t	use_ = 0, total_ = 0 ;
				std::unique_ptr<data []>	storage_ ;
				std::vector<std::ptrdiff_t>	offsets_ ;

			private:
				static bool	strace ;
		} ;

		class	vector_iter_b
		{
			using data = vector_base::data ;
			using off_t = vector_base::off_t ;

			public:
				constexpr vector_iter_b( data * dptr, off_t::iterator itr ) noexcept : ptr_( dptr), it_( itr ) { }

				bool	operator==(vector_iter_b & ai ) const noexcept { return ai.ptr_ == ptr_ ; }
				bool	operator==(const vector_iter_b & ai ) const noexcept { return ai.ptr_ == ptr_ ; }

				typedef	std::ptrdiff_t					difference_type ;
				typedef std::random_access_iterator_tag	iterator_category ;

			protected:

				void	step( void) noexcept ;
				void	back( void) noexcept ;
				void	jump( int aoff ) noexcept ;

				data	* ptr_ ;
				off_t::iterator	 it_ ;
		} ;

	} ;
} ;

#endif

