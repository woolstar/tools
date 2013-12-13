// io core -*- C++ -*-

#ifndef _BTL_IOCORE_H
#define _BTL_IOCORE_H 1

#include <atomic>
#include <memory>

#include <unistd.h>

namespace btl
{
	typedef int	IO_Port ;

	class	io
	{
		public:
			io( IO_Port aport ) : port_( aport), active_( aport >= 0 ) { }

				// no copy const
			io( const io & ) = delete ;
			io & operator=(const io &) = delete ;

				// ok, to move an IO
			io( io && ) ;

				// test active
			bool	isactive(void) const { return active_ ; }

				// close up port_
			void	close(void)
					{
						if ( active_ ) {
							active_= false ;
							::close( port_ ) ;
						}
					}

			const IO_Port	port_ ;

		protected:
			mutable bool	active_ = false ;

			~ io() { }

		public:
			class	Init
			{
				public:  Init() ;  ~Init() ;
				private:
					std::atomic<int>	refcount_ ;

					friend class io ;
			} ;

		protected:
				// buffer internals helpers
			static sized_storage *	fill(build_base & abuf) { return end( abuf) ; }
			static void				jump(build_base & abuf, int ioff) { abuf.jump( ioff) ; }
	} ;

} ;

#endif

