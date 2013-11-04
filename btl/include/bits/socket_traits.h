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
	class	socket_t
	{
		public:
			int	read(IO_Port, build_base &) const ;
			int	print(IO_Port, const buffer &) const ;
			int	close(IO_Port) const ;

				// helpers
			static unsigned int	getService(const char * aname, const char * atyp = "tcp" ) ;

			static bool	resolv( in_addr_t &, const char * ahost ) ;

			typedef IO_Port	IO_Socket ;

		protected:
				// PF_INET
			static IO_Socket	connect(const char *, int, int = 0) ;
			static IO_Socket	connect(const struct in_addr *, int, int = 0 ) ;

				// PF_LOCAL
			static IO_Socket	connect(long) ;
			static IO_Socket	connect(const char *) ;

			static IO_Socket	create(int atype, int afamily = PF_INET ) ;
			static void		dispose( IO_Socket ) ;
			static std::pair<IO_Socket, IO_Socket>	makepair( void) ;

		private:
			class init
			{
				public:
					init() ;
					~ init() ;
			} ;

			static init	__init ;	// windows WINSockInit
	} ;
} ;

#endif

