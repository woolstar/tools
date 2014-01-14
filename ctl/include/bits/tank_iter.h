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
		} ;

} ;

#endif

