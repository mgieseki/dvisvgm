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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Font.h"
#include "KPSFileFinder.h"
#include "Message.h"
#include "TFM.h"
#include "VFReader.h"
#include "macros.h"

using namespace std;


TFMFont::TFMFont (string name, UInt32 cs, double ds, double ss)
	: tfm(0), fontname(name), checksum(cs), dsize(ds), ssize(ss)
{
}


TFMFont::~TFMFont () {
	delete tfm;
}


const TFM* TFMFont::getTFM () const {
	if (!tfm) {
		tfm = TFM::createFromFile(fontname.c_str());
		if (!tfm)
			throw FontException("can't find "+fontname+".tfm");
	}
	return tfm;
}


double TFMFont::charWidth (int c) const  {return getTFM()->getCharWidth(c);}
double TFMFont::charDepth (int c) const  {return getTFM()->getCharDepth(c);}
double TFMFont::charHeight (int c) const {return getTFM()->getCharHeight(c);}

//////////////////////////////////////////////////////////////////////////////

Font* PhysicalFont::create (string name, UInt32 checksum, double dsize, double ssize, PhysicalFont::Type type) {
	return new PhysicalFontImpl(name, checksum, dsize, ssize, type);
}


Font* VirtualFont::create (string name, UInt32 checksum, double dsize, double ssize) {
	return new VirtualFontImpl(name, checksum, dsize, ssize);
}

//////////////////////////////////////////////////////////////////////////////

PhysicalFontImpl::PhysicalFontImpl (string name, UInt32 cs, double ds, double ss, PhysicalFont::Type type) 
	: TFMFont(name, cs, ds, ss)
{
}


//////////////////////////////////////////////////////////////////////////////

VirtualFontImpl::VirtualFontImpl (string name, UInt32 cs, double ds, double ss) 
	: TFMFont(name, cs, ds, ss)
{
}


VirtualFontImpl::~VirtualFontImpl () {
	for (map<UInt32, DVIVector*>::iterator i=charDefs.begin(); i != charDefs.end(); ++i)
		delete i->second;
}

void VirtualFontImpl::read (VFReader &vfr) {
	vfr.replaceActions(this);
	vfr.executeAll();
}


int VirtualFontImpl::fontID (int n) const {
	return 0; // @@
}

int VirtualFontImpl::firstFontNum () const {
	return 0; // @@
}

UInt8* VirtualFontImpl::getDVI (int c) const {
	return 0; // @@
}


void VirtualFontImpl::assignFontID (int fontnum, int id) {
	// @@
}


void VirtualFontImpl::defineFont (UInt32 fontnum, string name, UInt32 checksum, UInt32 dsize, UInt32 ssize) {
/*	SHOW(name);
	if (fontManager) {
		fontManager->enterVF(this);
		fontManager->registerFont(fontnum, name, checksum, dsize, ssize);
		fontManager->leaveVF();
	}*/
}


void VirtualFontImpl::defineChar (UInt32 c, UInt8 *dvi, UInt32 dvisize) {
	DVIVector *dvivec = new DVIVector(dvi, dvi+dvisize);
	charDefs[c] = dvivec;
}

/*
void VirtualFontImpl::readFontDefs () {
	vfReader.replaceActions(this);
	vfReader.executePreambleAndFontDefs();
}


void VirtualFontImpl::readCharDefs () {
//	vfReader.replaceActions(this);
//	vfReader.executeCharDefs();
}*/
