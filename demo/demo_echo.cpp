#include <io>
#include <buffer>

using btl::net_connector ;
using btl::socket_io ;
using btl::ioerr ;

	bool	isrunning = true ;

	btl::manage		servmgr ;

	//

	static net_connector	create_serv_0()
	{
		try
		{
			net_connector	serv( 0) ;
			return serv ;
		}
		catch (std::runtime_error &e)
		{
			btl::ioerr << "server create error :: " << e.what() << "\n" ;
			perror("why") ;
			exit( 1) ;
		}

		// optional return invalid value:   return net_connector( -1) ;
	}

	struct gen_client_con
	{
		btl::feeder_connection_m<socket_io>	operator()(btl::IO_Socket) ;
	} ;

	class	echomsg : public btl::channel_if
	{
		public:
			echomsg( btl::feeder & afeed, const socket_io & aio)
				: channel_if( afeed), conn_( aio)
					{ }

			void	data(const btl::buffer &) ;

			const socket_io	& conn_ ;
	} ;

	btl::feeder_connection_m<socket_io>	gen_client_con::operator()(btl::IO_Socket aport)
	{
		auto h = btl::make_feeder( socket_io( aport)) ;

		h.dest_ -> emplace_back<btl::channel_buffered_scanner<btl::text_scanf>>() ;
		h.dest_ -> emplace_back<echomsg>(* h) ;
		// h.init() ;

		(* h).print(":: online\n") ;

		return h ;
	}

	void	echomsg::data(const btl::buffer & txt)
	{
		if ( txt.size() && ( '.' == * (char *) * txt )) { isrunning= false ; }
		ioerr << "Msg: " << txt << "\n" ;
		conn_ << "ECHO: " << txt << "\n" ;

		if ( ! isrunning ) { ioerr << "halt.\n" ; }
	}

	void	setup(void)
	{
		net_connector	serv { create_serv_0() } ;
		btl::ioerr << "Server at port " << btl::format( "%d", serv.slot_ ) << "\n" ;

		servmgr.monitor(
			btl::make_connector<btl::feeder_connection_m<socket_io>>
				( move( serv), gen_client_con() ),
			serv.port_
		) ;
	}

	void	testecho(void)
	{
		while ( isrunning && servmgr.isactive() )
			{ servmgr.watch( 1000) ; }
	}

int main()
{
	setup() ;
	testecho() ;
}

