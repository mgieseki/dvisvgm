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
"put @SD/:clip @SD/clip get put @SD/:eoclip @SD/eoclip get put @SD/:charpath @SD/"
"charpath get put @SD/.setopacityalpha known not{@SD/.setopacityalpha{pop}put}if "
"@SD/prval{dup type/stringtype eq{print}{30 string cvs print}ifelse}put @SD/prseq"
"{-1 1{-1 roll prval( )print}for(\\n)print}put @SD/prcmd{( )exch(\\ndvi.)3{print}"
"repeat prseq}put @SD/cvxall{{cvx}forall}put @SD/defpr{[exch[/copy @SD]cvxall 5 -"
"1 roll dup 6 1 roll[/get/exec]cvxall 6 -1 roll dup 7 1 roll 4 -1 roll dup 5 1 ro"
"ll dup length string cvs/prcmd cvx]cvx def}put @SD/setpos{currentpoint dup type "
"cvlit/booleantype eq{pop}{2(setpos)prcmd}ifelse}put @SD/prpath{{2(moveto)prcmd}{"
"2(lineto)prcmd}{6(curveto)prcmd}{0(closepath)prcmd}pathforall}put @SD/charpath{/"
"@dodraw false store :charpath/@dodraw true store}put @SD/show{@dodraw{true charp"
"ath eofill}if}put @SD/stroke{@dodraw{0(newpath)prcmd prpath 0(stroke)prcmd newpa"
"th}{:stroke}ifelse}put @SD/fill{@dodraw{0(newpath)prcmd prpath 0(fill)prcmd newp"
"ath}{:fill}ifelse}put @SD/eofill{@dodraw{0(newpath)prcmd prpath 0(eofill)prcmd n"
"ewpath}{:eofill}ifelse}put @SD/clip{:clip 0(newpath)prcmd prpath 0(clip)prcmd}pu"
"t @SD/eoclip{:eoclip 0(newpath)prcmd prpath 0(eoclip)prcmd}put false setglobal @"
"SD readonly pop/initclip 0 defpr/sysexec{@SD exch get exec}def/adddot{dup length"
" 1 add string dup 0 46 put dup 3 -1 roll 1 exch putinterval}def/setlinewidth 1 d"
"efpr/setlinecap 1 defpr/setlinejoin 1 defpr/setmiterlimit 1 defpr/setdash{mark 3"
" 1 roll 2 copy/setdash sysexec exch aload length 1 add -1 roll counttomark(setda"
"sh)prcmd pop}def/gsave 0 defpr/grestore{:grestore currentlinewidth 1(setlinewidt"
"h)prcmd currentlinecap 1(setlinecap)prcmd currentlinejoin 1(setlinejoin)prcmd cu"
"rrentmiterlimit 1(setmiterlimit)prcmd currentrgbcolor 3(setrgbcolor)prcmd 6 arra"
"y currentmatrix aload pop 6(setmatrix)prcmd currentdash mark 3 1 roll exch aload"
" length 1 add -1 roll counttomark(setdash)prcmd pop 0(grestore)prcmd}def/rotate{"
"dup type/arraytype ne{dup 1(rotate)prcmd}if/rotate sysexec}def/scale{dup type/ar"
"raytype ne{2 copy 2(scale)prcmd}if/scale sysexec}def/translate{dup type/arraytyp"
"e ne{2 copy 2(translate)prcmd}if/translate sysexec}def/setmatrix{dup/setmatrix s"
"ysexec aload pop 6(setmatrix)prcmd}def/initmatrix{matrix setmatrix}def/concat{ma"
"trix currentmatrix matrix concatmatrix setmatrix}def/setgray 1 defpr/setcmykcolo"
"r 4 defpr/sethsbcolor 3 defpr/setrgbcolor 3 defpr/.setopacityalpha{dup/.setopaci"
"tyalpha sysexec 1(setopacityalpha)prcmd}def/.handleerror errordict/handleerror g"
"et def errordict begin/handleerror{0(beginerror)prcmd .handleerror 0(enderror)pr"
"cmd}.bind def end ";

// vim: set syntax=cpp:
