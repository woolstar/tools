#ifndef	GENERALOBJECT_DEF
#	define	GENERALOBJECT_DEF	1

class	generalobject
{
	public:
		generalobject() ;
		virtual ~ generalobject() ;

		virtual void	checkself(const char *) const ;

} ;

#endif	// GENERALOBJECT_DEF