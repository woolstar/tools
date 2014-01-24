// vek_iter.h -*- C++ -*-

#ifndef _CTL_VECTORITER
#define _CTL_VECTORITER	1

namespace ctl
{

	template <class T>
		class vector<T>::iterator : public __detail::vector_iter_b
		{
			public:
				constexpr iterator(data * aptr, off_t::iterator itr ) noexcept : __detail::vector_iter_b::vector_iter_b( aptr, itr )
				{ }

				operator const_iterator() const { return const_iterator( ptr_) ; }

				iterator	operator++() noexcept { step() ;  return * this ; }
				iterator	operator++(int) { iterator itmp( * this ) ;  step() ;  return itmp ; }

				typedef T	value_type ;
				typedef T *	pointer ;
				typedef T &	reference ;

				T &	operator*() const noexcept
								{
									auto rec= reinterpret_cast<__detail::vector_ctrl_common<T> *>( ptr_ ) ;
									return * ( rec-> contain() ) ;
								}
				T *	operator->() const noexcept
								{
									auto rec= reinterpret_cast<__detail::vector_ctrl_common<T> *>( ptr_ ) ;
									return rec-> contain() ;
								}
		} ;

} ;

