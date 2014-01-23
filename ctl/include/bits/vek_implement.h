// vek_implement.h -*- C++ -*-

#ifndef _CTL_VECTORIMPL
#define _CTL_VECTORIMPL	1

#include <typeinfo>
#include <utility>
#include <cstring>

namespace ctl
{
	namespace __detail
	{
		template <class T, class Tb>
			class vector_ctrl : public vector_ctrl_common<Tb>
			{
				public:
					vector_ctrl( vector_ctrl && ) ;
					template <class... Ar>
						vector_ctrl( Ar&&... arg ) : vector_ctrl()
						{
							void * mem= & storage_ ;
							tank_ctrl_common<Tb>::locate( new( mem)T(arg... ) ) ;
						}

					using data = tank_ctrl_base::data ;

					void move( data * zstorage ) override { }
					void destroy(void) override { ptr()-> ~ T() ; }
					void trace(void) const override
					{
					} ;

				protected:
					vector_ctrl() : vector_ctrl_common<Tb>( (long) &storage_ - (long) this ) { }

					friend class vector<Tb> ;

				private:
					T * ptr(void) noexcept
						{ return static_cast<T *>( (void *) &storage_ ) ; }

					typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type
						storage_ ;
			} ;
	} ;

	template <class T>
		vector<T>::vector(size_t asize)
		{
			// guess at how large N Base are going to be + ctrl ;
		}

	// internal 

	template <class T>
		void	vector<T>::relocate( data * dstart, data *dlimit, data * zdest )
		{
			std::memmove(zdest, dstart, dlimit - dstart) ;
		}

} ;

#endif

