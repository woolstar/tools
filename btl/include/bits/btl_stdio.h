// stdio -*- C++ -*-

#ifndef _BTL_STDIO_H
#define _BTL_STDIO_H 1

namespace btl
{
	class	iobasic : public io
	{
		public:
			iobasic( IO_Port ) ;
			~ iobasic() { close() ; }

			iobasic( iobasic && ) = default ;

				// io operators
			int	read(build_base & zbuf) const ;
			int	print(const buffer & abuf) const ;
			int	ctrl( int icode, void * aptr) const ;

			const iobasic&	operator<<(const buffer & abuf) const { print(abuf) ;  return * this ; }
	} ;

	extern iobasic	ioin ;
	extern iobasic	ioout ;
	extern iobasic	ioerr ;

	static io::Init		__init ;

} ;

#endif

