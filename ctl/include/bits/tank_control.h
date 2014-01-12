// tank_control.h -*- C++ -*-

#ifndef _CTL_DETAIL_TANKCTRL
#define _CTL_DETAIL_TANKCTRL	1

namespace ctl
{
	namespace __detail
	{
		struct tank_ctrl_base
		{
			tank_ctrl_base(unsigned int asz, unsigned int aoff) : size_( asz), offset_( aoff) { }
			unsigned int	size_, offset_ ;

			using data = tank_base::data ;

			virtual void	destroy(void) = 0 ;
			virtual void	move(data * zstorage) = 0 ;
			virtual void	trace(void) const = 0 ;
		} ;

		template < typename T > struct tank_ctrl_common : public tank_ctrl_base
		{
			tank_ctrl_common(unsigned int asz, unsigned int aoff) : tank_ctrl_base( asz, aoff) { }

			T *	operator()() noexcept { return reinterpret_cast<T *>( ((tank_base::data *) this ) + offset_ ) ; }
		} ;

			// for bidirectional blob
		template < typename T > struct tank_ctrl_tail
		{
			unsigned int	size_, offset_ ;

			T *	operator()() noexcept { return reinterpret_cast<T *>( ((tank_base::data *) this ) - offset_ ) ; }
		} ;

	} ;
} ;

#endif

