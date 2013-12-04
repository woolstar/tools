#include <io>

using btl::net_connector ;

net_connector::net_connector(int aslot, bool reuseaddr, int aqueue )
	: socket_io( bind( aslot, reuseaddr ) ),
		slot_( getslot( aslot, port_ ))
{
	if ( ! slot_ ) { throw std::runtime_error("unable to determine assigned location") ; }

	if ( ! listen( aqueue) ) { throw std::runtime_error("unable to start listen") ; }
}

	//

bool	net_connector::listen( int aqueue )
{
	return ( 0 == ::listen( port_, aqueue )) ;
}

	//

btl::IO_Socket	net_connector::bind(int aslot, bool reuseaddr)
{
	IO_Socket	sock= socket_io::create( SOCK_STREAM) ;
	int tmpret ;
	struct sockaddr_in tmpblock ;

	if ( sock < 0 ) { throw std::runtime_error("unable to create socket") ; }

	tmpblock.sin_family= AF_INET ;
	tmpblock.sin_port= htons( aslot ) ; 
	tmpblock.sin_addr.s_addr= 0 ;

	tmpret= ::bind( aslot, (sockaddr *) & tmpblock, sizeof( tmpblock)) ;
		if ( tmpret ) { dispose( sock) ;  throw std::runtime_error("unable to bind socket") ; }

	return sock ;
}

int		net_connector::getslot( int aslot, IO_Socket aport )
{
	if ( ! aslot )
	{
			// os assigned a random slot to us, ask it for the slot
		struct sockaddr_in  tmpblock ;
		socklen_t  tmpsz= sizeof( tmpblock ) ;
		int	 tmpret ;

		tmpret= getsockname( aport, (struct sockaddr *) & tmpblock, & tmpsz ) ;
		if ( ! tmpret && tmpsz ) { aslot= ntohs( tmpblock.sin_port ) ; }
	}

	return aslot ;
}


