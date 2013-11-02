// io core -*- C++ -*-

#ifndef _BTL_IOCORE_H
#define _BTL_IOCORE_H 1

#include <atomic>

namespace btl
{
	typedef int	IO_Port ;

	class	io
	{
		public:
			io(IO_Port) ;
			~ io() ;

			io(io &&) ;
			io & operator=(io &&) ;

				// no copy const
			io(const io &) = delete ;
			io & operator=(const io &) = delete ;

			IO_Port	getPort() const { return port_ ; }

				// io operators
			void	close(void) ;
			int	read(build_base &) const ;
			int	print(const buffer &) const ;

			const io&	operator<<(const buffer & abuf) const { print(abuf) ;  return * this ; }

				// test active
			bool	isactive(void) const { return active_ ; }

		protected:
			io() : port_( 0) {}

			void	setup(IO_Port) ;

			IO_Port	port_ ;
			mutable bool	active_ = false ;

		public:
			class	Init
			{
				public:  Init() ;  ~Init() ;
				private:
					std::atomic<int>	refcount_ ;

					friend class io ;
			} ;

	} ;

} ;

#endif

