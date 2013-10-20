// stdio -*- C++ -*-

#ifndef _BTL_STDIO_H
#define _BTL_STDIO_H 1

namespace btl
{

	enum IODirection { eReadWrite, eRead, eWrite } ;

	class	iobasic : public io
	{
		public:
			iobasic( IO_Port, IODirection adir ) ;
			iobasic() {}

			int	doread(void) { return 0 ; }
	} ;

	extern iobasic	ioin ;
	extern iobasic	ioout ;

	static io::Init		__init ;

} ;

#endif

