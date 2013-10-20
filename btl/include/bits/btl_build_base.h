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

			virtual sized_storage * getbuffer(void) = 0 ;
			virtual sized_storage const *	getbuffer(void) const = 0 ;

			void	copy(const buffer &) ;
			void	copy(scanner<sized_storage> &) ;

			friend sized_storage * begin(build_base &) ;
			const friend sized_storage * begin(const build_base &) ;
			friend sized_storage * end(build_base &) ;
			const friend sized_storage * end(const build_base &) ;

			sized_storage * limit_ ;

		// take care of all operations that affect fill_
			void	reset(void) { datasize_ = 0 ;  fill_ = (sized_storage *) rawbuffer_ ; }
			void	reduce(void) { if ( datasize_ ) { datasize_ --, fill_ -- ; } }
			void	term(void) { if ( fill_ != limit_ ) { *fill_ = '\0' ; } }
			void	jump(int aoff) { if ( aoff < 0 ) { if ( aoff >= datasize_ ) { reset() ; } else { datasize_ += aoff ;  fill_ += aoff ; } } else { if ( aoff > remaining() ) { aoff= remaining() ; }  datasize_ += aoff ;  fill_ += aoff ; } }

			sized_storage *	fill_get(void) const { return fill_ ; }
			void		fill_relocate(sized_storage * afill) { fill_ = afill ; }

			friend	io ;

		public:
			size_t	remaining(void) const { return limit_ - fill_ ; }

		private:
				// fill_ = rawbuffer_ + datasize_ :: so keeping it in sync requires care
			sized_storage * fill_ ;
	} ;

	inline sized_storage *	begin(build_base & __va) { return __va.getbuffer() ; }
	inline const sized_storage *	begin(const build_base & __va) { return __va.getbuffer() ; }

	inline sized_storage *	end(build_base & __va) { return __va.fill_ ; } 
	inline const sized_storage *	end(const build_base & __va) { return __va.fill_ ; } 
} ;

#endif

