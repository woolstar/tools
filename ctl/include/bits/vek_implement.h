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
							vector_ctrl_common<Tb>::locate( new( mem)T(arg... ) ) ;
						}

					using data = vector_ctrl_base::data ;

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
			size_t itemsz= sizeof( __detail::vector_ctrl<T,T> ) ;
			size_t datsz= asize * itemsz * 1.05 ;

				// guess at how large N Base are going to be + ctrl ;
			__detail::vector_base::reserve( datsz, asize) ;
		}

	template <class T>
		template <class Tc, class... Ar>
			typename vector<T>::iterator	vector<T>::emplace_back( Ar&&... arg )
			{
				using ctrl = __detail::vector_ctrl<Tc, T> ;
				size_t xsize = sizeof( ctrl ) ;
				ctrl * rec ;
				data * dcur ;

				reserve( xsize ) ;
			}

	// internal 

	template <class T>
		void	vector<T>::relocate( data * dstart, data *dlimit, data * zdest )
		{
			std::memmove(zdest, dstart, dlimit - dstart) ;
		}

} ;

#endif

