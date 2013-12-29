
	////	General String
	//
	//	Safe operations on unix strings
	//

#ifndef	GENERALSTRING_DEF
#	define	GENERALSTRING_DEF	1

#include "general.h"

class	generalstring
{
	public:

			// copy source to dest and terminate
		static void	ncopy(char * adest, const char * asource, unsigned long asize) ;

		static bool	iswhite(char c)	{ return ((' ' == c) || ('\t' == c) || ('\n' == c) || ('\r' == c)) ; }

			// skip over one word
		static void skip(const char * & astring)
		{
			char c;

			ASSERTPOINTER(astring) ;

				// skip non-white
			while ((c= *astring) && ! iswhite(c)) astring ++ ;
				// skip white
			while ((c= *astring) && iswhite(c)) astring ++ ;
		}
		static void	skip_word(const char * & apointer) { skip(apointer); }

			// skip past white space
		static void space(const char * & astring)
		{
			char c;

			ASSERTPOINTER(astring) ;

				// skip white
			while ((c= *astring) && iswhite(c)) astring ++ ;
		}
		static void	skip_space(const char * & apointer) { space(apointer); }

			// capture one word and terminate, skip ahead
		static void	ncaptureword(char * adest, const char * & asource, unsigned long asize, char asep = '\0') ;

			// capture entire line except for trailing white space
		static void ncaptureparam(char * adest, const char * asource, unsigned long asize) ;

			// make uppercase
		static void	toupper(char *) ;
			// make lowercase
		static void	tolower(char *) ;

			// trip trailing spaces
		static void	trim_space(char *) ;

			// form string from sequence
		static void	sequence(char * adest, const char * asource, unsigned int anum) ;
} ;


	// shortcuts
#define		copyKnown(aaa, bbb)	ncopy(aaa, bbb, sizeof(aaa))
#define		captureKnown(aaa, bbb)	ncaptureword(aaa, bbb, sizeof(aaa))
#define		paramK(aaa, bbb) ncaptureparam(aaa, bbb, sizeof(aaa))
#define		paramKnown(aaa, bbb) ncaptureparam(aaa, bbb, sizeof(aaa))
#define		captureKnownSep(aaa, bbb, kk)	ncaptureword(aaa, bbb, sizeof(aaa), kk)

#endif