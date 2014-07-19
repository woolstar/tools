// cu_list -*- C++ -*-

#ifndef	_CU_BITS_LIST
#define _CU_BITS_LIST

namespace ctl
{
		// cucumbered polymorphic list

	template <class T> class list : public __detail::list_base
	{
		public:
			list() { }

			typedef	T	base_type ;
			typedef	T	value_type ;

			class	iterator ;
			class	const_iterator ;
			class	range ;
			class	const_range ;

			// storage

			template < class Tc, class... Args >
				iterator	emplace( const_iterator apos, Args&&... arg ) ;
			template < class Tc, class... Args >
				iterator	emplace_front( Args&&... arg ) ;
			template < class Tc, class... Args >
				iterator	emplace_back( Args&&... arg ) ;
			template < class Tc >
				iterator	transfer_back( Tc && aref ) ;
			template < class Tc >
				list<T> &	operator<<( Tc && aref ) { transfer_back( aref) ;  return * this ; }

			iterator	erase(const_iterator pos) ;
			iterator	erase(const_range aspan) ;
			iterator	erase(const_iterator astart, const_iterator aend) ;

			// accessors

			T &	front() ;
			const T &	front() const ;
			T & back() ;
			const T &	back() const ;

			// iterators

			iterator	begin(void) noexcept ;
			const_iterator	begin(void) const noexcept ;
			const_iterator	cbegin(void) const noexcept ;

			iterator	end(void) noexcept ;
			const_iterator	end(void) const noexcept ;
			const_iterator	cend(void) const noexcept ;

			range	span(void) noexcept ;
			const_range	span(void) const noexcept ;
			const_range	cspan(void) const noexcept ;

			// iterators+

			// ghost_range	ghspan(void) noexcept ;

			// modifier

			void	erase() ;
			void	swap( list& other ) ;

			// helpers

			template< class UnaryPredicate >
				void remove_if( UnaryPredicate p ) ;
	} ;

} ;

#endif

