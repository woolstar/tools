#include <io>

#ifndef WIN32
#	include <sys/socket.h>
#	include <sys/ioctl.h>
#	include <sys/un.h>
#	include <netdb.h>
#	include <arpa/inet.h>
#endif

using btl::socket ;

	//

bool	socket::resolv( in_addr_t & zaddr, const char * ahost )
{
	int tmpReturn, ia, ib, ic, id ;

	if ( inet_pton( AF_INET, ahost, & zaddr ))
		{ return true ; }

	struct hostent  * tmpHostInfo= gethostbyname(ahost) ;

		// failed to resolve hostname if NULL 
	if (NULL == tmpHostInfo) { return false ; }
	memcpy(& zaddr, tmpHostInfo-> h_addr, sizeof(zaddr)) ;
	return true ;
}

	//

socket::IO_Socket	socket::create(int atype, int afamily)
{
	return ::socket( afamily, atype, 0) ;
}

void	socket::dispose( socket::IO_Socket aport )
{

#ifdef	WIN32
	::closesocket( aport ) ;
#else
	::close( aport ) ;
#endif
}

	//
	// WIN32 requires a call to the system to init networking

#ifdef WIN32

socket::init::init()
{
	WSADATA	wsadata ;
	WORD version = MAKEWORD( 1, 1 ) ;

	if ( 0 != WSAStartup(version,&wsadata)) exit( 1) ;
}

socket::init	socket::init::__init ;

#endif

