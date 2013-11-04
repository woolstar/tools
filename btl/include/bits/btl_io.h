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
			io(IO_Port aport) ;
			template <typename T> io(IO_Port, T x ) ;
			~ io() ;

			io(io &&) ;
			io & operator=(io &&) ;

				// no copy const
			io(const io &) = delete ;
			io & operator=(const io &) = delete ;

			IO_Port	getPort() const { return port_ ; }

				// io operators
			int	read(build_base & zbuf) 
				{ return interface_->read_( port_, active_, zbuf ) ; }
			int	print(const buffer & abuf) 
				{ return interface_->write_( port_, abuf) ; }
			int	ctrl( int icode, void * aptr) 
				{ return interface_->ctrl_( port_, icode, aptr ) ; }
			void	close(void)
				{
					if ( active_ ) {
						active_= false ;
						interface_->close_( port_ ) ;
					}
				}

			const io&	operator<<(const buffer & abuf) { print(abuf) ;  return * this ; }

				// test active
			bool	isactive(void) const { return active_ ; }

		protected:
			io() : port_( 0) {}

			void	setup(IO_Port) ;

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

			struct	base_if : public interface_t {
				int	read_( IO_Port, bool &, build_base & ) const ;
				int	write_( IO_Port, const buffer & ) const ;
				int	ctrl_( IO_Port, int icode, void * ) const ;
				void	close_( IO_Port ) const ;
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

