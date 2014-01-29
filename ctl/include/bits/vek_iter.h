// vek_iter.h -*- C++ -*-

#ifndef _CTL_VECTORITER
#define _CTL_VECTORITER	1

namespace ctl
{

	template <class T>
		class vector<T>::iterator : public __detail::vector_iter_b
		{
			public:
				constexpr iterator(data * abase, off_t::const_iterator itr ) noexcept : __detail::vector_iter_b::vector_iter_b( abase, itr )
				{ }

				operator const_iterator() const { return const_iterator( pbase_, it_) ; }

				iterator &	operator++() noexcept { step() ;  return * this ; }
				iterator	operator++(int) { iterator itmp( * this ) ;  step() ;  return itmp ; }

				iterator &	operator+=(int aval) noexcept { jump( aval) ;  return * this ; }
				iterator &	operator-=(int aval) noexcept { jump( - aval) ;  return * this ; }

				iterator	operator+(difference_type adelt)
							{ return const_iterator( pbase_, it_ + adelt ) ; }
				iterator	operator-(difference_type adelt)
							{ return const_iterator( pbase_, it_ - adelt ) ; }

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
				constexpr const_iterator(data * abase, off_t::const_iterator itr ) noexcept : __detail::vector_iter_b::vector_iter_b( abase, itr )
				{ }

				const_iterator &	operator++() noexcept { step() ;  return * this ; }
				const_iterator		operator++(int) { const_iterator itmp( * this ) ;  step() ;  return itmp ; }

				const_iterator &	operator+=(int aval) noexcept { jump( aval) ;  return * this ; }
				const_iterator &	operator-=(int aval) noexcept { jump( - aval) ;  return * this ; }

				const_iterator		operator+(difference_type adelt)
									{ return const_iterator( pbase_, it_ + adelt ) ; }
				const_iterator		operator-(difference_type adelt)
									{ return const_iterator( pbase_, it_ - adelt ) ; }

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

			protected:
				data *					location(void) const noexcept { return pbase_ + ( * it_ ) ; }
				off_t::const_iterator &	iter(void) noexcept { return it_ ; }

				friend class vector<T> ;
		} ;

	template <class T>
		class vector<T>::range : public __detail::vector_range_b
		{
			public:
				range( data * abase, off_t & aoff ) : __detail::vector_range_b( abase, aoff ) { }

				operator const_range() const { return const_range( pbase_, it_, itb_, ite_ ) ; }

				range &	operator++() noexcept { step() ;  return * this ; }
				range & operator--() noexcept { back() ;  return * this ; }
				range &	operator+=(int aval) noexcept { jump( aval) ;  return * this ; }
				range &	operator-=(int aval) noexcept { jump( -aval) ;  return * this ; }

				range 	remain(void) const { return range( pbase_, it_, it_, ite_ ) ; }

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

				iterator	begin(void) const { return iterator( pbase_, itb_ ) ; }
				iterator	end(void) const { return iterator( pbase_, ite_ ) ; }

			protected:
				using oit = off_t::const_iterator ;

				range( data * abase, oit apt, const oit & abegin, const oit & aend ) : __detail::vector_range_b( abase, apt, abegin, aend ) { }
		} ;

	template <class T>
		class vector<T>::const_range : public __detail::vector_range_b
		{
			public:
				const_range( data * abase, const off_t & aoff ) : __detail::vector_range_b( abase, aoff ) { }
				const_range( const_iterator & afir, const_iterator & alim ) : __detail::vector_range_b( afir, alim ) { }

				operator const_iterator() const { return const_iterator( pbase_, it_ ) ; }

				range &	operator++() noexcept { step() ;  return * this ; }

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

				const_iterator	begin(void) const { return const_iterator( pbase_, itb_ ) ; }
				const_iterator	end(void) const { return const_iterator( pbase_, ite_ ) ; }

			protected:
				const off_t::const_iterator &	ibegin(void) const noexcept { return itb_ ; }
				const off_t::const_iterator &	iend(void) const noexcept { return ite_ ; }

				friend class vector<T> ;
		} ;

} ;

#endif

