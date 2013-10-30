#ifndef BTL_BUFFER_
#define BTL_BUFFER_ 1

#include <string>

namespace btl
{
	class	buffer
	{
		public:
			buffer() { }
			buffer(void const * aptr, size_t n) : rawbuffer_(aptr), datasize_(n) { }
			buffer(const std::string & astr ) : rawbuffer_( astr.c_str() ), datasize_( astr.length() ) { }
			buffer(char const * astr) ;

			void const *	operator*(void) const { return rawbuffer_ ; }
			size_t	size(void) const { return datasize_ ; }

		protected:
			void setup( void const * aptr, size_t n) { rawbuffer_= aptr ;  datasize_ = n ; }

			void const * rawbuffer_ { nullptr } ;
			size_t	datasize_ { 0 } ;
	} ;
} ;

#endif

