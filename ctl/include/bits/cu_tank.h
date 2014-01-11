// cu_vector -*- C++ -*-

#ifndef	_CU_BITS_VECTOR
#define _CU_BITS_VECTOR

namespace ctl
{
		// cucumbered polymorphic vector

	template <class T> class vector : public __detail::vector_base
	{
		public:
			vector() { }
			vector(size_t asize) : vector_base( asize) { }

			typedef	T	base_type ;
			typedef	T	value_type ;

			class	iterator ;
			class	reverse_iterator ;
			class	range ;

			// storage

			void	clear(void) noexcept ;
			template < class Tc, class... Args >
				void	emplace( const iterator apos, Args&&... arg ) ;
			template < class Tc, class... Args >
				void	emplace_back( Args&&... arg ) ;
			template < class Tc >
				void	transfer_back( Tc && aref ) ;

			// accessors

			T &	at(size_t apos) ;
			const T &	at(size_t apos) const ;

			T & operator[](size_t apost) ;
			const T &	operator[](size_t apost) const ;

			T &	front() ;
			const T &	front() const ;
			T & back() ;
			const T &	back() const ;

			// iterators

			iterator	begin(void) ;
			const iterator	begin(void) const ;
			const iterator	cbegin(void) const ;

			iterator	end(void) ;
			const iterator	end(void) const ;
			const iterator	cend(void) const ;

			range	span(void) ;
			const range	span(void) const ;
			const range	cspan(void) const ;

			// tests

			bool empty() const noexcept { return ! use_ ; }
			size_t size() const noexcept { return use_ ; }
			size_t capacity() const noexcept { return total_ ; }
			
			void	shrink_fit( void ) ;

		private:
			typedef	__detail::vector_ctrl_base< T>	_ctrlBase ;
	} ;

} ;

#endif
