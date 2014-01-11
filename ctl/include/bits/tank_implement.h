// tank_implement.h -*- C++ -*-

#ifndef _CTL_TANKIMPL
#define _CTL_TANKIMPL	1

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
						tank_ctrl( Args&&... arg )
						{
							void * mem= & storage_ ;
							new( mem)T(arg... ) ;
						}

					void move(unsigned char * zstorage) { }
					void destroy(void) 
					{
						T * ptr= static_cast< T *>( static_cast<void *>( & storage_ ) ) ;
						ptr->~T() ;
					}

					const T *	ptr(void) const { return static_cast<T *>( & storage_ ) ; }

				private:
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
				size_t xsize = sizeof( ctrl) + sizeof( __detail::tank_ctrl_tail<T>) ;
				unsigned char * abuf ;
				ctrl * rec ;
				Tc * ptr ;

				reserve( xsize) ;
				abuf= storage_.get() + use_ ;
				rec= new(abuf) ctrl( arg... ) ;
			}

} ;

#endif

