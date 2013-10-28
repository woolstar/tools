// manage -*- C++ -*-

#ifndef _BTL_MANAGE_H
#define _BTL_MANAGE_H 1

#include <vector>
#include <unordered_map>

namespace btl
{
	class	manage 
	{
		public:
			void	monitor( io * ) ;
			void	push( io * ) ;
			void	release( io * ) ;

			void	destroy( void ) ;

			enum Results
			{
				eIdle = 0,
				eActivity = 1,
				eError = -1,
			} ;

			void	watch(void) ;
			Results	watch( int amsec ) ;

			bool	isactive(void) const ;

		protected:
			void	scan( int aactive ) ;
			void	activity( pollfd * ) ;
			void	clean( void) ;

			std::vector<pollfd>	poll_ ;
			std::unordered_map<IO_Port, io *>	stor_ ;

			io	* cur_ ;

	} ;
} ;

#endif

