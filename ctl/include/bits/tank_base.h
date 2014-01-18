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

				void	clear( void) ;
				void	reserve(size_t) ;
				void	shrink(size_t) ;

				static void	dotrace( bool atrace = true ) { strace= atrace ; }

			protected:
				void	use(size_t asz) noexcept
						{ if ( ( use_ + asz ) <= total_ ) { use_ += asz ; } }
				void	reduce(size_t asz) noexcept
						{ if ( asz > use_ ) { use_= 0 ; } else { use_ -= asz ; } }

				virtual void relocate( data * asrc, data * alim, data * zdest) = 0 ;

					// storage
				size_t	use_ = 0, total_ = 0 ;
				std::unique_ptr<data []>	storage_ ;

			private:
				static bool	strace ;
		} ;

		class	tank_iter_b
		{
			public:
				typedef unsigned char	data ;

				constexpr tank_iter_b( data * aloc ) noexcept : ptr_( aloc ) { }

				bool	operator==(tank_iter_b & ai ) const noexcept { return ai.ptr_ == ptr_ ; }
				bool	operator!=(tank_iter_b & ai ) const noexcept { return ai.ptr_ != ptr_ ; }
				bool	operator==(const tank_iter_b & ai ) const noexcept { return ai.ptr_ == ptr_ ; }
				bool	operator!=(const tank_iter_b & ai ) const noexcept { return ai.ptr_ != ptr_ ; }

			protected:

				void	step( void) noexcept ;
				// void	back( void) ;

				data	* ptr_ ;
		} ;

		class	tank_range_b
		{
			private:
				class	IsValid { void operator delete(void *) ; } ;

			public:
				typedef unsigned char	data ;

				constexpr tank_range_b( data * aptr, data * alim ) noexcept : ptr_( aptr ), limit_( alim ) { }

				operator const IsValid *() const {
					static IsValid _sentinal ;
					return (( limit_ != ptr_ ) && ( nullptr != ptr_ )) ? & _sentinal : nullptr ;
				}

				bool	operator==(tank_range_b & arng) const noexcept { return arng.ptr_ == ptr_ ; }
				bool	operator!=(tank_range_b & arng) const noexcept { return arng.ptr_ != ptr_ ; }
				bool	operator==(const tank_range_b & arng) const noexcept { return arng.ptr_ == ptr_ ; }
				bool	operator!=(const tank_range_b & arng) const noexcept { return arng.ptr_ != ptr_ ; }

			protected:

				void	step( void) noexcept ;

				data	* ptr_ ;
				data	* const limit_ ;
		} ;

	} ;
} ;

#endif

