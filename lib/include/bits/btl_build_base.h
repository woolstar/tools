// build base -*- C++ -*-

#ifndef _BTL_BUILDBASE_H
#define _BTL_BUILDBASE_H 1

namespace btl
{
	typedef	unsigned char	sized_storage ;
	typedef scanner<sized_storage>	build_scanner ;

	class	build_base : public buffer
	{
		public:
			build_base(unsigned char * aptr, size_t amax) : buffer(aptr, 0), fill_(aptr), limit_(aptr + amax) { }
			virtual ~ build_base() ;

		protected:
			build_base() {}
			sized_storage * fill_, * limit_ ;

			virtual sized_storage * getbuffer(void) = 0 ;
			virtual sized_storage const *	getbuffer(void) const = 0 ;

			void	copy(const buffer &) ;
			void	copy(scanner<sized_storage> &) ;

			friend sized_storage * begin(build_base &) ;
			const friend sized_storage * begin(const build_base &) ;
			friend sized_storage * end(build_base &) ;
			const friend sized_storage * end(const build_base &) ;
	} ;

	inline sized_storage *	begin(build_base & __va) { return __va.getbuffer() ; }
	inline const sized_storage *	begin(const build_base & __va) { return __va.getbuffer() ; }

	inline sized_storage *	end(build_base & __va) { return __va.fill_ ; } 
	inline const sized_storage *	end(const build_base & __va) { return __va.fill_ ; } 
} ;

#endif
