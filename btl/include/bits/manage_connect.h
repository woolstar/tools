#ifndef _MANAGED_IO_H
#define _MANAGED_IO_H 1

#include <sys/types.h>

namespace btl
{
		// connector_t
		//
		//	inbound PF_NET type connections
		//	listen on a port (or random port if zero)
		//	optinal argument use SO_REUSEADDR to try and use a previously busy port
		//	pass result to iom, with connector_t for handler
		//

	class	connector_t : public io, public manage::link
	{
		public:
			enum ConstructArg { None = 0, Reuse = 1, ReuseADDR } ;

			connector(int = 0, ConstructArg = None ) ;
			~ connector() { close() ; }

			bool	doread(void) const ;

			//	leave off dowrite() for subclass to implement and specialize

		private:
			static	IO_Socket	listen(int = 0, ConstructArg = None ) ;

			IO_Socket	accept() ;
	} ;

		// post connect
		//
		//	handle outbound async connect
		//	waits for write available, then signals connection on connected(), error() otherwise
		//

	class	post_connect_t : public io, public manage::link
	{
		public:
			bool	dowrite( io & ) const ;

			bool	doread(void) const { return false ; }
			bool	dowrite(void) const ;

			virtual void	connected( io &, manage * ) = 0 ;
	} ;

} ;

#endif

