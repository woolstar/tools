// channel -*- C++ -*-

#ifndef _BTL_CHANNEL_H
#define _BTL_CHANNEL_H 1

#include <buffer>
#include <container>

namespace btl
{
	class	feeder ;

	class channel_if : public safe_list<channel_if>::listable
	{
		public:
			channel_if( feeder & afeed ) : base_( afeed ) {}
			virtual ~ channel_if() { }

			virtual bool	init(void) { return true ; }
			virtual void	data( const buffer & ) = 0 ;

		protected:
			virtual void	signal( int) { }

			void	pass(const buffer &) const ;

			feeder &	base_ ;

			friend feeder ;
	} ;

	class	feeder
	{
		public:
			void	data( const buffer &) ;

			template <class T, class... Args>
				void emplace_back(Args&& ... args)
					{ list_ << new T(* this, std::forward<Args>(args)...) ; }

			void remove( channel_if * aptr ) {
						if ( & ( aptr-> base_ ) != this ) return ;
						list_.erase( aptr ) ;
					}

		private:
			safe_list<channel_if>	list_ ;
	} ;

	template <class Tio, class Tbuffer = build_static<2048>>
		class	feeder_connection_m : public manage::link
		{
			public:
				feeder_connection_m(Tio && aio)
					: dest_(make_unique<feeder>()), io_( move( aio)) { }

				bool	isactive(void) const { return io_.isactive() ; }
				bool	doread_() const
				{

				}
				bool	dowrite_() const { return false ; }

				std::unique_ptr<feeder>	dest_ ;

			private:
				Tio		io_ ;
				Tbuffer	buffer_ ;
		} ;

} ;

#endif

