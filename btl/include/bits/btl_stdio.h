// stdio -*- C++ -*-

#ifndef _BTL_STDIO_H
#define _BTL_STDIO_H 1

#include <unistd.h>

namespace btl
{
	class	iobasic : public io
	{
		public:
			iobasic( IO_Port ) ;
			~ iobasic() { close() ; }

				// io operators
			int	read(build_base & zbuf) ;
			int	print(const buffer & abuf) const ;
			int	ctrl( int icode, void * aptr) const ;
			void	close(void)
				{
					if ( active_ ) {
						active_= false ;

						::close( port_ ) ;
					}
				}

			const iobasic&	operator<<(const buffer & abuf) const { print(abuf) ;  return * this ; }
	} ;

	extern iobasic	ioin ;
	extern iobasic	ioout ;

	static io::Init		__init ;

} ;

#endif

