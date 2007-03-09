/***********************************************************************
** Font.cpp                                                           **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2007 Martin Gieseking <martin.gieseking@uos.de> **
**                                                                    **
** This program is free software; you can redistribute it and/or      **
** modify it under the terms of the GNU General Public License        **
** as published by the Free Software Foundation; either version 2     **
** of the License, or (at your option) any later version.             **
**                                                                    **
** This program is distributed in the hope that it will be useful,    **
** but WITHOUT ANY WARRANTY; without even the implied warranty of     **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      **
** GNU General Public License for more details.                       **
**                                                                    **
** You should have received a copy of the GNU General Public License  **
** along with this program; if not, write to the Free Software        **
** Foundation, Inc., 51 Franklin Street, Fifth Floor,                 **
** Boston, MA 02110-1301, USA.                                        **
***********************************************************************/
// $Id$

#include <iostream>
#include "Font.h"
#include "KPSFileFinder.h"
#include "TFM.h"

using namespace std;


static TFM* create_tfm (string name) {
	TFM *tfm = TFM::createFromFile(name.c_str());
	if (tfm)
		return tfm;
	throw FontException("can't find "+name+".tfm");
}



Font* PhysicalFont::create (string name, UInt32 checksum, double dsize, double ssize, PhysicalFont::Type type) {
	return new PhysicalFontImpl(name, checksum, dsize, ssize, type);
}


Font* VirtualFont::create (string name, UInt32 checksum, double dsize, double ssize) {
	return new VirtualFontImpl(name, checksum, dsize, ssize);
}

//////////////////////////////////////////////////////////////////////////////

PhysicalFontImpl::PhysicalFontImpl (string name, UInt32 cs, double ds, double ss, PhysicalFont::Type type) 
	: _tfm(0), _name(name), checksum(cs), dsize(ds), ssize(ss)
{
}


PhysicalFontImpl::~PhysicalFontImpl () {
	delete _tfm;
}


const TFM* PhysicalFontImpl::getTFM () const {
	if (!_tfm) 
		_tfm = create_tfm(_name);
	return _tfm;
}


double PhysicalFontImpl::charWidth (int c) const  {return getTFM()->getCharWidth(c);} 
double PhysicalFontImpl::charDepth (int c) const  {return getTFM()->getCharDepth(c);} 
double PhysicalFontImpl::charHeight (int c) const {return getTFM()->getCharHeight(c);} 

//////////////////////////////////////////////////////////////////////////////

VirtualFontImpl::VirtualFontImpl (string name, UInt32 cs, double ds, double ss) 
	: _tfm(0), _name(name), checksum(cs), dsize(ds), ssize(ss)
{
}


VirtualFontImpl::~VirtualFontImpl () {
	delete _tfm;
}


const TFM* VirtualFontImpl::getTFM () const {
	if (!_tfm) 
		_tfm = create_tfm(_name);
	return _tfm;
}


double VirtualFontImpl::charWidth (int c) const  {return getTFM()->getCharWidth(c);} 
double VirtualFontImpl::charDepth (int c) const  {return getTFM()->getCharDepth(c);} 
double VirtualFontImpl::charHeight (int c) const {return getTFM()->getCharHeight(c);} 


int VirtualFontImpl::fontID (int n) const {
	return 0; // @@
}

int VirtualFontImpl::firstFontNum () const {
	return 0; // @@
}

UInt8* VirtualFontImpl::getDVI (int c) const {
	return 0; // @@
}
