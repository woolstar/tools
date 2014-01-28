// vek_implement.h -*- C++ -*-

#ifndef _CTL_VECTORIMPL
#define _CTL_VECTORIMPL	1

#include <typeinfo>
#include <utility>
#include <cstring>

namespace ctl
{
	namespace __detail
	{
		template <class T, class Tb>
			class vector_ctrl : public vector_ctrl_common<Tb>
			{
				public:
					vector_ctrl( vector_ctrl && ) ;
					template <class... Ar>
						vector_ctrl( Ar&&... arg ) : vector_ctrl()
						{
							void * mem= & storage_ ;
							vector_ctrl_common<Tb>::locate( new( mem)T(arg... ) ) ;
						}

					using data = vector_ctrl_base::data ;

					void move( data * zstorage ) override { }
					void destroy(void) override { ptr()-> ~ T() ; }
					void trace(void) const override
					{
					} ;

				protected:
					vector_ctrl() : vector_ctrl_common<Tb>( (long) &storage_ - (long) this ) { }

					friend class vector<Tb> ;

				private:
					T * ptr(void) noexcept
						{ return static_cast<T *>( (void *) &storage_ ) ; }

					typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type
						storage_ ;
			} ;
	} ;

	template <class T>
		vector<T>::vector(size_t anum)
		{
			size_t itemsz= sizeof( __detail::vector_ctrl<T,T> ) ;
			size_t datsz= anum * itemsz * 1.05 ;

				// guess at how large N Base are going to be + ctrl ;
			__detail::vector_base::reserve( datsz, anum) ;
		}

	template <class T>
		template <class Tc, class... Ar>
			typename vector<T>::iterator	vector<T>::emplace_back( Ar&&... arg )
			{
				using ctrl = __detail::vector_ctrl<Tc, T> ;
				size_t xsize = sizeof( ctrl ) ;
				ctrl * rec ;
				data * dcur ;
				int n = offsets_.size() - 1 ;

				reserve( xsize ) ;
				dcur= storage_.get() + use_ ;
				offsets_[n ]= use_ ;

				rec= new( dcur) ctrl( std::forward<Ar>( arg )... ) ;
				use( xsize) ;
				offsets_.push_back( use_ ) ;

				return iterator( storage_.get(), offsets_.end() -1 ) ;
			}

	template <class T>
		template <class Tc>
			typename vector<T>::iterator	vector<T>::transfer_back( Tc && aref )
			{
				using ctrl = __detail::vector_ctrl<typename std::remove_reference<Tc>::type, T> ;
				size_t xsize = sizeof( ctrl ) ;
				ctrl * rec ;
				data * dcur ;
				int n = offsets_.size() - 1 ;

				reserve( xsize ) ;
				dcur= storage_.get() + use_ ;
				offsets_[n ]= use_ ;

				rec= new( dcur) ctrl( std::forward<Tc>( aref )) ;
				use( xsize ) ;
				offsets_.push_back( use_ ) ;

				return iterator( storage_.get(), offsets_.end() -1 ) ;
			}

	// erase

	template <class T>
		typename vector<T>::iterator	vector<T>::erase( const_iterator apos )
		{
			using ctrl = __detail::vector_ctrl_common<T> ;
			data * ptr ;
			ctrl * rec= static_cast<ctrl *>( (void *) ( ptr= apos.location()) ) ;

#if 1
				// horrible hack for libstd deficit
				// current implementation does not allow erase(const_iterator), so recreate plain iterator
			off_t::iterator itx= offsets_.begin() + ( apos.iter() - offsets_.begin() ) ;
#else
				// for compliant implementations
			auto itx= apos.iter() ;
#endif

			size_t xsize= *( itx +1 ) - * ( itx ) ;

			rec-> destroy() ;
			relocate( ptr + xsize, storage_.get() + use_, ptr ) ;
			reduce( xsize ) ;

				// remove from offsets_ iterator and reduce ick
			auto itpost= offsets_.erase( itx ) ;
			auto itend= offsets_.cend() ;
			for ( auto itstep = itpost ; ( itstep != itend ) ; ++ itstep ) { ( * itstep ) -= xsize ; } 

			return iterator( storage_.get(), itpost ) ;
		}

	// iter

	template <class T>
		typename vector<T>::iterator	vector<T>::begin( void ) noexcept
		{ return vector<T>::iterator( storage_.get(), offsets_.begin() ) ; } ;

	template <class T>
		typename vector<T>::const_iterator	vector<T>::begin( void ) const noexcept
		{ return vector<T>::const_iterator( storage_.get(), offsets_.begin() ) ; } ;
	template <class T>
		typename vector<T>::const_iterator	vector<T>::cbegin( void ) const noexcept
		{ return vector<T>::const_iterator( storage_.get(), offsets_.cbegin() ) ; } ;

	template <class T>
		typename vector<T>::iterator	vector<T>::end( void ) noexcept
		{ return vector<T>::iterator( storage_.get(), offsets_.end() -1 ) ; } ;

	template <class T>
		typename vector<T>::const_iterator	vector<T>::end( void ) const noexcept
		{ return vector<T>::const_iterator( storage_.get(), offsets_.end() - 1 ) ; } ;
	template <class T>
		typename vector<T>::const_iterator	vector<T>::cend( void ) const noexcept
		{ return vector<T>::const_iterator( storage_.get(), offsets_.cend() - 1 ) ; } ;

	template <class T>
		typename vector<T>::range	vector<T>::span( void ) noexcept
		{ return vector<T>::range( storage_.get(), offsets_ ) ; }

	template <class T>
		typename vector<T>::const_range	vector<T>::span( void ) const noexcept
		{ return vector<T>::const_range( storage_.get(), offsets_ ) ; }
	template <class T>
		typename vector<T>::const_range	vector<T>::cspan( void ) const noexcept
		{ return vector<T>::const_range( storage_.get(), offsets_ ) ; }

	//

	template <class T>
		void vector<T>::reserve(unsigned int anum)
		{
			size_t itemsz, datsz ;

			itemsz= ( use_ && ( offsets_.size() > 1 )) ? ( use_ / ( offsets_.size() -1 ) ) : ( 1.04 * sizeof( __detail::vector_ctrl<T,T> ) ) ;
			datsz= anum * itemsz ;

			__detail::vector_base::reserve( datsz, anum) ;
		}

	// internal 

	template <class T>
		void	vector<T>::relocate( data * dstart, data *dlimit, data * zdest )
		{
			std::memmove(zdest, dstart, dlimit - dstart) ;
		}

} ;

#endif

