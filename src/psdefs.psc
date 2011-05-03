/*************************************************************************
** psdefs.psc                                                           **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2011 Martin Gieseking <martin.gieseking@uos.de>   **
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
"ll dup length string cvs/prcmd cvx]cvx def}put @SD/setpos{{currentpoint}stopped{"
"$error/newerror false put}{2(setpos)prcmd}ifelse}put @SD/applyscalevals{1 0 tran"
"sform 0 0 transform 3 -1 roll sub dup mul 3 1 roll sub dup mul add sqrt 0 1 tran"
"sform 0 0 transform 3 -1 roll sub dup mul 3 1 roll sub dup mul add sqrt 1 0 tran"
"sform dup mul exch dup dup mul 3 -1 roll add sqrt div 3(applyscalevals)prcmd}def"
" @SD/prpath{{2(moveto)prcmd}{2(lineto)prcmd}{6(curveto)prcmd}{0(closepath)prcmd}"
"pathforall}put @SD/charpath{/@dodraw false store :charpath/@dodraw true store}pu"
"t @SD/show{@dodraw{true charpath eofill}if}put @SD/stroke{@dodraw{0(newpath)prcm"
"d prpath 0(stroke)prcmd newpath}{:stroke}ifelse}put @SD/fill{@dodraw{0(newpath)p"
"rcmd prpath 0(fill)prcmd newpath}{:fill}ifelse}put @SD/eofill{@dodraw{0(newpath)"
"prcmd prpath 0(eofill)prcmd newpath}{:eofill}ifelse}put @SD/clip{:clip 0(newpath"
")prcmd prpath 0(clip)prcmd}put @SD/eoclip{:eoclip 0(newpath)prcmd prpath 0(eocli"
"p)prcmd}put false setglobal @SD readonly pop/initclip 0 defpr/sysexec{@SD exch g"
"et exec}def/adddot{dup length 1 add string dup 0 46 put dup 3 -1 roll 1 exch put"
"interval}def/setlinewidth{dup/setlinewidth sysexec applyscalevals 1(setlinewidth"
")prcmd}def/setlinecap 1 defpr/setlinejoin 1 defpr/setmiterlimit 1 defpr/setdash{"
"mark 3 1 roll 2 copy/setdash sysexec applyscalevals exch aload length 1 add -1 r"
"oll counttomark(setdash)prcmd pop}def/gsave 0 defpr/grestore{:grestore currentli"
"newidth 1(setlinewidth)prcmd currentlinecap 1(setlinecap)prcmd currentlinejoin 1"
"(setlinejoin)prcmd currentmiterlimit 1(setmiterlimit)prcmd currentrgbcolor 3(set"
"rgbcolor)prcmd 6 array currentmatrix aload pop 6(setmatrix)prcmd currentdash mar"
"k 3 1 roll exch aload length 1 add -1 roll counttomark(setdash)prcmd pop 0(grest"
"ore)prcmd}def/rotate{dup type/arraytype ne{dup 1(rotate)prcmd}if/rotate sysexec}"
"def/scale{dup type/arraytype ne{2 copy 2(scale)prcmd}if/scale sysexec}def/transl"
"ate{dup type/arraytype ne{2 copy 2(translate)prcmd}if/translate sysexec}def/setm"
"atrix{dup/setmatrix sysexec aload pop 6(setmatrix)prcmd}def/initmatrix{matrix se"
"tmatrix}def/concat{matrix currentmatrix matrix concatmatrix setmatrix}def/setgra"
"y 1 defpr/setcmykcolor 4 defpr/sethsbcolor 3 defpr/setrgbcolor 3 defpr/.setopaci"
"tyalpha{dup/.setopacityalpha sysexec 1(setopacityalpha)prcmd}def/.handleerror er"
"rordict/handleerror get def errordict begin/handleerror{0(beginerror)prcmd .hand"
"leerror 0(enderror)prcmd}.bind def end ";

// vim: set syntax=cpp:
