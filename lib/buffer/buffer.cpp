#include <buffer>
#include <string.h>

	using btl::buffer ;

	buffer::buffer(char const * astring) : rawbuffer_( astring)
	{
		datasize_= ( nullptr == astring ) ? 0 : strlen( astring) ;
	}
