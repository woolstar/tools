// channel -*- C++ -*-

#ifndef _BTL_CHANNEL_H
#define _BTL_CHANNEL_H 1

namespace btl
{
	class	feeder ;

	class channel_if
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

			channel_if	* prev_ = nullptr, * next_ = nullptr ;
	} ;

	class	feeder
	{
		public:
			void	data( const buffer &) ;

			template <class T, class... Args>
				emplace_back(Args&& ... args)
				{
					channel_if * tmpptr = new T(* this, std::forward<Args>(args)...) ;
					if ( nullptr == first_ ) { first_= last_= tmpptr ; }
					else
					{
						last_-> next_= tmpptr, tmpptr-> prev_ = last_ ;
						last_= tmpptr ;
					}
				}

			void remove( channel_if * ) ;

		private:
			channel_if * first_ = nullptr , * last_ = nullptr ;
	} ;

} ;

#endif

