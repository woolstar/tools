// manage -*- C++ -*-

#ifndef _BTL_MANAGE_H
#define _BTL_MANAGE_H 1

#include <vector>
#include <unordered_map>
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

			template <typename T> void	monitor( T, int amodes = eRead ) ;

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

			template <typename T> struct adapter_t ;

			void	scan( int aactive ) ;
			void	activity( int amask, int aport ) ;

			std::vector<pollfd>	poll_ ;
			std::unordered_map<IO_Port, std::unique_ptr<const concept_t>>	stor_ ;

	} ;
} ;

#endif

