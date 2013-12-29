#include <general.h>

#include <time.h>

	// constructor
generallog::generallog(const char * aFile)
{
	generalstring::copyKnown(name, aFile) ;
}


void	generallog::add(const char * amessage)
{
	FILE * f= fopen(name, "a") ;

	if (f)
	{
		fputs(amessage, f) ;
		fclose(f) ;
	}
}

	// message()
	//	write message to appropriate log
void	generallog::message(const char * amessage, ErrorLevel alevel)
{
	time_t	curtime ;
	struct tm * timerec ;

	generallog	* curptr ;

	if (eFatal == alevel)
	{
			// confirm error file exists
		if (! error) error= new generallog("errorlog.txt") ;
		curptr= error ;
	}
	else
	{
			// ignore masked messages
		if (testmask(alevel))
			return ;

			// confirm general message file exists
		if (! log) log= new generallog("log.txt") ;
		curptr= log ;
	}

	if (curptr)
	{
		char	msg[400] ;
			// get time for timestamp
		time(& curtime);  timerec= localtime(& curtime) ;
			// format message
#if 0
		sprintf(msg, "%04d/%02d/%02d %02d%02d%02d - %s\n",
			1900 + timerec-> tm_year, timerec-> tm_mon, timerec-> tm_mday,
			timerec-> tm_hour, timerec-> tm_min, timerec-> tm_sec,
			amessage) ;
#else
		sprintf(msg, "\"%04d%02d%02d\",\"%02d:%02d:%02d\",\"%s\",\"%s\"\n",
			1900 + timerec-> tm_year, 1 +timerec-> tm_mon, timerec-> tm_mday,
			timerec-> tm_hour, timerec-> tm_min, timerec-> tm_sec,
			strlevel(alevel),
			amessage) ;
#endif
			// output
		curptr-> add(msg) ;
	}
}

	// setErrorFile()
	// test and set error_file
 void	generallog::setErrorFile(const char * aname)
{
	FILE * ftest= fopen(aname, "a") ;

	if (NULL == ftest)
		return ;

	fclose(ftest) ;
	if (error) delete error ;
	error= new generallog(aname) ;
}

	// setLogFile()
	// test and set log_file
 void	generallog::setLogFile(const char * aname)
{
	FILE * ftest= fopen(aname, "a") ;

	if (NULL == ftest)
		return ;

	fclose(ftest) ;
	if (log) delete log ;
	log= new generallog(aname) ;
}

	// maskLevel()
	// disable messages of selected type (not eFatal)
 void	generallog::maskLevel(ErrorLevel el)
 {
	 if (eFatal != el)
		 smessagemask |= (1 << el) ;
 }

void	generallog::unmaskLevel(ErrorLevel el)
{
	 if (eFatal != el)
		 smessagemask &= ~ (1 << el) ;
}

	// strlevel()
	// convert error level to string
char	* generallog::strlevel(ErrorLevel alevel)
{
	static char	name[32] ;

	switch (alevel)
	{
		case eDebug:   strcpy(name, "SYS_DEBUG") ;  break;
		case eWarning:   strcpy(name, "SYS_WARNING") ;  break;
		case eNotice:   strcpy(name, "SYS_EVENT") ;  break;
		case eFatal:   strcpy(name, "SYS_ERROR") ;  break;
		default:   strcpy(name, "SYS_UNDEF") ;  break;
	}

	return name ;
}


	// testmask
	// determine if specific messages should be ignored
bool	generallog::testmask(ErrorLevel	el)
{
	if (eFatal == el) return false ;

	return (0 != (smessagemask & (1 << el))) ;
}

	///// static file names
generallog	* generallog::error= NULL ;
generallog	* generallog::log= NULL ;

unsigned long	generallog::smessagemask = 0 ;