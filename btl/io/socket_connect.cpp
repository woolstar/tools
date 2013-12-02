#include <io>

#ifndef WIN32
#	include <sys/un.h>
#endif

using btl::socket_io ;

socket_io::IO_Socket	socket_io::connect(const char * ahost, int aslot )
{
	struct in_addr tmpaddr ;

	if ( ! resolv( tmpaddr.s_addr, ahost ) ) { return -1 ; }
	return connect( & tmpaddr, aslot ) ;
}

socket_io::IO_Socket	socket_io::connect(const struct in_addr * ahost, int aslot )
{
	int  tmpret, tmpopt ;
	socket_io::IO_Socket	tmpport ;
	struct sockaddr_in	ssin = { 0 } ;

	tmpport= create(SOCK_STREAM) ;
	if ( tmpport < 0 ) { return tmpport ; }

	tmpopt= 1 ;
	setsockopt(tmpport, SOL_SOCKET, SO_KEEPALIVE, (char *) & tmpopt, sizeof( tmpopt)) ;

		// setup addressing
	ssin.sin_family= AF_INET ;
	memcpy((char *) & (ssin.sin_addr), (char *) ahost, sizeof(ssin.sin_addr)) ;
	ssin.sin_port= htons(aslot) ;

		// attempt connection 
	tmpret= ::connect( tmpport, (sockaddr *) & ssin, sizeof ssin) ;
	if ( ! tmpret ) { return tmpport ; }

		// failed
	dispose( tmpport) ;
	return -1 ;
}

	//
	//

socket_io::IO_Socket	socket_io::connect( long aslot)
{
	int  tmpret ;
	socket_io::IO_Socket	tmpport ;
	struct sockaddr_un	ssin = { 0 } ;

	if ( aslot < 1 ) { return -1 ; }

	tmpport= create( SOCK_STREAM, PF_LOCAL) ;
	if ( tmpport < 0 ) { return tmpport ; }

		// setup addressing
	ssin.sun_family= AF_LOCAL ;
	memcpy( ssin.sun_path +1, & aslot, sizeof( aslot)) ;

	tmpret= ::connect( tmpport, (sockaddr *) & ssin, sizeof ssin) ;
	if ( ! tmpret ) { return tmpport ; }

		// failed
	dispose( tmpport) ;
	return -1 ;
}

socket_io::IO_Socket	socket_io::connect( const char * apath )
{
	int  tmpret ;
	socket_io::IO_Socket	tmpport ;
	struct sockaddr_un	ssin = { 0 } ;

	if ( ! apath || ! * apath ) { return -1 ; }

	tmpport= create( SOCK_STREAM, PF_LOCAL) ;
	if ( tmpport < 0 ) { return tmpport ; }

		// setup addressing
	ssin.sun_family= AF_LOCAL ;
	strncpy( ssin.sun_path, apath, sizeof(ssin.sun_path) ) ;

	tmpret= ::connect( tmpport, (sockaddr *) & ssin, sizeof ssin) ;
	if ( ! tmpret ) { return tmpport ; }

		// failed
	dispose( tmpport) ;
	return -1 ;
}

