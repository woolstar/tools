#ifndef _EXPAND_TRAITS_H
#define _EXPAND_TRAITS_H 1

namespace btl
{
		// buffer traits

			// keep buffer restricted to initial size
	class	expand_fixed : virtual public build_base
	{
		public: enum _traits { fixed = 1, exceptions = 0 } ;
		protected: void expand(int) { }
	} ;

			// exception on buffer overflow
	class	expand_strict : virtual public build_base
	{
		public: enum _traits { fixed = 1, exceptions = 1 } ;
		protected: void expand(int) { throw std::length_error("exceeded buffer") ; }
	} ;

			// attempt to reallocate larger buffer each time
	class	expand_alloc : virtual public build_base
	{
		public:
			enum _traits { fixed = 0, exceptions = 1 } ;

		protected:
			void expand(int) ; 
			std::unique_ptr<sized_storage []>	storage_ ;

			sized_storage *	getbuffer(void) { return & ( storage_[0] ) ; }
			sized_storage const *	getbuffer(void) const { return & ( storage_[0] ) ; }
	} ;

			// add methods
	template <class Trait>
		class	build_methods : virtual public build_base, public Trait
		{
			public:
				build_methods<Trait> &	add(const buffer & abuf) 
					{ test( abuf.size()) ;  copy( abuf) ;  return * this ; }
				build_methods<Trait> &	add(scanner<> & ascan)
					{ test( ascan.size() ) ;  copy( ascan) ;  return * this ; }
				build_methods<Trait> &	add(scanner<> & ascan, size_t asz )
					{ test( asz ) ;  copy( ascan, asz ) ;  return * this ; }
				build_methods<Trait> &	add(const char * astr)
				{
					int alen= strlen( astr) ;
					test( alen) ;
					copy( buffer( astr, astr + alen)) ;
					return * this ;
				}

				build_methods<Trait> & operator<<(const buffer & abuf) { return add( abuf) ; }
				build_methods<Trait> & operator<<(build_scanner & ascan) { return add( ascan) ; }
				build_methods<Trait> & operator<<(const char * astr) { return add( astr) ; }

				build_methods<Trait> &	reset(void) { build_base::reset() ;  return * this ; }
				build_methods<Trait> &	chomp()  { reduce() ;  return * this ; }
				build_methods<Trait> &	terminate() { term() ;  return * this ; }

			protected:
				build_methods() {}	// gets ignored

				void	test(size_t) ;
		} ;

	template <class Trait> void	build_methods<Trait>::test(size_t asize)
	{
		if ( remaining() < asize ) 
			{ Trait::expand( asize - remaining()) ; }
	}
} ;

#endif

