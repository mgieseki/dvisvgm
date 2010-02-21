/*************************************************************************
** CharmapTranslator.h                                                  **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2010 Martin Gieseking <martin.gieseking@uos.de>   **
**                                                                      **
** This program is free software; you can redistribute it and/or        ** 
** modify it under the terms of the GNU General Public License as       **
** published by the Free Software Foundation; either version 3 of       **
** the License, or (at your option) any later version.                  **
**                                                                      **
** This program is distributed in the hope that it will be useful, but  **
** WITHOUT ANY WARRANTY; without even the implied warranty of           **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         **
** GNU General Public License for more details.                         **
**                                                                      **
** You should have received a copy of the GNU General Public License    **
** along with this program; if not, see <http://www.gnu.org/licenses/>. ** 
*************************************************************************/

#ifndef CHARMAPTRANSLATOR_H
#define CHARMAPTRANSLATOR_H

#include <map>
#include "types.h"


class FileFinder;
class Font;

class CharmapTranslator
{
	typedef std::map<UInt32,UInt32>::iterator Iterator;
	typedef std::map<UInt32,UInt32>::const_iterator ConstIterator;
   public:
		CharmapTranslator () {}
      CharmapTranslator (const Font *font);
		void setFont (const Font *font);
		UInt32 unicode (UInt32 customCode) const;

   private:
		std::map<UInt32,UInt32> translationMap;
};

#endif
