// stdio -*- C++ -*-

#ifndef _BTL_STDIO_H
#define _BTL_STDIO_H 1

namespace btl
{

	class	ioread : public io
	{
		public:
			ioread( IO_Port ) ;
			ioread() {}

			int	doread(void) ;

			int	read(build_base &) const ;
	} ;

	class	iowrite : public io
	{
		public:
			iowrite( IO_Port ) ;
			iowrite() {}

			int	doread(void) { return 0 ; }
	} ;

	extern ioread	ioin ;
	extern iowrite	ioout ;

	static io::Init		__init ;

} ;

#endif

