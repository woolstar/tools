// list -*- C++ -*-

#ifndef _BTL_LIST_H
#define _BTL_LIST_H 1

#include <iterator>

namespace btl
{
		// encumbered safe list

	template <class _Tp> class safe_list
	{
		public:
			safe_list() : first_( nullptr), last_( nullptr ) { }

			typedef	_Tp		value_type ;
			typedef	size_t	size_type ;

				// listable - the part that each object belongs to (encumbered by)

			class	listable
			{
				public:
					listable() : next_( nullptr ), prev_( nullptr ) { }
					listable(const listable &) = delete ;

					_Tp *	next(void) const { return next_ ; }
					_Tp *	prior(void) const { return prev_ ; }

					static void	step( _Tp * & aptr ) { aptr= aptr-> next_ ; }
					static void	step( const _Tp * & aptr ) { aptr= aptr-> next_ ; }
					static void	back( _Tp * & aptr ) { aptr= aptr-> prev_ ; }
					static void	back( const _Tp * & aptr ) { aptr= aptr-> prev_ ; }

				protected:
					friend class	safe_list<_Tp> ;

					_Tp *	next_, * prev_ ;
			} ;

				// iterator -- a way to step through these things
				//	this iterator stores the next_ pointer independant of ptr_
				//	so it is safe to delete the current object without invalidating the iterator
				//

		public:
			typedef	__detail::_List_iterator_Base< _Tp >	_Base ;
			typedef	__detail::_List_iterator_Base< const _Tp >	_constBase ;

			class	iterator : public _Base
			{
				public:
					iterator	operator++() { step() ;  return * this ; }
					iterator	operator++(int) { iterator itmp( * this) ;  step() ;  return itmp ; }

				public:
					iterator() : _Base( nullptr) { }
					iterator( _Tp * aptr, _Tp * alim = nullptr) : _Base( aptr, alim)
						{ next_ = ( _Base::limit_ != _Base::ptr_ ) ? _Base::ptr_-> next() : _Base::limit_ ; }

					iterator( const iterator & asrc ) = default ;

					void	step( void) { _Base::ptr_= next_ ;  if ( _Base::limit_ != next_ ) { listable::step( next_ ) ; } }

				private:
					_Tp *	next_ = nullptr ;
			} ;

			class	const_iterator ;
			class	reverse_iterator ;
			class	const_reverse_iterator ;

			void	push_front( _Tp * aptr )
					{
						if ( first_ ) { first_-> prev_= aptr, aptr-> next_ = first_ ; }
							else { last_= aptr, aptr-> next_ = nullptr ; }
						first_= aptr, first_-> prev_= nullptr ;
					}
			void	push_back( _Tp * aptr )
					{
						if ( last_ ) { last_ -> next_= aptr, aptr-> prev_= last_ ; }
							else { first_= aptr, aptr-> prev_= nullptr ; }
						last_= aptr, last_ -> next_= nullptr ;
					}

			iterator	begin(void) { return iterator( first_ ) ; }
			iterator	end(void) { return iterator() ; }

			reverse_iterator	rbegin(void) { return reverse_iterator( last_ ) ; }
			reverse_iterator	rend(void) { return reverse_iterator() ; }

			const_iterator	begin(void) const { return const_iterator( first_ ) ; }
			const_iterator	end(void) const { return const_iterator() ; }

			/*
			const_reverse_iterator	rbegin(void) const { return const_reverse_iterator( last_ ) ; }
			const_reverse_iterator	rend(void) const { return const_reverse_iterator() ; }

			const_reverse_iterator	crbegin(void) { return const_reverse_iterator( last_ ) ; }
			const_reverse_iterator	crend(void) { return const_reverse_iterator() ; }
			*/

			bool	empty(void) const { return ( nullptr == first_ ) ; }
			size_t	size(void) const {
						size_t	ct ;
						const_iterator	iptr( begin()) ;

						for ( ct= 0 ; ( iptr() ) ; ++ iptr, ++ ct ) { }
						return ct ;
					}

			void	clear(void) {
						iterator	iptr( begin()) ;
						while ( iptr() ) { delete * iptr ;  ++ iptr ; }
					}

		public:

			class	const_iterator : public _constBase
			{
				public:
					const_iterator	operator++() { step() ;  return * this ; }
					const_iterator	operator++(int) { iterator itmp( * this) ;  step() ;  return itmp ; }

				public:
					const_iterator() : _constBase( nullptr) { }
					const_iterator( _Tp * aptr, _Tp * alim = nullptr) : _constBase( aptr, alim)
						{ next_ = ( _constBase::limit_ != _constBase::ptr_ ) ? _constBase::ptr_-> next() : _constBase::limit_ ; }

					const_iterator( const const_iterator & asrc ) = default ;

					void	step( void) { _constBase::ptr_= next_ ;  if ( _constBase::limit_ != next_ ) { listable::step( next_ ) ; } }

				private:
					const _Tp *	next_ = nullptr ;
			} ;

			class	reverse_iterator : public _Base
			{
				public:
					reverse_iterator	operator++() { rstep() ;  return * this ; }
					reverse_iterator	operator++(int) { reverse_iterator itmp( * this) ;  rstep() ;  return itmp ; }

				public:
					reverse_iterator() : _Base( nullptr) { }
					reverse_iterator( _Tp * aptr, _Tp * alim = nullptr) : _Base( aptr, alim)
						{ prev_ = ( _Base::limit_ != _Base::ptr_ ) ? _Base::ptr_-> prior() : _Base::limit_ ; }

					reverse_iterator( const reverse_iterator & asrc ) = default ;

					void	rstep( void) { _Base::ptr_= prev_ ;  if ( _Base::limit_ != prev_ ) { listable::back( prev_ ) ; } }

				private:
					_Tp *	prev_ = nullptr ;
			} ;

		private:
			_Tp		* first_ , * last_ ;
	} ;

} ;

		// things to add (copying std::list)
		//
		//	merge()
		//	splice()
		//	remove()
		//	reverse()
		//	unique()
		//	sort()
		//

#endif

