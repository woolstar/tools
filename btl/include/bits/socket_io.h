#ifndef _MANAGED_IO_H
#define _MANAGED_IO_H 1

#include <sys/types.h>

#ifndef WIN32
#	include <sys/socket.h>
#	include <netinet/in.h>
#else
#	include <winsock2.h>

	typedef unsigned long	in_addr_t ;
	typedef int		socklen_t ;

#define	EINPROGRESS	WSAEINPROGRESS

#endif

#include <utility>

namespace btl
{
	class	socket : public io
	{
		public:

			~ socket() ;

			int	read(build_base &) const ;
			int read_from(build_base &, struct sockaddr &) const ;
			int	print(const buffer &) const ;

			int	ctrl(int, void *) const ;

			const socket&	operator<<(const buffer & abuf) const { print( abuf) ;  return * this ; }

				// helpers
			static bool			resolv( in_addr_t &, const char * ahost ) ;
			static unsigned int	getService(const char * aname, const char * atyp = "tcp" ) ;

			typedef IO_Port	IO_Socket ;

		protected:

			// outbound
				// PF_INET
			static IO_Socket	connect(const char *, int ) ;
			static IO_Socket	connect(const struct in_addr *, int ) ;
				// PF_LOCAL
			static IO_Socket	connect(long) ;
			static IO_Socket	connect(const char *) ;


				// base operators
			static IO_Socket	create(int atype, int afamily = PF_INET ) ;
			static void			dispose( IO_Socket ) ;

			static std::pair<IO_Socket, IO_Socket>	makepair( void) ;

		private:
			class init
			{
				public:
					init() ;
			} ;

#ifdef WIN32
			static init	__init ;	// windows WINSockInit
#endif
	} ;
} ;

#endif

