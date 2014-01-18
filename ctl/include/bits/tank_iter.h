// tank_iter.h -*- C++ -*-

#ifndef _CTL_TANKITER
#define _CTL_TANKITER	1

namespace ctl
{

	template <class T>
		class tank<T>::iterator : public __detail::tank_iter_b
		{
			public:
				constexpr iterator(data * aptr ) noexcept : __detail::tank_iter_b::tank_iter_b( aptr )
				{ }

				operator const_iterator() const { return const_iterator( ptr_) ; }

				iterator	operator++() noexcept { step() ;  return * this ; }
				iterator	operator++(int) { iterator itmp( * this ) ;  step() ;  return itmp ; }

				T &	operator*() const noexcept
								{
									auto rec= reinterpret_cast<__detail::tank_ctrl_common<T> *>( ptr_ ) ;
									return * ( rec-> contain() ) ;
								}
				T *	operator->() const noexcept
								{
									auto rec= reinterpret_cast<__detail::tank_ctrl_common<T> *>( ptr_ ) ;
									return rec-> contain() ;
								}
		} ;

	template <class T>
		class tank<T>::const_iterator : public __detail::tank_iter_b
		{
			public:
				constexpr const_iterator(data * aptr ) noexcept : __detail::tank_iter_b::tank_iter_b( aptr )
				{ }

				const_iterator	operator++() noexcept { step() ;  return * this ; }
				const_iterator	operator++(int) { const_iterator itmp( * this ) ;  step() ;  return itmp ; }

				const T &	operator*() const noexcept
								{
									auto rec= reinterpret_cast<__detail::tank_ctrl_common<T> *>( ptr_ ) ;
									return * ( rec-> contain() ) ;
								}
				const T *	operator->() const noexcept
								{
									auto rec= reinterpret_cast<__detail::tank_ctrl_common<T> *>( ptr_ ) ;
									return rec-> contain() ;
								}

			protected:
				data *	location(void) const noexcept { return ptr_ ; }
				friend class tank<T> ;
		} ;

	template <class T>
		class tank<T>::range : public __detail::tank_range_b
		{
			public:
				constexpr range(data * aptr, data * alim ) noexcept : __detail::tank_range_b::tank_range_b( aptr, alim )
				{ } 

				range	operator++() noexcept { step() ;  return * this ; }
				range	operator++(int) { range rtmp( * this ) ;  step() ;  return rtmp ; }

				operator const_range() const { return const_range( ptr_, limit_ ) ; }
				operator const_iterator() const { return const_iterator( ptr_ ) ; }

				T &	operator*() const noexcept
								{
									auto rec= reinterpret_cast<__detail::tank_ctrl_common<T> *>( ptr_ ) ;
									return * ( rec-> contain() ) ;
								}
				T *	operator->() const noexcept
								{
									auto rec= reinterpret_cast<__detail::tank_ctrl_common<T> *>( ptr_ ) ;
									return rec-> contain() ;
								}
		} ;

	template <class T>
		class tank<T>::const_range : public __detail::tank_range_b
		{
			public:
				constexpr const_range(data * aptr, data * alim ) noexcept : __detail::tank_range_b::tank_range_b( aptr, alim )
				{ } 
				constexpr const_range(const_iterator & afir, const_iterator & alim ) noexcept
					: __detail::tank_range_b::tank_range_b( afir.location(), alim.location() )
				{ }

				const_range	operator++() noexcept { step() ;  return * this ; }
				const_range	operator++(int) { const_range rtmp( * this ) ;  step() ;  return rtmp ; }

				const T &	operator*() const noexcept
								{
									auto rec= reinterpret_cast<__detail::tank_ctrl_common<T> *>( ptr_ ) ;
									return * ( rec-> contain() ) ;
								}
				const T *	operator->() const noexcept
								{
									auto rec= reinterpret_cast<__detail::tank_ctrl_common<T> *>( ptr_ ) ;
									return rec-> contain() ;
								}

			protected:
				data *	location(void) const noexcept { return ptr_ ; }
				friend class tank<T> ;
		} ;

} ;

#endif

