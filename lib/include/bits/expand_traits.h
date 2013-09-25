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
				build_methods<TRAIT> &	add(const char * astr)
				{
					int alen= strlen( astr) ;
					test( alen) ;
					copy( buffer( astr, alen)) ;
					return * this ;
				}

				build_methods<TRAIT> & operator<<(const buffer & abuf) { return add( abuf) ; }
				build_methods<TRAIT> & operator<<(const char * astr) { return add( astr) ; }

				build_methods<TRAIT> &	chomp() 
					{ if ( datasize_ ) { datasize_ -- ; fill_ -- ; } return * this; } 
				build_methods<TRAIT> &	terminate()
					{ if ( fill_ < limit_ ) { * fill_= '\0' ; }  return * this ; }

				size_t	remaining(void) const { return limit_ - fill_ ; }

			protected:
				build_methods() {}	// gets ignored

				void	test(size_t) ;
		} ;

	template <class TRAIT> void	build_methods<TRAIT>::test(size_t asize)
	{
		if ( ( fill_ + asize ) > limit_ )
			{ TRAIT::expand( asize - ( limit_ - fill_ )) ; }
	}
} ;

#endif

