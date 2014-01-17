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
			class	const_iterator ;
			class	range ;
			class	const_range ;

			// storage

			template < class Tc, class... Args >
				void	emplace( const_iterator apos, Args&&... arg ) ;
			template < class Tc, class... Args >
				void	emplace_back( Args&&... arg ) ;
			template < class Tc >
				void	transfer_back( Tc && aref ) ;
			template < class Tc >
				tank<T> &	operator<<( Tc && aref ) { transfer_back( aref) ;  return * this ; }

			iterator	erase(iterator pos) ;

			// accessors

			T &	at(unsigned int apos) ;
			const T &	at(unsigned int apos) const ;

			T &	front() ;
			const T &	front() const ;

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
			size_t size() const noexcept { return use_ ; }
			size_t capacity() const noexcept { return total_ ; }
			
			void	shrink_fit( void ) ;

		private:
			typedef	__detail::tank_ctrl_common< T>	_ctrlBase ;

			void	relocate(data * asrc, data * alimit, data * ddest ) override ;
	} ;

} ;

#endif

