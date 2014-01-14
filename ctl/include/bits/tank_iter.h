// tank_iter.h -*- C++ -*-

#ifndef _CTL_TANKITER
#define _CTL_TANKITER	1

namespace ctl
{

	template<class T>
		class tank<T>::iterator : public __detail::tank_iter_b
		{
			public:
				constexpr iterator(data * aptr ) noexcept : __detail::tank_iter_b::tank_iter_b( aptr )
				{ }

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

} ;

#endif

