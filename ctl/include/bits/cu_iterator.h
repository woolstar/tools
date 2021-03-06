// cu_iterator -*- C++ -*-

#ifndef	_CU_BITS_ITER
#define	_CU_BITS_ITER	1

#include <iterator>
#include <utility>

namespace ctl
{
		// top level iterator because of limitations in C++ type deduction
			// clone of gnu std::__normal_iterator

	template <class Iter, class TContainer> class iterator
	{
		protected:
			Iter	it_ ;

			typedef	std::iterator_traits<Iter>	traits_t ;

		public:
			typedef	Iter	iterator_type ;
			typedef typename traits_t::iterator_category	iterator_category ;
			typedef typename traits_t::value_type			value_type ;
			typedef typename traits_t::difference_type		difference_type ;
			typedef typename traits_t::reference			reference ;
			typedef typename traits_t::pointer				pointer ;

			constexpr iterator() : it_( Iter()) { }

			explicit	iterator(const Iter ait) : it_( ait ) { }

			template <class... Arg>
				explicit	iterator(Arg&&... ar) : it_( std::forward<Arg>( ar)... ) { }

				// makes a const version -- no safeties
			template <typename UIter>
				iterator( iterator<UIter, TContainer> ait ) : it_( ait.base() ) { }

				// type erasure interface

			reference	operator*() const { return * it_ ; }
			pointer		operator->() const { return it_ ; }
			reference	operator[](const difference_type& aind) { return it_[ aind] ; }

			iterator &	operator++() { ++ it_ ;  return * this ; }
			iterator &	operator--() { -- it_ ;  return * this ; }
			iterator	operator++(int) { return iterator( it_ ++ ) ; }
			iterator	operator--(int) { return iterator( it_ -- ) ; }

			iterator &	operator+=(const difference_type& adelt) { it_ += adelt ;  return * this ; }
			iterator	operator+=(const difference_type& adelt) const { return iterator( it_ + adelt ) ; }

			iterator	operator+(const difference_type& adelt) { return iterator( it_ + adelt ) ; }
			iterator	operator-(const difference_type& adelt) { return iterator( it_ - adelt ) ; }

			const Iter& base() const { return it_ ; }
	} ;

} ;

#endif

