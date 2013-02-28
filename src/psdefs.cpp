/*************************************************************************
** psdefs.cpp                                                           **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2013 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include "PSInterpreter.h"

const char *PSInterpreter::PSDEFS =
"3 dict dup begin/Install{matrix setmatrix}def/HWResolution[72 72]def/PageSize[10"
"000 10000]def end setpagedevice/@dodraw true store/@patcnt 0 store/@SD systemdic"
"t def/@UD userdict def true setglobal @SD/:save @SD/save get put @SD/:restore @S"
"D/restore get put @SD/:gsave @SD/gsave get put @SD/:grestore @SD/grestore get pu"
"t @SD/:grestoreall @SD/grestoreall get put @SD/:stroke @SD/stroke get put @SD/:f"
"ill @SD/fill get put @SD/:eofill @SD/eofill get put @SD/:clip @SD/clip get put @"
"SD/:eoclip @SD/eoclip get put @SD/:charpath @SD/charpath get put @SD/.setopacity"
"alpha known not{@SD/.setopacityalpha{pop}put}if @SD/prval{dup type/stringtype eq"
"{print}{30 string cvs print}ifelse}put @SD/prseq{-1 1{-1 roll prval( )print}for("
"\\n)print}put @SD/prcmd{( )exch(\\ndvi.)3{print}repeat prseq}put @SD/cvxall{{cvx"
"}forall}put @SD/defpr{[exch[/copy @SD]cvxall 5 -1 roll dup 6 1 roll[/get/exec]cv"
"xall 6 -1 roll dup 7 1 roll 4 -1 roll dup 5 1 roll dup length string cvs/prcmd c"
"vx]cvx def}put @SD/querypos{{currentpoint}stopped{$error/newerror false put}{2(q"
"uerypos)prcmd}ifelse}put @SD/applyscalevals{1 0 transform 0 0 transform 3 -1 rol"
"l sub dup mul 3 1 roll sub dup mul add sqrt 0 1 transform 0 0 transform 3 -1 rol"
"l sub dup mul 3 1 roll sub dup mul add sqrt 1 0 transform dup mul exch dup dup m"
"ul 3 -1 roll add sqrt div 3(applyscalevals)prcmd}def @SD/prpath{{2(moveto)prcmd}"
"{2(lineto)prcmd}{6(curveto)prcmd}{0(closepath)prcmd}pathforall}put @SD/charpath{"
"/@dodraw false store :charpath/@dodraw true store}put @SD/show{@dodraw{true char"
"path eofill}if}put @SD/stroke{@dodraw{0(newpath)prcmd prpath 0(stroke)prcmd newp"
"ath}{:stroke}ifelse}put @SD/fill{@dodraw{0(newpath)prcmd prpath 0(fill)prcmd new"
"path}{:fill}ifelse}put @SD/eofill{@dodraw{0(newpath)prcmd prpath 0(eofill)prcmd "
"newpath}{:eofill}ifelse}put @SD/clip{:clip 0(newpath)prcmd prpath 0(clip)prcmd}p"
"ut @SD/eoclip{:eoclip 0(newpath)prcmd prpath 0(eoclip)prcmd}put @SD/bop{pop pop}"
"put/@rect{4 -2 roll moveto exch dup 0 rlineto exch 0 exch rlineto neg 0 rlineto "
"closepath}bind def/@rectcc{4 -2 roll moveto 2 copy 0 lt exch 0 lt xor{dup 0 exch"
" rlineto exch 0 rlineto neg 0 exch rlineto}{exch dup 0 rlineto exch 0 exch rline"
"to neg 0 rlineto}ifelse closepath}bind def @SD/rectclip{newpath dup type/arrayty"
"pe eq{aload length 4 idiv{@rectcc}repeat}{@rectcc}ifelse clip newpath}put @SD/re"
"ctfill{gsave newpath dup type/arraytype eq{aload length 4 idiv{@rectcc}repeat}{@"
"rectcc}ifelse fill grestore}put @SD/rectstroke{gsave newpath dup type/arraytype "
"eq{aload length 4 idiv{@rect}repeat}{@rect}ifelse stroke grestore}put false setg"
"lobal @SD readonly pop/initclip 0 defpr/sysexec{@SD exch get exec}def/adddot{dup"
" length 1 add string dup 0 46 put dup 3 -1 roll 1 exch putinterval}def/setlinewi"
"dth{dup/setlinewidth sysexec applyscalevals 1(setlinewidth)prcmd}def/setlinecap "
"1 defpr/setlinejoin 1 defpr/setmiterlimit 1 defpr/setdash{mark 3 1 roll 2 copy/s"
"etdash sysexec applyscalevals exch aload length 1 add -1 roll counttomark(setdas"
"h)prcmd pop}def/setgstate{currentlinewidth 1(setlinewidth)prcmd currentlinecap 1"
"(setlinecap)prcmd currentlinejoin 1(setlinejoin)prcmd currentmiterlimit 1(setmit"
"erlimit)prcmd currentrgbcolor 3(setrgbcolor)prcmd 6 array currentmatrix aload po"
"p 6(setmatrix)prcmd currentdash mark 3 1 roll exch aload length 1 add -1 roll co"
"unttomark(setdash)prcmd pop}def/save{@UD begin/@saveID vmstatus pop pop def end "
":save @saveID 1(save)prcmd}def/restore{:restore setgstate @UD/@saveID known{@UD "
"begin @saveID end}{0}ifelse 1(restore)prcmd}def/gsave 0 defpr/grestore{:grestore"
" setgstate 0(grestore)prcmd}def/grestoreall{:grestoreall setstate 0(grestoreall)"
"prcmd}def/rotate{dup type/arraytype ne{dup 1(rotate)prcmd}if/rotate sysexec}def/"
"scale{dup type/arraytype ne{2 copy 2(scale)prcmd}if/scale sysexec}def/translate{"
"dup type/arraytype ne{2 copy 2(translate)prcmd}if/translate sysexec}def/setmatri"
"x{dup/setmatrix sysexec aload pop 6(setmatrix)prcmd}def/initmatrix{matrix setmat"
"rix}def/concat{matrix currentmatrix matrix concatmatrix setmatrix}def/makepatter"
"n{gsave <</mx 3 -1 roll >> begin dup/XUID[1000000 @patcnt]put mx/makepattern sys"
"exec dup dup begin PatternType @patcnt BBox aload pop XStep YStep PaintType mx a"
"load pop 15(makepattern)prcmd newpath matrix setmatrix PaintProc 0 1(makepattern"
")prcmd end/@patcnt @patcnt 1 add store end grestore}def/setpattern{begin Pattern"
"Type 1 eq{PaintType 1 eq{XUID aload pop exch pop 1}{:gsave[currentcolorspace alo"
"ad length -1 roll pop]setcolorspace/setcolor sysexec XUID aload pop exch pop cur"
"rentrgbcolor :grestore 4}ifelse(setpattern)prcmd}{/setpattern sysexec}ifelse end"
"}def/setcolor{dup type/dicttype eq{setpattern}{/setcolor sysexec}ifelse}def/setg"
"ray 1 defpr/setcmykcolor 4 defpr/sethsbcolor 3 defpr/setrgbcolor 3 defpr/.setopa"
"cityalpha{dup/.setopacityalpha sysexec 1(setopacityalpha)prcmd}def ";
