// io core -*- C++ -*-

#ifndef _BTL_IOCORE_H
#define _BTL_IOCORE_H 1

#include <atomic>
#include <memory>

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
			io( io && ) = default ;

				// test active
			bool	isactive(void) const { return active_ ; }

		protected:
			friend class manage ;

			const IO_Port	port_ ;
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
			static sized_storage *	fill(build_base & abuf) { return abuf.fill_get() ; }
			static void				jump(build_base & abuf, int ioff) { abuf.jump( ioff) ; }
	} ;

	class	iox
	{
		public:
			iox(IO_Port aport) ;
			template <typename T> iox(IO_Port, T x ) ;
			~ iox() ;

			iox(io &&) ;
			iox & operator=(iox &&) ;

				// no copy const
			iox(const iox &) = delete ;
			iox & operator=(const iox &) = delete ;

		protected:
			iox() : port_( 0) {}

			IO_Port	port_ ;
			mutable bool	active_ = false ;

		private:
			struct interface_t {
				virtual ~ interface_t() = default ;

				virtual int	read_( IO_Port, bool &, build_base & ) const = 0 ;
				virtual int	write_( IO_Port, const buffer & ) const = 0 ;
				virtual int	ctrl_( IO_Port, int icode, void * ) const = 0 ;
				virtual void	close_( IO_Port ) const = 0 ;
			} ;

			template <typename T > struct	adapter_if : public interface_t
			{
				adapter_if( T && x ) : op_( std::move( x)) {}

				int	read_( IO_Port aport, bool & isactive, build_base & zbuf ) const
					{ return op_.read( aport, isactive, zbuf ) ; }
				int	write_( IO_Port aport, const buffer & abuf ) const
					{ return op_.write( aport, abuf ) ; }
				int	ctrl_( IO_Port aport, int icode, void * aptr ) const
					{ return op_.ctrl( aport, icode, aptr ) ; }
				void	close_( IO_Port aport ) const { op_.close( aport) ; }

				T	op_ ;
			} ;

			std::unique_ptr<const interface_t>	interface_ ;
	} ;

} ;

#endif

