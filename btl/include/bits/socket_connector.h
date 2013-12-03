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

	class	net_connector_t : public socket_io
	{
		public:
			net_connector_t(int aslot = 0, bool reuseaddr = true, int aqueue = 4 ) ;
			~ net_connector_t() { close() ; }

			template <class SockConn>
				SockConn create(void) const { return SockConn( accept()) ; }

		protected:
				// create connection
			IO_Socket	accept() const ;
			IO_Socket	accept( struct sockaddr_in & ) const ;

			template <class C, typename T> friend class connector ;

		private:
				// setup
			static IO_Socket	bind(int, bool reuseaddr ) ;

			bool		listen(int aqueue ) ;

	} ;

	class	local_connector_t : public socket_io
	{
		public:
			local_connector_t( long = 0 ) ;
			local_connector_t( const char * apath ) ;

			~ local_connector_t() { close() ; }

		protected:

			IO_Socket	accept() const ;

			template <class C, typename T> friend class connector ;
	} ;

	template <class Cmgr, typename T> class connector : public manage::link
	{
		public:

			template <class Genr>
				connector( T x, Genr agen ) : conn_( move( x )), generator_{ agen } { }

			bool	isactive(void) const { return conn_.isactive() ; }
			bool	doread(void) const { conn_.close() ;  return false ; }

			bool	dowrite(void) const
			{
				IO_Socket tmpsock= conn_.accept() ;
				if ( tmpsock < 0 )
					return false ;

				mgr_ -> monitor( generator_( tmpsock)) ;
				return true ;
			}

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

