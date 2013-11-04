#ifndef _MANAGED_IO_H
#define _MANAGED_IO_H 1

#include <sys/types.h>

namespace btl
{
		// connector, connector_t
		//
		//	inbound PF_NET type connections
		//	listen on a random port, first port selected if available, second as backup
		//	second form use SO_REUSEADDR to try and use a previously busy port
		//	pass result to iom, with connector_t for handler
		//

	class	connector : public socket_t
	{
		public:
			enum ConstructArg { Reuse = 1, ReuseADDR } ;

			static	IO_Socket	listen(int = 0, int = 0) ;
			static	IO_Socket	listen(int, ConstructArg ) ;
	} ;

	class	connector_t : public iom_base_t
	{
		public:
			bool	doread( iom & ) const ;

	} ;

		// post connect
		//
		//	handle outbound async connect
		//	waits for write available, then signals connection on connected(), error() otherwise
		//

	class	post_connect_t : public iom_base_t
	{
		public:
			bool	dowrite( io & ) const ;

			virtual void	connected( iom & ) const = 0 ;
			virtual void	error( iom & aio ) const { aio.destroy() ; }
	} ;

	class	local_connect_t
	{
	} ;

} ;

#endif

