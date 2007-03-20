/************************************************
* Directory.h                                  *
*                                               *
* This file is part of dvisvgm                  *
* Copyright (c) 2005 by Martin Gieseking        *
************************************************/
// $Id$

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <string>
#ifdef __WIN32__
#include <windows.h>
#endif

class Directory
{
   public:
		Directory ();
      Directory (const std::string &path);
		~Directory ();
		bool open (const std::string &path);
		void close ();
		const char* read (char type='a');
		std::string getEntry () const;

   private:
		bool firstread;
#ifdef __WIN32__
		HANDLE handle;
		WIN32_FIND_DATA fileData;
#else
#endif
};

#endif
