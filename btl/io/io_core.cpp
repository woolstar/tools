#include <io>
#include <unistd.h>

using	btl::io ;

io::io( io && asrc ) : port_( asrc.port_ ), active_( asrc.active_ )
{
	asrc.active_= false ;
}

	//

