/***********************************************************************
** psdefs.psc                                                         **
**                                                                    **
** This file is part of dvisvgm -- the DVI to SVG converter           **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de> **
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

const char *PSInterpreter::PSDEFS = 
"3 dict dup begin/Install{matrix setmatrix}def/HWResolution[72.27 72.27]def/PageS"
"ize[10000 10000]def end setpagedevice/:gsave systemdict/gsave get def/:grestore "
"systemdict/grestore get def/prval{dup type/stringtype eq{print}{30 string cvs pr"
"int}ifelse}def/prseq{-1 1{-1 roll prval( )print}for(\\n)print}def/prcmd{( )exch("
"\\ndvi.)3{print}repeat prseq}def/cvxall{{cvx}forall}def/sysexec{systemdict exch "
"get exec}def/defpr{[exch[/copy systemdict]cvxall 5 -1 roll dup 6 1 roll[/get/exe"
"c]cvxall 6 -1 roll dup 7 1 roll 4 -1 roll dup 5 1 roll dup length string cvs/prc"
"md cvx]cvx def}def/prpath{{2(moveto)prcmd}{2(lineto)prcmd}{6(curveto)prcmd}{0(cl"
"osepath)prcmd}pathforall}def/stroke{0(newpath)prcmd prpath 0(stroke)prcmd newpat"
"h}def/fill{0(newpath)prcmd prpath 0(fill)prcmd newpath}def/eofill{0(newpath)prcm"
"d prpath 0(eofill)prcmd newpath}def/clip{/clip sysexec :gsave clippath 0(newpath"
")prcmd prpath 0(clip)prcmd :grestore}def/eoclip{/eoclip sysexec :gsave clippath "
"0(newpath)prcmd prpath 0(eoclip)prcmd :grestore}def/initclip 0 defpr/adddot{dup "
"length 1 add string dup 0 46 put dup 3 -1 roll 1 exch putinterval}def/setlinewid"
"th 1 defpr/setlinecap 1 defpr/setlinejoin 1 defpr/setmiterlimit 1 defpr/setdash{"
"mark 3 1 roll 2 copy/setdash sysexec exch aload length 1 add -1 roll counttomark"
"(setdash)prcmd pop}def/gsave 0 defpr/grestore{:grestore currentlinewidth 1(setli"
"newidth)prcmd currentlinecap 1(setlinecap)prcmd currentlinejoin 1(setlinejoin)pr"
"cmd currentmiterlimit 1(setmiterlimit)prcmd currentrgbcolor 3(setrgbcolor)prcmd "
"6 array currentmatrix aload pop 6(setmatrix)prcmd currentdash mark 3 1 roll exch"
" aload length 1 add -1 roll counttomark(setdash)prcmd pop 0(grestore)prcmd}def/s"
"how{dup/show sysexec currentpoint 3 -1 roll 3(show)prcmd}def/rotate{dup type/arr"
"aytype ne{dup 1(rotate)prcmd}if/rotate sysexec}def/scale{dup type/arraytype ne{2"
" copy 2(scale)prcmd}if/scale sysexec}def/translate{dup type/arraytype ne{2 copy "
"2(translate)prcmd}if/translate sysexec}def/setmatrix{dup/setmatrix sysexec aload"
" pop 6(setmatrix)prcmd}def/initmatrix{matrix setmatrix}def/concat{matrix current"
"matrix matrix concatmatrix setmatrix}def/setgray 1 defpr/setcmykcolor 4 defpr/se"
"thsbcolor 3 defpr/setrgbcolor 3 defpr/splitarray{dup length 1 sub 0 exch 1 exch{"
"2 copy dup 4 mod 0 eq{5 -1 roll dup 6 1 roll print}if get prval( )print 1 add 4 "
"mod 0 eq{(\\n)print}if}for pop pop}def/.handleerror errordict/handleerror get de"
"f errordict begin/handleerror{0(beginerror)prcmd .handleerror 0(enderror)prcmd}d"
"ef end ";

// vim: set syntax=cpp:
