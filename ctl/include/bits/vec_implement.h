// vec_implement.h -*- C++ -*-

#ifndef _CTL_VECIMPL
#define _CTL_VECIMPL	1

namespace ctl
{

	namespace __detail
	{
		template <class T, class Tbase>
			class vector_ctrl : public vector_ctrl_base<Tbase>
			{
				public:
					vector_ctrl() { }
					vector_ctrl(vector_ctrl &&) ;
					template<class... Args>
						vector_ctrl( Args&&... arg ) { }

					void move(unsigned char * zstorage) ;
					void destroy(void) ;

					const T *	ptr(void) const { return static_cast<T *>( & storage_ ) ; }

				private:
					typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type storage_ ;
			} ;
	} ;

	// implementation
	// lots of ugly

	template <class T>
		template <class Tc, class... Args>
			void vector<T>::emplace_back( Args&&... arg )
			{
				using ctrl = __detail::vector_ctrl<Tc,T> ;
				size_t xsize = sizeof( ctrl) + sizeof( __detail::vector_ctrl_tail<T>) ;
				unsigned char * abuf ;

				reserve( xsize) ;
				abuf= storage_.get() + use_ ;
			}

} ;

#endif

