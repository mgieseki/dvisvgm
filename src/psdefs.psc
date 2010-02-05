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
"ize[10000 10000]def end setpagedevice/:gsave systemdict/gsave get def/:grestore "
"systemdict/grestore get def/@dodraw true store/prval{dup type/stringtype eq{prin"
"t}{30 string cvs print}ifelse}def/prseq{-1 1{-1 roll prval( )print}for(\\n)print"
"}def/prcmd{( )exch(\\ndvi.)3{print}repeat prseq}def/cvxall{{cvx}forall}def/sysex"
"ec{systemdict exch get exec}def/defpr{[exch[/copy systemdict]cvxall 5 -1 roll du"
"p 6 1 roll[/get/exec]cvxall 6 -1 roll dup 7 1 roll 4 -1 roll dup 5 1 roll dup le"
"ngth string cvs/prcmd cvx]cvx def}def/setpos{currentpoint dup type cvlit/boolean"
"type eq{pop}{2(setpos)prcmd}ifelse}def/prpath{{2(moveto)prcmd}{2(lineto)prcmd}{6"
"(curveto)prcmd}{0(closepath)prcmd}pathforall}def/charpath{/@dodraw false store/c"
"harpath sysexec/@dodraw true store}def/show{@dodraw{true charpath eofill 0(show)"
"prcmd}if}def/stroke{@dodraw{0(newpath)prcmd prpath 0(stroke)prcmd newpath}{/stro"
"ke sysexec}ifelse}def/fill{@dodraw{0(newpath)prcmd prpath 0(fill)prcmd newpath}{"
"/fill sysexec}ifelse}def/eofill{@dodraw{0(newpath)prcmd prpath 0(eofill)prcmd ne"
"wpath}{/eofill sysexec}ifelse}def/clip{/clip sysexec 0(newpath)prcmd prpath 0(cl"
"ip)prcmd}def/eoclip{/eoclip sysexec 0(newpath)prcmd prpath 0(eoclip)prcmd}def/in"
"itclip 0 defpr/adddot{dup length 1 add string dup 0 46 put dup 3 -1 roll 1 exch "
"putinterval}def/setlinewidth 1 defpr/setlinecap 1 defpr/setlinejoin 1 defpr/setm"
"iterlimit 1 defpr/setdash{mark 3 1 roll 2 copy/setdash sysexec exch aload length"
" 1 add -1 roll counttomark(setdash)prcmd pop}def/gsave 0 defpr/grestore{:grestor"
"e currentlinewidth 1(setlinewidth)prcmd currentlinecap 1(setlinecap)prcmd curren"
"tlinejoin 1(setlinejoin)prcmd currentmiterlimit 1(setmiterlimit)prcmd currentrgb"
"color 3(setrgbcolor)prcmd 6 array currentmatrix aload pop 6(setmatrix)prcmd curr"
"entdash mark 3 1 roll exch aload length 1 add -1 roll counttomark(setdash)prcmd "
"pop 0(grestore)prcmd}def/rotate{dup type/arraytype ne{dup 1(rotate)prcmd}if/rota"
"te sysexec}def/scale{dup type/arraytype ne{2 copy 2(scale)prcmd}if/scale sysexec"
"}def/translate{dup type/arraytype ne{2 copy 2(translate)prcmd}if/translate sysex"
"ec}def/setmatrix{dup/setmatrix sysexec aload pop 6(setmatrix)prcmd}def/initmatri"
"x{matrix setmatrix}def/concat{matrix currentmatrix matrix concatmatrix setmatrix"
"}def/setgray 1 defpr/setcmykcolor 4 defpr/sethsbcolor 3 defpr/setrgbcolor 3 defp"
"r/.handleerror errordict/handleerror get def errordict begin/handleerror{0(begin"
"error)prcmd .handleerror 0(enderror)prcmd}.bind def end ";

// vim: set syntax=cpp:
