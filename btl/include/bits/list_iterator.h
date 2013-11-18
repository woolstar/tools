// list -*- C++ -*-

#ifndef _BTL_DETAIL_ITER_H
#define _BTL_DETAIL_ITER_H 1

#include <iterator>

namespace btl
{
	namespace __detail
	{
		template < typename _Tp > struct	_List_iterator_Base
		{
			_List_iterator_Base( _Tp * aptr) : ptr_( aptr) { }

			_Tp & 	operator*() const { return * ptr_ ; }
			_Tp * 	operator->() const { return ptr_ ; }

			bool		operator==(_List_iterator_Base & ai) const { return ai.ptr_ == ptr_ ; }
			bool		operator!=(_List_iterator_Base & ai) const { return ai.ptr_ != ptr_ ; }
			bool		operator==(const _List_iterator_Base & ai) const { return ai.ptr_ == ptr_ ; }
			bool		operator!=(const _List_iterator_Base & ai) const { return ai.ptr_ != ptr_ ; }

			typedef int		difference_type ;
			typedef _Tp		value_type ;
			typedef _Tp *	pointer ;
			typedef _Tp &	reference ;

			typedef std::forward_iterator_tag	iterator_category ;

			_Tp *	ptr_ ;
		} ;

		template < typename _Tp > class	_List_range_Base
		{
			private:
				class	IsValid { void operator delete(void *) ; } ;

			public:
				_List_range_Base( _Tp * aptr, _Tp * alim = nullptr ) : ptr_( aptr), limit_( alim) { }

				operator const IsValid *() const {
					static IsValid _sentinal ;
					return (( limit_ != ptr_ ) && ( nullptr != ptr_ )) ? & _sentinal : nullptr ;
				}
				// bool	operator()() const { return (( limit_ != ptr_ ) && ( nullptr != ptr_ )) ; }

				_Tp & 	operator*() const { return * ptr_ ; }
				_Tp * 	operator->() const { return ptr_ ; }

				bool		operator==(_List_range_Base & ai) const { return ai.ptr_ == ptr_ ; }
				bool		operator!=(_List_range_Base & ai) const { return ai.ptr_ != ptr_ ; }
				bool		operator==(const _List_range_Base & ai) const { return ai.ptr_ == ptr_ ; }
				bool		operator!=(const _List_range_Base & ai) const { return ai.ptr_ != ptr_ ; }

				typedef int		difference_type ;
				typedef _Tp		value_type ;
				typedef _Tp *	pointer ;
				typedef _Tp &	reference ;

				typedef std::forward_iterator_tag	iterator_category ;

				_Tp *	ptr_ ;
				_Tp	* const limit_ ;

		} ;
	} ;
} ;

#endif

