// ===========================================================================
// A String List Container.
// ===========================================================================

#ifndef	GENERALSTRINGLIST_DEF
#	define	GENERALSTRINGLIST_DEF	1

class	generaloutput ;

class	generalstringlist
{
	public:
		generalstringlist(unsigned int asize = 0) ;
		generalstringlist(const generalstringlist &) ;
		~ generalstringlist() ;

			// list operations
		void	add(const char *) ;
		void	remove(unsigned int) ;
		void	clear(void) { muse= 0, mcount= 0; }

		void	copy(const generalstringlist &) ;

		unsigned int	getcount(void) const { return mcount; }

		char *	tostring(char = '\n') const ;
		char *	tostring(const char *) const ;
		char *	join(const char *) const ;

		void	print(FILE * fdest, const char * aformat = "%s\n") const ;
		void	print(generaloutput *, const char * aformat = "%s\n") const ;

		const char *	operator[](int) const;

	private:
		unsigned int	muse, mmax,
								mcount, mcountmax ;
		char			* mbuffer ;
		unsigned int	* moffset ;

		bool		resize(unsigned int) ;
		bool		resize_offset(unsigned int) ;
} ;

#endif
