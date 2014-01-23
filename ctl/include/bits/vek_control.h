// vek_control.h -*- C++ -*-

#ifndef _CTL_DETAIL_VECTORCTRL
#define _CTL_DETAIL_VECTORCTRL	1

namespace ctl
{
	namespace __detail
	{
		struct vector_ctrl_base
		{
			constexpr vector_ctrl_base(unsigned int afull) : full_( afull) { }
			unsigned int	base_ = 0, full_ ;

			using data = vector_base::data ;

			virtual void	destroy(void) = 0 ;
			virtual void	move(data * zstorage) = 0 ;
			virtual void	trace(void) const = 0 ;

			protected:
				void	locate( unsigned int abase) { base_ = abase ; }
		} ;

		template < typename T > struct vector_ctrl_common : public vector_ctrl_base
		{
			vector_ctrl_common(unsigned int aoff) : vector_ctrl_base( aoff) { }

			T *	contain() noexcept { return reinterpret_cast<T *>( ((vector_base::data *) this ) + base_ ) ; }

			protected:
				void	locate( const T * aptr )
						{ vector_ctrl_base::locate( (long) aptr - (long) this ) ; }
		} ;

	} ;
} ;

#endif

