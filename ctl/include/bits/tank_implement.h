// tank_implement.h -*- C++ -*-

#ifndef _CTL_TANKIMPL
#define _CTL_TANKIMPL	1

#include <typeinfo>
#include <utility>
#include <cstring>

#include <stdio.h>

namespace ctl
{

	namespace __detail
	{
		template <class T, class Tbase>
			class tank_ctrl : public tank_ctrl_common<Tbase>
			{
				public:
					tank_ctrl(tank_ctrl &&) ;
					template<class... Args>
						tank_ctrl( Args&&... arg ) : tank_ctrl()
						{
							void * mem= & storage_ ;
							tank_ctrl_common<Tbase>::locate( new( mem)T(arg... ) ) ;
						}

					using data = tank_ctrl_base::data ;

					void move(data * zstorage) override { }
					void destroy(void) override { ptr()->~T() ; }
					void trace(void) const override
					{
						T * p ;
						Tbase * bp ;

						fprintf(stderr, "T - tnk+ctrl, (type) %s :: (base) %s  sz= %zu, full= %u, base= %u.\n",
							typeid( p).name(), typeid( bp).name(),
							tank_ctrl_base::size_ , tank_ctrl_base::full_, tank_ctrl_base::base_ ) ;
					}

				private:
					tank_ctrl() : tank_ctrl_common<Tbase>( sizeof(* this), (long) & storage_ - (long) this ) { }

					T *	ptr(void) noexcept
						{ return static_cast<T *>( static_cast<void *>( & storage_ )) ; }

					typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type storage_ ;
			} ;
	} ;

	// implementation
	// lots of ugly

	// creates in place

	template <class T>
		template <class Tc, class... Args>
			typename tank<T>::iterator tank<T>::emplace(tank<T>::const_iterator apos, Args&&... arg )
			{
				using ctrl = __detail::tank_ctrl<Tc,T> ;
				size_t xsize = sizeof( ctrl) ;
				size_t lcur ;
				data * dcur, * dend ;
				ctrl * rec ;

				lcur= apos.location() - storage_.get() ;

				reserve( xsize) ;
				dcur= storage_.get() + lcur ;
				dend= storage_.get() + use_ ;
				if ( dcur < dend )
					{ relocate( dcur, dend, dcur + xsize ) ; }
				try
				{
					rec= new( dcur ) ctrl( std::forward<Args>( arg )... ) ;
				}
				catch ( ... )
				{
						// put the original objects back
					relocate( dcur + xsize, dend + xsize, dcur ) ;
					throw ;
				}
				use( xsize ) ;
				return iterator( dcur) ;
			}

	template <class T>
		template <class Tc, class... Args>
			typename tank<T>::iterator tank<T>::emplace_back( Args&&... arg )
			{
				using ctrl = __detail::tank_ctrl<Tc,T> ;
				size_t xsize = sizeof( ctrl) ;
				ctrl * rec ;
				data * dcur ; 

				reserve( xsize) ;
				dcur= storage_.get() + use_ ;
				rec= new( dcur ) ctrl( std::forward<Args>( arg )... ) ;
				use( xsize) ;
				return iterator( dcur) ;
			}

	template <class T>
		template <class Tc>
			typename tank<T>::iterator tank<T>::transfer_back( Tc && aref )
			{
				using ctrl = __detail::tank_ctrl<typename std::remove_reference<Tc>::type,T> ;
				size_t xsize = sizeof( ctrl) ;
				ctrl * rec ;
				data * dcur ; 

				reserve( xsize) ;
				dcur= storage_.get() + use_ ;
				rec= new( dcur ) ctrl( std::forward<Tc>( aref) ) ;
				use( xsize) ;
				return iterator( dcur) ;
			}

	// splice 

	template <class T>
		void tank<T>::splice( const_iterator apos, tank && aother )
		{
			if ( aother.use_ )
			{
				data_splice( apos.location(), aother.storage_.get(), aother.use_ ) ;
				aother.use_= 0 ;
			}
		}

	template <class T>
		void	tank<T>::splice( const_iterator apos, tank && aother, const_iterator asrc )
		{
			using ctrl = __detail::tank_ctrl_common<T> ;
			data * ptr= asrc.location() ;
			ctrl * rec = static_cast<ctrl *>( (void *) ptr ) ;
			size_t xsize= rec-> size_ ;

			data_splice( apos.location(), ptr, xsize ) ;

			relocate( ptr + xsize, aother.storage_.get(), ptr ) ;
			aother.reduce( xsize) ;
		}

	template <class T>
		void	tank<T>::splice( const_iterator apos, tank && aother, const_range asrc )
		{
			data * ptr= asrc.location() ;
			size_t xsize= asrc.span() ;

			data_splice( apos.location(), ptr, xsize ) ;

			relocate( ptr + xsize, aother.storage_.get(), ptr ) ;
			aother.reduce( xsize) ;
		}

	template <class T>
		void	tank<T>::splice( const_iterator apos, tank && aother, const_iterator asrc, const_iterator alim )
			{ splice( apos, aother, const_range( asrc, alim) ) ; }

	// erase

	template <class T>
		typename tank<T>::iterator	tank<T>::erase(const_iterator apos)
		{
			using ctrl = __detail::tank_ctrl_common<T> ;
			data * ptr= apos.location() ;
			ctrl * rec = static_cast<ctrl *>( (void *) ptr ) ;
			size_t xsize= rec-> size_ ;
			rec-> destroy() ;
			relocate( ptr + xsize, storage_.get() + use_, ptr ) ;
			reduce( xsize ) ;
			return iterator( ptr) ;
		}

	template <class T>
		typename tank<T>::iterator	tank<T>::erase(const_range arng)
		{
			using ctrl = __detail::tank_ctrl_common<T> ;
			data * dstart= arng.location() , * dend ;
			size_t xsize ;

			while ( arng )
			{
				ctrl * rec= static_cast<ctrl *>( (void *) arng.location() ) ;
				++ arng ;
				rec-> destroy() ;
			}
			dend= arng.location() ;
			xsize= dend - dstart ;
			relocate( dend, storage_.get() + use_, dstart ) ;
			reduce( xsize) ;
			return iterator( dstart) ;
		}

		// erase adapter :: two iterators -> range object
	template <class T>
		typename tank<T>::iterator	tank<T>::erase(const_iterator astart, const_iterator alim)
			{ return erase( const_range( astart, alim)) ; }
  
		//
	template <class T>
		T & tank<T>::at( unsigned int apos )
		{
			range rstep( span()) ;

			while ( apos && rstep ) { apos --, ++ rstep ; }
			if ( ! rstep ) { throw std::out_of_range("index beyond end of tank") ; }
			return * rstep ;
		}

	template <class T>
		T &	tank<T>::front( void )
		{ return * tank<T>::iterator( storage_.get() ) ; } ;

	template <class T>
		const T &	tank<T>::front( void ) const
		{ return * tank<T>::iterator( storage_.get() ) ; } ;

	// iter

	template <class T>
		typename tank<T>::iterator	tank<T>::begin( void ) noexcept
		{ return tank<T>::iterator( storage_.get() ) ; } ;

	template <class T>
		typename tank<T>::const_iterator	tank<T>::begin( void ) const noexcept
		{ return tank<T>::const_iterator( storage_.get() ) ; } ;
	template <class T>
		typename tank<T>::const_iterator	tank<T>::cbegin( void ) const noexcept
		{ return tank<T>::const_iterator( storage_.get() ) ; } ;

	template <class T>
		typename tank<T>::iterator	tank<T>::end( void ) noexcept
		{ return tank<T>::iterator( storage_.get() + use_ ) ; } ;

	template <class T>
		typename tank<T>::const_iterator	tank<T>::end( void ) const noexcept
		{ return tank<T>::const_iterator( storage_.get() + use_ ) ; } ;
	template <class T>
		typename tank<T>::const_iterator	tank<T>::cend( void ) const noexcept
		{ return tank<T>::const_iterator( storage_.get() + use_ ) ; } ;

	template <class T>
		typename tank<T>::range	tank<T>::span( void ) noexcept
		{ return tank<T>::range( storage_.get(), storage_.get() + use_ ) ; } ;

	template <class T>
		typename tank<T>::const_range	tank<T>::span( void ) const noexcept
		{ return tank<T>::const_range( storage_.get(), storage_.get() + use_ ) ; } ;
	template <class T>
		typename tank<T>::const_range	tank<T>::cspan( void ) const noexcept
		{ return tank<T>::const_range( storage_.get(), storage_.get() + use_ ) ; } ;

	// internal 

	template <class T>
		void	tank<T>::relocate( data * dstart, data *dlimit, data * zdest )
		{
			std::memmove(zdest, dstart, dlimit - dstart) ;
		}

	template <class T>
		void	tank<T>::data_splice( data * zloc, data * asrc, size_t asz )
		{
			size_t lcur= zloc - storage_.get() ;
			data * dcur, * dend ;

			reserve( asz ) ;
			dcur= storage_.get() + lcur ;
			dend= storage_.get() + use_ ;
			if ( dcur < dend ) { relocate( dcur, dend, dcur + asz ) ; }
			try
			{
				relocate( asrc, asrc + asz, dcur ) ;
			}
			catch ( ... )
			{
					// put original objects back
				relocate( dcur + asz, dend + asz, dcur ) ;
				throw ;
			}

			use( asz) ;
		}


	// ops

	template <class T>
		void	tank<T>::swap( tank & aother )
		{
			size_t tmpu, tmptot ;
			tmpu= aother.use_, tmptot= aother.total_ ;

			std::swap( storage_, aother.storage_ ) ;
			aother.use_= use_, aother.total_= total_ ;
			use_= tmpu, total_= tmptot ;
		}

	template <class T>
		template <class Pred>
			void	tank<T>::remove_if( Pred p )
			{
				using ctrl = __detail::tank_ctrl_common<T> ;
				data * dstep= storage_.get(), * dnext ;
				size_t xsize ;
				ctrl * rec ;

				for ( ; ( dstep < ( storage_.get() + use_ ) ) ; dstep= dnext )
				{
					ctrl * rec= static_cast<ctrl *>( (void *) dstep ) ;
					dnext= dstep + ( xsize= rec-> size_ ) ;
					if ( p( * ( rec-> contain() ) ) )
					{
						relocate( dnext, storage_.get() + use_, dstep ) ;
						reduce( xsize ) ;
						dnext= dstep ;
					}
				}
			}

} ;

#endif

