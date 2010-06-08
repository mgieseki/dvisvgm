/*************************************************************************
** psdefs.psc                                                           **
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

const char *PSInterpreter::PSDEFS =
"3 dict dup begin/Install{matrix setmatrix}def/HWResolution[72.27 72.27]def/PageS"
"ize[10000 10000]def end setpagedevice/@dodraw true store/@SD systemdict def true"
" setglobal @SD/:gsave @SD/gsave get put @SD/:grestore @SD/grestore get put @SD/:"
"stroke @SD/stroke get put @SD/:fill @SD/fill get put @SD/:eofill @SD/eofill get "
"put @SD/:clip @SD/clip get put @SD/:eoclip @SD/eoclip get put @SD/.setopacityalp"
"ha known not{@SD/.setopacityalpha{pop}put}if @SD/prval{dup type/stringtype eq{pr"
"int}{30 string cvs print}ifelse}put @SD/prseq{-1 1{-1 roll prval( )print}for(\\n"
")print}put @SD/prcmd{( )exch(\\ndvi.)3{print}repeat prseq}put @SD/cvxall{{cvx}fo"
"rall}put @SD/defpr{[exch[/copy @SD]cvxall 5 -1 roll dup 6 1 roll[/get/exec]cvxal"
"l 6 -1 roll dup 7 1 roll 4 -1 roll dup 5 1 roll dup length string cvs/prcmd cvx]"
"cvx def}put @SD/setpos{currentpoint dup type cvlit/booleantype eq{pop}{2(setpos)"
"prcmd}ifelse}put @SD/prpath{{2(moveto)prcmd}{2(lineto)prcmd}{6(curveto)prcmd}{0("
"closepath)prcmd}pathforall}put @SD/charpath{/@dodraw false store/charpath sysexe"
"c/@dodraw true store}put @SD/show{@dodraw{true charpath eofill 0(show)prcmd}if}p"
"ut @SD/stroke{@dodraw{0(newpath)prcmd prpath 0(stroke)prcmd newpath}{:stroke}ife"
"lse}put @SD/fill{@dodraw{0(newpath)prcmd prpath 0(fill)prcmd newpath}{:fill}ifel"
"se}put @SD/eofill{@dodraw{0(newpath)prcmd prpath 0(eofill)prcmd newpath}{:eofill"
"}ifelse}put @SD/clip{:clip 0(newpath)prcmd prpath 0(clip)prcmd}put @SD/eoclip{:e"
"oclip 0(newpath)prcmd prpath 0(eoclip)prcmd}put false setglobal @SD readonly pop"
"/initclip 0 defpr/sysexec{@SD exch get exec}def/adddot{dup length 1 add string d"
"up 0 46 put dup 3 -1 roll 1 exch putinterval}def/setlinewidth 1 defpr/setlinecap"
" 1 defpr/setlinejoin 1 defpr/setmiterlimit 1 defpr/setdash{mark 3 1 roll 2 copy/"
"setdash sysexec exch aload length 1 add -1 roll counttomark(setdash)prcmd pop}de"
"f/gsave 0 defpr/grestore{:grestore currentlinewidth 1(setlinewidth)prcmd current"
"linecap 1(setlinecap)prcmd currentlinejoin 1(setlinejoin)prcmd currentmiterlimit"
" 1(setmiterlimit)prcmd currentrgbcolor 3(setrgbcolor)prcmd 6 array currentmatrix"
" aload pop 6(setmatrix)prcmd currentdash mark 3 1 roll exch aload length 1 add -"
"1 roll counttomark(setdash)prcmd pop 0(grestore)prcmd}def/rotate{dup type/arrayt"
"ype ne{dup 1(rotate)prcmd}if/rotate sysexec}def/scale{dup type/arraytype ne{2 co"
"py 2(scale)prcmd}if/scale sysexec}def/translate{dup type/arraytype ne{2 copy 2(t"
"ranslate)prcmd}if/translate sysexec}def/setmatrix{dup/setmatrix sysexec aload po"
"p 6(setmatrix)prcmd}def/initmatrix{matrix setmatrix}def/concat{matrix currentmat"
"rix matrix concatmatrix setmatrix}def/setgray 1 defpr/setcmykcolor 4 defpr/seths"
"bcolor 3 defpr/setrgbcolor 3 defpr/.setopacityalpha{dup/.setopacityalpha sysexec"
" 1(setopacityalpha)prcmd}def/.handleerror errordict/handleerror get def errordic"
"t begin/handleerror{0(beginerror)prcmd .handleerror 0(enderror)prcmd}.bind def e"
"nd ";

// vim: set syntax=cpp:
