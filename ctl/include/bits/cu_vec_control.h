// cu_vec_control.h -*- C++ -*-

#ifndef _CTL_DETAIL_VECCTRL
#define _CTL_DETAIL_VECCTRL	1

namespace ctl
{
	namespace __detail
	{
		template < typename T > struct vector_ctrl_base
		{
			unsigned int	size_, offset_ ;

			T *	operator()() noexcept { return reinterpret_cast<T *>( ((unsigned char *) this ) + offset_ ) ; }
		} ;

		template < typename T > struct vector_ctrl_tail
		{
			unsigned int	size_, offset_ ;

			T *	operator()() noexcept { return reinterpret_cast<T *>( ((unsigned char *) this ) - offset_ ) ; }
		} ;

	} ;
} ;

#endif

