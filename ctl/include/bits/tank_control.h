// tank_control.h -*- C++ -*-

#ifndef _CTL_DETAIL_TANKCTRL
#define _CTL_DETAIL_TANKCTRL	1

namespace ctl
{
	namespace __detail
	{
		struct vector_ctrl_base
		{
			unsigned int	size_, offset_ ;

			virtual void	destroy(void) = 0 ;
			virtual void	move(unsigned char * zstorage) = 0 ;
		} ;

		template < typename T > struct vector_ctrl_common : public vector_ctrl_base
		{
			T *	operator()() noexcept { return reinterpret_cast<T *>( ((unsigned char *) this ) + offset_ ) ; }

		} ;

			// for bidirectional blob
		template < typename T > struct vector_ctrl_tail
		{
			unsigned int	size_, offset_ ;

			T *	operator()() noexcept { return reinterpret_cast<T *>( ((unsigned char *) this ) - offset_ ) ; }
		} ;

	} ;
} ;

#endif

