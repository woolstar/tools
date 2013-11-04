#ifndef _MANAGED_IO_H
#define _MANAGED_IO_H 1

#include <memory>

namespace btl
{
	class	iom : public io
	{
		public:
			template <typename T>
				iom( T x ) : handler_( new adapter_t<T>( std::move( x )) ) {}
			template <typename T>
				iom( IO_Port aport, T x ) : io( aport), handler_( new adapter_t<T>( std::move( x )) ) {}

			~ iom() ;

			bool	doread( void) { return handler_-> doread_( * this) ; }
			bool	dowrite( void) { return handler_-> dowrite_( * this) ; }

			void	destroy( void) ;

		protected:
			manage *	mgr_ ;

			friend manage ;

		private:
			struct concept_t {
				virtual ~concept_t() = default ;

				virtual bool	doread_( iom & ) const = 0 ;
				virtual bool	dowrite_( iom & ) const = 0 ;
			} ;

			template <typename T> struct adapter_t : public concept_t
			{
				adapter_t( T && x ) : op_( std::move( x)) { }

				bool	doread_( io & aio ) const { return op_.doread( aio ) ; }
				bool	dowrite_( io & aio ) const { return op_.dowrite( aio ) ; }

				T op_ ;
			} ;

			std::unique_ptr<const concept_t> handler_ ;
	} ;

	class	iom_base_t
	{
		public:
			virtual ~ iom_base_t() { }

			virtual	bool	doread( iom & ) const { return false ; }
			virtual bool	dowrite( iom & ) const { return false ; }
	} ;
} ;

#endif

