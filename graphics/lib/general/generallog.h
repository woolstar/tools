//	General Log
//

#ifndef	GENERALLOG_DEF
#	define	GENERALLOG_DEF	1

class	generallog
{
	public:
		generallog(const char * name) ;

		enum	ErrorLevel
		{
			eDebug= 1,
			eWarning= 2,
			eNotice= 0,
			eFatal= -1,
		} ;

		void	add(const char *amessage) ;

		static void	message(const char * amessage, ErrorLevel = eNotice) ;

		static void	setErrorFile(const char *) ;
		static void	setLogFile(const char *) ;
		static void	maskLevel(ErrorLevel) ;
		static void	unmaskLevel(ErrorLevel) ;

	protected:
		static char	* strlevel(ErrorLevel) ;

		static	generallog	* error ;
		static	generallog	* log ;

		static	bool	testmask(ErrorLevel) ;
		static	unsigned long	smessagemask ;

		char	name[256] ;
} ;


#endif	// GENERALLOG_DEF