#ifndef	GENERALHASH_DEF
#	define	GENERALHASH_DEF	1

class	generalhash
{
	public:
		generalhash() ;

		void	data(const void *, unsigned long) ;
		unsigned long	getcrc(void) { return lv; }

	protected:
		unsigned long	lv ;
		int	cycle ;

		static const int pattern[]; 
} ;

#endif	// GENERALHASH_DEF