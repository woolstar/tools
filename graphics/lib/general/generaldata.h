//	General Data (array)
//

#ifndef	GENERALDATA_DEF
#	define	GENERALDATA_DEF	1


class	garray
{
	public:
		garray(unsigned int = 0) ;
		virtual ~ garray() ;

		unsigned int	getCount(void) const { return mcount; }

		void		size(unsigned int) ;
		void		clear(void) { mcount=0 ; }
		bool		add(void *) ;

		void		move(garray &) ;

	protected:

		void		* get(unsigned int) const;

		unsigned int	mcount, mmax ;
		void		** container ;

} ;

template <class GT> class tarray : public garray
{
	public:
		GT * operator[](unsigned int aindex) const{ return (GT *) get(aindex); }
} ;

#endif	// GENERALDATA_DEF
