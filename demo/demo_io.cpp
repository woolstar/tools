#include <io>
#include <buffer>

	using btl::ioout ;

static bool	runtest = true ;

	class	testmsg : public btl::channel_if
	{
		public:
			testmsg( btl::feeder & afeed) : channel_if( afeed) { }

			void	data(const btl::buffer &) ;
	} ;

	void	testmsg::data(const btl::buffer & txt)
	{
		ioout << "Message: [" << txt << "]\n" ;
		if ( ! strncmp( (const char *) * txt, ".", txt.size())) { runtest= false ; }
	}

int main(int N, char **S )
{
	btl::manage iomgr ;
	ioout << "Hello." << "\n\n" ;

		/***/
	ioout << "Enter data, end with '.'\n" ;
	int idle = 0 ;
	auto & h= iomgr.monitor( btl::make_feeder( move( btl::ioin )), btl::ioin.port_ ) ;

	// demonstrate the static assert catching the lack of exception trait for expand_fixed
		// h.dest_-> emplace_back<btl::channel_buffered_scanner<btl::text_scanf, btl::build_static<1024, btl::expand_fixed>>>( btl::CtrlVec<'\n'>::mask ) ;
	h.dest_-> emplace_back<btl::channel_buffered_scanner<btl::text_scanf>>( btl::CtrlVec<'\n'>::mask ) ;
	h.dest_-> emplace_back<testmsg>() ;

	while ( runtest && iomgr.isactive() ) { if ( btl::manage::eIdle == iomgr.watch( 100)) { idle ++ ; } }
	ioout << "Done: " << btl::format("%g", ( 0.1 * idle )) << "s idle.\n" ;

	return 0 ;
}

