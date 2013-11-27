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

	class	net_connector_t : public socket
	{
		public:
			net_connector_t(int aport = 0, bool reuseaddr = true, int aqueue = 4 ) ;
			~ net_connector_t() { close() ; }

		protected:
				// create connection
			IO_Socket	accept() const ;
			IO_Socket	accept( struct sockaddr_in & ) const ;

			template <class C, typename T> friend class connector<T> ;

		private:
				// setup
			bool		bind(int, bool reuseaddr ) ;
			bool		listen(int aqueue ) ;

	} ;

	class	local_connector_t : public socket
	{
		public:
			local_connector_t( long = 0 ) ;
			local_connector_t( const char * apath ) ;

			~ local_connector_t() { close() ; }

		protected:

			IO_Socket	accept() const ;

			template <class C, typename T> friend class connector<T> ;
	} ;

	template <class C, typename T> class connector : public manage::link
	{
		public:
			connector( T x ) : conn_( move( x )), port_( x.port_ ) { }

			bool	isactive(void) const { return conn_.isactive() ; }
			bool	doread(void) const { conn_.close() ;  return false ; }

			bool	dowrite(void) const
			{
				IO_Socket tmpsock= conn_.accept() ;
				if ( tmpsock < 0 )
					return false ;

				mgr_ -> monitor( C( tmpsock)) ;
				return true ;
			}

			const IO_Port	port_ ;

		private:
			T	conn_ ;
	} ;

		// helper function to automatically determine T
		//	call with make_connector<user_client_class>( connector_object ) ;
		//

	template <class C, typename T>
		connector<C,T> make_connector( T x ) { return connector<C,T>( move( x) ) ; }


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

