#ifndef _MANAGED_IO_H
#define _MANAGED_IO_H 1

namespace btl
{
	class	iom : public io
	{
		public:
			template <typename T>
				iom( T x ) : handler_( new adapter_t<T>( std::move( x )) ) {}
			template <typename T>
				iom( IO_Port aport, T x ) : io( aport), handler_( new adapter_t<T>( std::move( x )) ) {}

			bool	doread( void) { return worker_.doread_( * this) ; }
			bool	dowrite( void) { return worker_.dowrite_( * this) ; }

		protected:
			manage *	mgr_ ;

			friend manage ;

		private:
			struct concept_t {
				virtual ~concept_t() = default ;

				virtual bool	doread_( io & ) const = 0 ;
				virtual bool	dowrite_( io & ) const = 0 ;
			} ;

			template <typename T> struct adapter_t : public concept_t
			{
				adapter_t( T && x ) : op_( std::move( x)) { }

				bool	doread_( io & aio ) const { return op_.doread( aio ) ; }
				bool	dowrite_( io & aio ) const { return op_.dowrite( aio ) ; }

				T op_ ;
			} ;

			std::unique_ptr<const concept_t> worker_ ;
	} ;
} ;

#endif

