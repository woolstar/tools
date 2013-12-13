#ifndef BTL_BUFFER_
#define BTL_BUFFER_ 1

#include <string>

namespace btl
{
	class	buffer
	{
		public:
			buffer() { }

			buffer(void const * aptr, void const * afar ) : rawbuffer_(aptr), far_( afar ) { }

			buffer(const std::string & astr ) : rawbuffer_( astr.c_str() ), far_( astr.c_str() + astr.length() ) { }
			buffer(char const * astr) ;

			void const *	operator*(void) const { return rawbuffer_ ; }
			size_t	size(void) const { return (const char *) far_ - (const char *) rawbuffer_ ; }

			void	swap(buffer & aother)
					{
						std::swap(rawbuffer_, aother.rawbuffer_) ;
						std::swap(far_, aother.far_) ;
					}
			buffer &	operator=(const buffer & abuf)
						{
							if ( &abuf != this )
							{
								rawbuffer_= abuf.rawbuffer_ ;
								far_= abuf.far_ ;
							}
							return * this ;
						}

			static const buffer nullbuf ;

		protected:
				// create a zero size buffer, mostly for build
			buffer(void const * aptr ) : rawbuffer_(aptr), far_( rawbuffer_ ) { }

			void const * rawbuffer_ { nullptr },
						* far_ { nullptr } ;
	} ;

} ;

#endif

