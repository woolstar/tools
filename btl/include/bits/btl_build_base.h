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
			build_base(unsigned char * aptr, size_t amax) : buffer(aptr, 0), limit_(aptr + amax) { }
			virtual ~ build_base() ;

			size_t	remaining(void) const { return limit_ - static_cast<const sized_storage *>(far_) ; }
			void	reset(void) { far_= rawbuffer_ ; }
			void	print(const char * afmt, ...) ;

		protected:
			build_base() {}

			virtual sized_storage * getbuffer(void) = 0 ;
			virtual sized_storage const *	getbuffer(void) const = 0 ;

			void	copy(const buffer &) ;
			void	copy(scanner<> &) ;
			void	copy(scanner<> &, size_t) ;

			friend sized_storage * begin(build_base &) ;
			const friend sized_storage * begin(const build_base &) ;
			friend sized_storage * end(build_base &) ;
			const friend sized_storage * end(const build_base &) ;

			sized_storage * limit_ ;

		// take care of all operations that affect far_
			void	reduce(void) { if ( far_ != rawbuffer_ ) { far_= ((sized_storage * ) far_ ) -1 ; } }
			void	term(void) { if ( far_ != limit_ ) { * ((sized_storage *) far_) = '\0' ; } }
			void	jump(int aoff)
					{
						const sized_storage * ptr= static_cast<const sized_storage *>(far_) ;
						if ( aoff < 0 ) { if ( aoff >= size() ) { reset() ; } else { far_ = ptr + aoff ; } }
							else { if ( aoff > remaining() ) { aoff= remaining() ; }  far_ = ptr + aoff ; }
					}

			friend	io ;
	} ;

	inline sized_storage *	begin(build_base & T) { return T.getbuffer() ; }
	inline const sized_storage *	begin(const build_base & T) { return T.getbuffer() ; }

	inline sized_storage *	end(build_base & T) { return (sized_storage *) T.far_ ; }
	inline const sized_storage *	end(const build_base & T) { return static_cast<const sized_storage *>( T.far_ ) ; }
} ;

#endif

