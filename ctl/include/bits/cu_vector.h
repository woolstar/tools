// cu_vector -*- C++ -*-

#ifndef	_CU_BITS_VECTOR
#define _CU_BITS_VECTOR

namespace ctl
{
		// cucumbered polymorphic vector

	template <class T> class vector : public __detail::vector_base
	{
		protected:
			using base = __detail::vector_base ;

		public:
			vector() { }
			vector(size_t asize) ;

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
				iterator	emplace_back( Args&&... arg ) ;
			template < class Tc >
				iterator	transfer_back( Tc && aref ) ;
			template < class Tc >
				vector<T> &	operator<<( Tc && aref ) { transfer_back( aref) ;  return * this ; }

			iterator	erase(const_iterator pos) ;
			iterator	erase(const_range aspan) ;
			iterator	erase(const_iterator astart, const_iterator aend) ;

			// accessors

			T & operator [](unsigned int apos) ;
			T &	at(unsigned int apos) ;
			const T &	at(unsigned int apos) const ;

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

			// tests

			bool empty() const noexcept { return ! use_ ; }
			size_t size() const noexcept { return base::offsets_.size() ; }
			size_t capacity() const noexcept { return base::offsets_.capacity() ; }
			
			void	reserve(unsigned int) ;
			void	shrink_fit( void ) ;
			void	swap( vector& other ) ;

		private:
			void	relocate(data * asrc, data * alimit, data * ddest ) override ;
	} ;

} ;

#endif

