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

				void	reserve(size_t adat) ;
				void	reserve(size_t adat, size_t anum) { reserve( adat) ;  offsets_.reserve( anum) ; }
				void	use( size_t asz ) noexcept { if ( ( asz + use_ ) > total_ ) { use_= total_ ; } else { use_ += asz ; } }
				void	reduce( size_t asz ) noexcept { if ( asz > use_ ) { use_= 0 ; } else { use_ -= asz ; } }
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
			protected:
				using data = vector_base::data ;
				using off_t = vector_base::off_t ;

			public:
				constexpr vector_iter_b( data * const dbase, off_t::const_iterator itr ) noexcept : pbase_( dbase), it_( itr ) { }

				bool	operator==(vector_iter_b & ai ) const noexcept { return ai.it_ == it_ ; }
				bool	operator==(const vector_iter_b & ai ) const noexcept { return ai.it_ == it_ ; }
				bool	operator!=(vector_iter_b & ai ) const noexcept { return ai.it_ != it_ ; }
				bool	operator!=(const vector_iter_b & ai ) const noexcept { return ai.it_ != it_ ; }

				bool	operator<(vector_iter_b & ai) const noexcept { return it_ < ai.it_ ; }
				bool	operator<=(vector_iter_b & ai) const noexcept { return it_ <= ai.it_ ; }
				bool	operator>(vector_iter_b & ai) const noexcept { return it_ > ai.it_ ; }
				bool	operator>=(vector_iter_b & ai) const noexcept { return it_ >= ai.it_ ; }

				typedef	std::ptrdiff_t					difference_type ;
				typedef std::random_access_iterator_tag	iterator_category ;

			protected:

				void	step( void) noexcept { ++ it_ ; }
				void	back( void) noexcept { -- it_ ; }
				void	jump( int aoff ) noexcept { it_ += aoff ; }

				data * const pbase_ ;
				off_t::const_iterator	 it_ ;
		} ;

		class	vector_range_b
		{
			private:
				class	IsValid { void operator delete(void *) ; } ;

			protected:
				using data = vector_base::data ;
				using off_t = vector_base::off_t ;

			public:
				vector_range_b( data * const dbase, const off_t & off )
					: pbase_( dbase), it_( off.begin() ), itb_( off.begin() ), ite_( off.end() )
					{ }

				operator const IsValid *() const noexcept {
					static IsValid _sentinal ;
					return (it_ != ite_ ) ? & _sentinal : nullptr ;
				}

				bool	operator==(vector_range_b & ai ) const noexcept { return ai.it_ == it_ ; }
				bool	operator==(const vector_range_b & ai ) const noexcept { return ai.it_ == it_ ; }

			protected:

				void	step( void) noexcept ;
				void	back( void) noexcept ;
				void	jump( int aoff ) noexcept ;

				data	* const pbase_ ;

				off_t::const_iterator			it_ ;
				const off_t::const_iterator	itb_, ite_ ;

		} ;
	} ;
} ;

#endif

