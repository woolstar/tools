// manage -*- C++ -*-

#ifndef _BTL_MANAGE_H
#define _BTL_MANAGE_H 1

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <poll.h>

namespace btl
{
	class	manage 
	{
		public:
			enum Modes
			{
				eRead = ( 1 << 0 ),
				eWrite = ( 1 << 1 ),
			} ;

			template <typename T>
				void	monitor( T x, int amodes = eRead )
				{
					IO_Port	iport = x.port_ ;
					pollfd tmprec = { iport, 0, 0 } ;

					if ( amodes & Modes::eRead ) { tmprec.events |= POLLIN ; }
					if ( amodes & Modes::eWrite ) { tmprec.events |= POLLOUT ; }

					stor_.emplace( std::make_pair( iport,
						std::unique_ptr<const concept_t>( new manage::adapter_t<T>( *this,  std::move( x)) ) ) ) ;
						// look for possible previous record and destroy it
					auto p= std::find_if( poll_.begin(), poll_.end(),
											[iport](const pollfd& rec){ return rec.fd == iport ; }) ;
					if ( p != poll_.end()) { poll_.erase( p) ; }
					poll_.push_back( tmprec) ;
				}

			void	destroy( void ) ;

			enum Results
			{
				eIdle = 0,
				eActivity = 1,
				eError = -1,
			} ;

			void	watch(void) ;
			Results	watch( int amsec ) ;

			bool	isactive(void) const { return ! stor_.empty() ; }

		private:
			struct concept_t {
				virtual ~concept_t() = default ;

				virtual bool	isactive_() const = 0 ;

				virtual bool	doread_() const = 0 ;
				virtual bool	dowrite_() const = 0 ;
			} ;

			template <typename T> class adapter_t : public concept_t
			{
				public:
					adapter_t( manage & aset, T && x ) : io_( std::move( x)) { io_.mngr( aset) ; }

					bool	isactive_(void) const { return io_.isactive() ; }
					bool	doread_() const { return io_.doread() ; }
					bool	dowrite_() const { return io_.dowrite() ; }

					T io_ ;
			} ;

			void	scan( int aactive ) ;
			void	activity( int amask, int aport ) ;

			std::vector<pollfd>	poll_ ;
			std::unordered_map<IO_Port, std::unique_ptr<const concept_t>>	stor_ ;

	} ;
} ;

#endif


