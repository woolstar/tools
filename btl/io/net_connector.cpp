#include <io>

btl::net_connector_t::net_connector_t(int aslot, bool reuseaddr, int aqueue ) : socket_io( bind( aslot, reuseaddr ) )
{
	listen( aqueue) ;
}

