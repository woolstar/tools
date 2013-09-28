#ifndef _EXPAND_TRAITS_H
#define _EXPAND_TRAITS_H 1

namespace btl
{
		// buffer traits

			// keep buffer restricted to initial size
	class	expand_fixed : virtual public build_base
		{ protected: void expand(int) { } } ;

			// exception on buffer overflow
	class	expand_strict : virtual public build_base
		{ protected: void expand(int) { throw std::length_error("exceeded buffer") ; } } ;

			// attempt to reallocate larger buffer each time
	class	expand_alloc : virtual public build_base
	{
		protected:
			void expand(int) ; 
			std::unique_ptr<sized_storage []>	storage_ ;

			sized_storage *	getbuffer(void) { return & ( storage_[0] ) ; }
			sized_storage const *	getbuffer(void) const { return & ( storage_[0] ) ; }
	} ;

			// add methods
	template <class TRAIT>
		class	build_methods : virtual public build_base, public TRAIT
		{
			public:
				build_methods<TRAIT> &	add(const buffer & abuf) 
					{ test( abuf.size()) ;  copy( abuf) ;  return * this ; }
				build_methods<TRAIT> &	add(scanner<sized_storage> & ascan)
					{ test( ascan.size() ) ;  copy( ascan) ;  return * this ; }
				build_methods<TRAIT> &	add(const char * astr)
				{
					int alen= strlen( astr) ;
					test( alen) ;
					copy( buffer( astr, alen)) ;
					return * this ;
				}

				build_methods<TRAIT> & operator<<(const buffer & abuf) { return add( abuf) ; }
				build_methods<TRAIT> & operator<<(build_scanner & ascan) { return add( ascan) ; }
				build_methods<TRAIT> & operator<<(const char * astr) { return add( astr) ; }

				build_methods<TRAIT> &	reset(void) { build_base::reset() ;  return * this ; }
				build_methods<TRAIT> &	chomp()  { reduce() ;  return * this ; }
				build_methods<TRAIT> &	terminate() { term() ;  return * this ; }

			protected:
				build_methods() {}	// gets ignored

				void	test(size_t) ;
		} ;

	template <class TRAIT> void	build_methods<TRAIT>::test(size_t asize)
	{
		if ( remaining() < asize ) 
			{ TRAIT::expand( asize - remaining()) ; }
	}
} ;

#endif

