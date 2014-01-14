// tank_control.h -*- C++ -*-

#ifndef _CTL_DETAIL_TANKCTRL
#define _CTL_DETAIL_TANKCTRL	1

namespace ctl
{
	namespace __detail
	{
		struct tank_ctrl_base
		{
			tank_ctrl_base(size_t asz, unsigned int afull) : size_( asz), full_( afull) { }
			size_t	size_ ;
			unsigned int	base_ = 0, full_ ;

			using data = tank_base::data ;

			virtual void	destroy(void) = 0 ;
			virtual void	move(data * zstorage) = 0 ;
			virtual void	trace(void) const = 0 ;

			protected:
				void	locate( unsigned int abase) { base_ = abase ; }
		} ;

		template < typename T > struct tank_ctrl_common : public tank_ctrl_base
		{
			tank_ctrl_common(unsigned int asz, unsigned int aoff) : tank_ctrl_base( asz, aoff) { }

			T *	operator()() noexcept { return reinterpret_cast<T *>( ((tank_base::data *) this ) + base_ ) ; }

			protected:
				void	locate( const T * aptr )
						{ tank_ctrl_base::locate( (long) aptr - (long) this ) ; }
		} ;

	} ;
} ;

#endif

