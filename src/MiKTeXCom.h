#ifndef MIKTEXCOM_H
#define MIKTEXCOM_H

#include <string>

#ifdef _MSC_VER
#import <MiKTeX209-session.tlb>
using namespace MiKTeXSession2_9;
#else
#include "miktex209-session.h"
#endif


class MiKTeXCom
{
	public:
		MiKTeXCom ();
		~MiKTeXCom ();
		std::string getVersion ();
		const char* findFile (const char* fname);

	private:
#ifdef _MSC_VER
		ISession2Ptr _session;
#else
		ISession2 *_session;
#endif
};


#endif
