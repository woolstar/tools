#include <buffer>
#include <string.h>

	using btl::buffer ;

	buffer::buffer(char const * astring) : rawbuffer_( astring)
	{
		far_= ( nullptr == astring ) ? astring : ( astring + strlen( astring) ) ;
	}
