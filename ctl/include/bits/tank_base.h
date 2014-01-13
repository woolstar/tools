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

		class	tank_iter_b
		{
			public:
				typedef unsigned char	data ;

				tank_iter_b( data * aloc ) : ptr_( aloc ) { }

			protected:

				void	step( void) ;
				// void	back( void) ;

				data	* ptr_ ;
		} ;

		class	tank_range_b
		{
			private:
				class	IsValid { void operator delete(void *) ; } ;

			public:
				typedef unsigned char	data ;

				tank_range_b( data * aptr, data * alim ) : ptr_( aptr ), limit_( alim ) { }

				operator const IsValid *() const {
					static IsValid _sentinal ;
					return (( limit_ != ptr_ ) && ( nullptr != ptr_ )) ? & _sentinal : nullptr ;
				}

				bool	operator==(tank_range_b & arng) const noexcept { return arng.ptr_ == ptr_ ; }
				bool	operator!=(tank_range_b & arng) const noexcept { return arng.ptr_ != ptr_ ; }
				bool	operator==(const tank_range_b & arng) const noexcept { return arng.ptr_ == ptr_ ; }
				bool	operator!=(const tank_range_b & arng) const noexcept { return arng.ptr_ != ptr_ ; }

			protected:

				void	step( void) ;

				data	* ptr_ ;
				data	* const limit_ ;
		} ;

	} ;
} ;

#endif

