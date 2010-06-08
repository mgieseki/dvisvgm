/*************************************************************************
** Terminal.cpp                                                         **
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

#include "Terminal.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_TERMIOS_H
#include <termios.h>
#endif

#ifdef GWINSZ_IN_SYS_IOCTL
#include <sys/ioctl.h>
#endif

#ifdef __WIN32__
#include <windows.h>
#endif

#include <cstdio>


using namespace std;


#ifdef __WIN32__
int Terminal::_defaultColor;
const int Terminal::RED     = FOREGROUND_RED;
const int Terminal::GREEN   = FOREGROUND_GREEN;
const int Terminal::BLUE    = FOREGROUND_BLUE;
#else
const int Terminal::RED     = 1;
const int Terminal::GREEN   = 2;
const int Terminal::BLUE    = 4;
#endif

const int Terminal::CYAN    = GREEN|BLUE;
const int Terminal::YELLOW  = RED|GREEN;
const int Terminal::MAGENTA = RED|BLUE;
const int Terminal::WHITE   = RED|GREEN|BLUE;
const int Terminal::DEFAULT = -1;
const int Terminal::BLACK   = 0;



void Terminal::init (ostream &os) {
#ifdef __WIN32__
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	if (h != INVALID_HANDLE_VALUE) {
		CONSOLE_SCREEN_BUFFER_INFO info;
		GetConsoleScreenBufferInfo(h, &info);
		_defaultColor = (info.wAttributes & 0xff);
	}
#endif
}


void Terminal::finish (ostream &os) {
	color(DEFAULT, false, os);
}


/** Returns the number of terminal columns (number of characters per row).
 *  If it's not possible to retrieve information about the terminal size, 0 is returned. */
int Terminal::columns () {
#if defined(TIOCGWINSZ)
	struct winsize ws;
	if (ioctl(fileno(stderr), TIOCGWINSZ, &ws) < 0)
		return 0;
	return ws.ws_col;
#elif defined(__WIN32__)
	CONSOLE_SCREEN_BUFFER_INFO info;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &info))
		return 0;
	return info.dwSize.X;
#else
	return 0;
#endif
}


/** Returns the number of terminal rows.
 *  If it's not possible to retrieve information about the terminal size, 0 is returned. */
int Terminal::rows () {
#if defined(TIOCGWINSZ)
	struct winsize ws;
	if (ioctl(fileno(stderr), TIOCGWINSZ, &ws) < 0)
		return 0;
	return ws.ws_row;
#elif defined(__WIN32__)
	CONSOLE_SCREEN_BUFFER_INFO info;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &info))
		return 0;
	return info.dwSize.Y;
#else
	return 0;
#endif
}


void Terminal::color (int color, bool light, ostream &os) {
#ifdef __WIN32__
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	if (h != INVALID_HANDLE_VALUE) {
		if (color == DEFAULT)
			color = _defaultColor;
		else {
			CONSOLE_SCREEN_BUFFER_INFO info;
			GetConsoleScreenBufferInfo(h, &info);
			color = (info.wAttributes & 0xf0) | (color & 0x07);
			if (light)
				color |= FOREGROUND_INTENSITY;
		}
		SetConsoleTextAttribute(h, (DWORD)color);
	}
#else
	if (color == DEFAULT)
		os << "\e[0m";
	else
		os << "\e[" << (light ? '1': '0') << ';' << (30+(color & 0x07)) << 'm';
#endif
}


void Terminal::bgcolor (int color, bool light, ostream &os) {
#ifdef __WIN32__
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	if (h != INVALID_HANDLE_VALUE) {
		if (color == DEFAULT)
			color = _defaultColor;
		else {
			CONSOLE_SCREEN_BUFFER_INFO info;
			GetConsoleScreenBufferInfo(h, &info);
			color = (info.wAttributes & 0x0f) | ((color & 0x07) << 4);
			if (light)
				color |= BACKGROUND_INTENSITY;
		}
		SetConsoleTextAttribute(h, (DWORD)color);
	}
#else
	if (color == DEFAULT)
		os << "\e[0m";
	else
		os << "\e[" << (40+(color & 0x07)) << 'm';
#endif
}
