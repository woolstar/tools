// io core -*- C++ -*-

#ifndef _BTL_IOCORE_H
#define _BTL_IOCORE_H 1

#include <atomic>

namespace btl
{
	typedef int	IO_Port ;

	class	manage ;

	class	io
	{
		public:
			io(IO_Port) ;
			virtual ~ io() ;

				// specialization
			virtual int	doread(void) = 0 ;
			virtual bool	dowrite(void) { return false ; }

				// io operators
			virtual void	close(void) ;
			virtual int	read(build_base &) const ;
			virtual int	print(const buffer &) const ;

			const io&	operator<<(const buffer & abuf) const { print(abuf) ;  return * this ; }

				// test active
			bool	isactive(void) const { return active_ ; }

		protected:
			io() : port_( 0) {}

			void	setup(IO_Port) ;

			friend class	manage ;

			virtual IO_Port	getPort() const { return port_ ; }

			IO_Port	port_ ;
			mutable bool	active_ = false ;
			manage	* set_ = nullptr ;

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

