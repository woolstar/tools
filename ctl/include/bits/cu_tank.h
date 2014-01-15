// cu_tank -*- C++ -*-

#ifndef	_CU_BITS_TANK
#define _CU_BITS_TANK

namespace ctl
{
		// cucumbered polymorphic vector like tank

	template <class T> class tank : public __detail::tank_base
	{
		public:
			tank() { }
			tank(size_t asize) : tank_base( asize) { }

			typedef	T	base_type ;
			typedef	T	value_type ;

			class	iterator ;
			class	reverse_iterator ;
			class	range ;

			// storage

			void	clear(void) ;
			template < class Tc, class... Args >
				void	emplace( const iterator apos, Args&&... arg ) ;
			template < class Tc, class... Args >
				void	emplace_back( Args&&... arg ) ;
			template < class Tc >
				void	transfer_back( Tc && aref ) ;

			// accessors

			T &	at(size_t apos) ;
			const T &	at(size_t apos) const ;

			// T & operator[](size_t apost) ;
			// const T &	operator[](size_t apost) const ;

			T &	front() ;
			const T &	front() const ;
			T & back() ;
			const T &	back() const ;

			// iterators

			iterator	begin(void) noexcept ;
			const iterator	begin(void) const noexcept ;
			const iterator	cbegin(void) const noexcept ;

			iterator	end(void) noexcept ;
			const iterator	end(void) const noexcept ;
			const iterator	cend(void) const noexcept ;

			range	span(void) noexcept ;
			const range	span(void) const noexcept ;
			const range	cspan(void) const noexcept ;

			// tests

			bool empty() const noexcept { return ! use_ ; }
			size_t size() const noexcept { return use_ ; }
			size_t capacity() const noexcept { return total_ ; }
			
			void	shrink_fit( void ) ;

		private:
			typedef	__detail::tank_ctrl_common< T>	_ctrlBase ;
	} ;

} ;

#endif

