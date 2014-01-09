// cu_vector -*- C++ -*-

#ifndef	_CU_BITS_VECTOR
#define _CU_BITS_VECTOR

#include <memory>

namespace ctl
{
		// cucumbered polymorphic vector

	template <class T> class vector
	{
		public:
			vector() { }
			vector(size_t asize) : storage_( new unsigned char[ asize ] ), m_total( asize ) { }

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

			bool empty() const noexcept ;
			size_t size() const noexcept ;
			size_t capacity() const noexcept ;
			
			void	shrink_fit( void ) ;

		private:
				// storage
			size_t	m_use = 0, m_total = 0 ;
			std::unique_ptr<unsigned char []>	storage_ ;

			typedef	__detail::vector_ctrl_base< T>	_ctrlBase ;
	} ;

} ;

#endif
