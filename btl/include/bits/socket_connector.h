#ifndef _SOCKET_CONN_H
#define _SOCKET_CONN_H 1

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
	template <class Cmgr, typename T> class connector ;

	class	net_connector : public socket_io
	{
		public:
			net_connector(int aslot = 0, bool reuseaddr = true, int aqueue = 4 ) ;
			~ net_connector() { close() ; }

			template <class SockConn>
				SockConn create(void) const { return SockConn( accept()) ; }

			const int slot_ ;

		protected:
				// create connection
			IO_Socket	accept() const ;
			IO_Socket	accept( struct sockaddr_in & ) const ;

			template <class C, typename T> friend class connector ;

		private:
				// setup
			static IO_Socket	bind(int, bool reuseaddr ) ;
			static int			getslot( int aslot, IO_Socket ) ;

			bool		listen(int aqueue ) ;

		public:
			net_connector(net_connector &&) = default ;
	} ;

	class	local_connector : public socket_io
	{
		public:
			local_connector( long = 0 ) ;
			local_connector( const char * apath ) ;

			~ local_connector() { close() ; }

		protected:

			IO_Socket	accept() const ;

			template <class C, typename T> friend class connector ;

		public:
			local_connector(local_connector &&) = default ;
	} ;

	template <class Cmgr, typename T> class connector : public manage::link
	{
		public:

			template <class Genr>
				connector( T x, Genr agen ) : conn_( move( x )), generator_{ agen } { }

			bool	isactive(void) const { return conn_.isactive() ; }
			bool	doread(void) const
			{
				IO_Socket tmpsock= conn_.accept() ;
				if ( tmpsock < 0 )
					return false ;

				mgr_ -> monitor( generator_( tmpsock), tmpsock ) ;
				return true ;
			}

			bool	dowrite(void) const { return false ; }

		private:
			T	conn_ ;

			std::function<Cmgr(IO_Socket)>	generator_ ;
	} ;

		// helper function to automatically determine T
		//	call with make_connector<user_client_class>( connector_object ) ;
		//

	template <class C, typename T, class Genr>
		connector<C,T> make_connector( T x, Genr agen ) { return connector<C,T>( move( x), agen ) ; }


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
	} ;

} ;

#endif

