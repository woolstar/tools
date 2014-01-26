// vek_iter.h -*- C++ -*-

#ifndef _CTL_VECTORITER
#define _CTL_VECTORITER	1

namespace ctl
{

	template <class T>
		class vector<T>::iterator : public __detail::vector_iter_b
		{
			public:
				constexpr iterator(data * abase, off_t::iterator itr ) noexcept : __detail::vector_iter_b::vector_iter_b( abase, itr )
				{ }

				operator const_iterator() const { return const_iterator( pbase_, it_) ; }

				iterator &	operator++() noexcept { step() ;  return * this ; }
				iterator	operator++(int) { iterator itmp( * this ) ;  step() ;  return itmp ; }


				typedef T	value_type ;
				typedef T *	pointer ;
				typedef T &	reference ;

				T &	operator*() const noexcept
								{
									auto rec= reinterpret_cast<__detail::vector_ctrl_common<T> *>( pbase_ + ( * it_ ) ) ;
									return * ( rec-> contain() ) ;
								}
				T *	operator->() const noexcept
								{
									auto rec= reinterpret_cast<__detail::vector_ctrl_common<T> *>( pbase_ + ( * it_ ) ) ;
									return rec-> contain() ;
								}
		} ;

	template <class T>
		class vector<T>::const_iterator : public __detail::vector_iter_b
		{
			public:
				constexpr const_iterator(data * abase, off_t::iterator itr ) noexcept : __detail::vector_iter_b::vector_iter_b( abase, itr )
				{ }

				const_iterator &	operator++() noexcept { step() ;  return * this ; }
				const_iterator		operator++(int) { const_iterator itmp( * this ) ;  step() ;  return itmp ; }

				typedef const T	value_type ;
				typedef const T *	pointer ;
				typedef const T &	reference ;

				const T &	operator*() const noexcept
								{
									auto rec= reinterpret_cast<__detail::vector_ctrl_common<T> *>( pbase_ + ( * it_ ) ) ;
									return * ( rec-> contain() ) ;
								}
				const T *	operator->() const noexcept
								{
									auto rec= reinterpret_cast<__detail::vector_ctrl_common<T> *>( pbase_ + ( * it_ ) ) ;
									return rec-> contain() ;
								}
		} ;

	template <class T>
		class vector<T>::range : public __detail::vector_range_b
		{
			public:
				range( data * abase, off_t & aoff ) : __detail::vector_range_b( abase, aoff ) { }

				range &	operator++() noexcept { step() ;  return * this ; }

				T &	operator*() const noexcept
								{
									auto rec= reinterpret_cast<__detail::vector_ctrl_common<T> *>( pbase_ + ( * it_ ) ) ;
									return * ( rec-> contain() ) ;
								}
				T *	operator->() const noexcept
								{
									auto rec= reinterpret_cast<__detail::vector_ctrl_common<T> *>( pbase_ + ( * it_ ) ) ;
									return rec-> contain() ;
								}
		} ;

} ;

#endif

