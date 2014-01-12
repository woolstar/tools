// tank_implement.h -*- C++ -*-

#ifndef _CTL_TANKIMPL
#define _CTL_TANKIMPL	1

#include <io>
#include <typeinfo>

	using btl::ioerr ;
	using btl::format ;

namespace ctl
{

	namespace __detail
	{
		template <class T, class Tbase>
			class tank_ctrl : public tank_ctrl_common<Tbase>
			{
				public:
					tank_ctrl(tank_ctrl &&) ;
					template<class... Args>
						tank_ctrl( Args&&... arg ) : tank_ctrl()
						{
							void * mem= & storage_ ;
							new( mem)T(arg... ) ;
						}

					using data = tank_ctrl_base::data ;

					void move(data * zstorage) override { }
					void destroy(void) override { ptr()->~T() ; }
					void trace(void) const override
					{
						T * p ;
						Tbase * bp ;
						ioerr << "T - tnk+ctrl, "
							<< " (type) " << typeid( p).name()
							<< " (base type) " << typeid( bp).name()
							<< format(":: sz= %d, ", tank_ctrl_base::size_ ) << format(" off= %d ", tank_ctrl_base::offset_ )
							<< ".\n" ;
					}

				private:
					tank_ctrl() : tank_ctrl_common<Tbase>( sizeof(this), (long) & storage_ - (long) this ) { }

					T *	ptr(void) noexcept
						{ return static_cast<T *>( static_cast<void *>( & storage_ )) ; }

					typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type storage_ ;
			} ;
	} ;

	// implementation
	// lots of ugly

	template <class T>
		template <class Tc, class... Args>
			void tank<T>::emplace_back( Args&&... arg )
			{
				using ctrl = __detail::tank_ctrl<Tc,T> ;
				size_t xsize = sizeof( ctrl) ;
				data * abuf ;
				ctrl * rec ;

				reserve( xsize) ;
				new(storage_.get() + use_ ) ctrl( arg... ) ;
				use( xsize) ;
			}

} ;

#endif

