#ifndef BTL_BUFFER_
#define BTL_BUFFER_ 1

#include <string>

namespace btl
{
	class	buffer
	{
		public:
			buffer() { }
			buffer(void const * aptr, size_t n) : rawbuffer_(aptr), far_( (const char *) rawbuffer_ + n) { }
			buffer(const std::string & astr ) : rawbuffer_( astr.c_str() ), far_( astr.c_str() + astr.length() ) { }
			buffer(char const * astr) ;

			void const *	operator*(void) const { return rawbuffer_ ; }
			size_t	size(void) const { return (const char *) far_ - (const char *) rawbuffer_ ; }

			void	swap(buffer & aother)
					{
						std::swap(rawbuffer_, aother.rawbuffer_) ;
						std::swap(far_, aother.far_) ;
					}

		protected:
			void const * rawbuffer_ { nullptr },
						* far_ { nullptr } ;
	} ;
} ;

#endif

