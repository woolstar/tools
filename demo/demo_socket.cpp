#include <io>
#include <buffer>

using btl::socket_io ;
using btl::net_connector ;

	static socket_io	create_socket_80()
	{
		try
		{
			socket_io	conn= socket_io::make_connection( "localhost", 80 ) ;
			return conn ;
		}
		catch ( std::runtime_error &e )
		{
			btl::ioerr << "unable to connect(2) :: " << e.what() << "\n" ;
			exit( 1) ;
		} ;
	}

	void	testweb(void)
	{
		socket_io	conn { create_socket_80() } ;
		btl::build_static<512>	abuf ;

		conn << "GET /\n" ;
		conn >> abuf ;

		btl::ioout << "GOT: " << abuf << "\n----\n" ;
	}

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

	void	testserv(void)
	{
		net_connector	serv { create_serv_0() } ;

		btl::ioerr << "Server at port " << btl::format( "%d", serv.slot_ ) << "\n" ;

		socket_io  conn{ serv.create<socket_io>() } ;
		btl::build_static<512>	abuf ;

		conn << "Welcome.  Enter message.\n" ;
		conn >> abuf ;

		btl::ioout << "GOT: " << abuf << "\n----\n" ;
	}

int main()
{
	testweb() ;
	testserv() ;
}

