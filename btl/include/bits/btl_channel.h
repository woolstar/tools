// channel -*- C++ -*-

#ifndef _BTL_CHANNEL_H
#define _BTL_CHANNEL_H 1

#include <container>

namespace btl
{
	class	feeder ;

	class channel_if : protected safe_list<channel_if>::listable
	{
		public:
			channel_if( feeder & afeed ) : base_( afeed ) {}
			virtual ~ channel_if() { }

			virtual bool	init(void) { return true ; }
			virtual void	data( const buffer & ) = 0 ;

			virtual void	signal( int) { }

		protected:
			feeder &	base_ ;

			friend feeder ;
	} ;

	class	feeder
	{
		public:
			void	data( const buffer &) ;

			template <class T, class... Args>
				emplace_back(Args&& ... args)
					{ list_ << new T(* this, std::forward<Args>(args)...) ; }

			void remove( channel_if * aptr ) {
						if ( & ( aptr-> base_ ) != this ) return ;
						list_.erase( aptr ) ;
					}

		private:
			safe_list<channel_if>	list_ ;
	} ;

} ;

#endif

