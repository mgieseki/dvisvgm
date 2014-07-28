/*************************************************************************
** psdefs.cpp                                                           **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2014 Martin Gieseking <martin.gieseking@uos.de>   **
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
"3 dict dup begin/Install{matrix setmatrix}def/HWResolution[72 72]def/PageSize["
"10000 10000]def end setpagedevice/@dodraw true store/@patcnt 0 store/@SD syste"
"mdict def/@UD userdict def true setglobal @SD/:save @SD/save get put @SD/:rest"
"ore @SD/restore get put @SD/:gsave @SD/gsave get put @SD/:grestore @SD/grestor"
"e get put @SD/:grestoreall @SD/grestoreall get put @SD/:newpath @SD/newpath ge"
"t put @SD/:stroke @SD/stroke get put @SD/:fill @SD/fill get put @SD/:eofill @S"
"D/eofill get put @SD/:clip @SD/clip get put @SD/:eoclip @SD/eoclip get put @SD"
"/:charpath @SD/charpath get put @SD/:show @SD/show get put @SD/.setopacityalph"
"a known not{@SD/.setopacityalpha{pop}put}if @SD/prseq{-1 1{-1 roll =only( )pri"
"nt}for(\\n)print}put @SD/prcmd{( )exch(\\ndvi.)3{print}repeat prseq}put @SD/cv"
"xall{{cvx}forall}put @SD/defpr{[exch[/copy @SD]cvxall 5 -1 roll dup 6 1 roll[/"
"get/exec]cvxall 6 -1 roll dup 7 1 roll 4 -1 roll dup 5 1 roll dup length strin"
"g cvs/prcmd cvx]cvx def}put @SD/querypos{{currentpoint}stopped{$error/newerror"
" false put}{2(querypos)prcmd}ifelse}put @SD/applyscalevals{1 0 transform 0 0 t"
"ransform 3 -1 roll sub dup mul 3 1 roll sub dup mul add sqrt 0 1 transform 0 0"
" transform 3 -1 roll sub dup mul 3 1 roll sub dup mul add sqrt 1 0 transform d"
"up mul exch dup dup mul 3 -1 roll add sqrt div 3(applyscalevals)prcmd}put @SD/"
"prpath{{2(moveto)prcmd}{2(lineto)prcmd}{6(curveto)prcmd}{0(closepath)prcmd}pat"
"hforall}put @SD/charpath{/@dodraw false store :charpath/@dodraw true store}put"
" @SD/show{@dodraw{dup :gsave currentpoint :newpath moveto true charpath eofill"
" :grestore/@dodraw false store :show/@dodraw true store}if}put @SD/newpath{:ne"
"wpath 0 1(newpath)prcmd}put @SD/stroke{@dodraw{1 1(newpath)prcmd prpath 0(stro"
"ke)prcmd :newpath}{:stroke}ifelse}put @SD/fill{@dodraw{1 1(newpath)prcmd prpat"
"h 0(fill)prcmd :newpath}{:fill}ifelse}put @SD/eofill{@dodraw{1 1(newpath)prcmd"
" prpath 0(eofill)prcmd :newpath}{:eofill}ifelse}put @SD/clip{:clip 0 1(newpath"
")prcmd prpath 0(clip)prcmd}put @SD/eoclip{:eoclip 1 1(newpath)prcmd prpath 0(e"
"oclip)prcmd}put @SD/bop{pop pop}put/@rect{4 -2 roll moveto exch dup 0 rlineto "
"exch 0 exch rlineto neg 0 rlineto closepath}bind def/@rectcc{4 -2 roll moveto "
"2 copy 0 lt exch 0 lt xor{dup 0 exch rlineto exch 0 rlineto neg 0 exch rlineto"
"}{exch dup 0 rlineto exch 0 exch rlineto neg 0 rlineto}ifelse closepath}bind d"
"ef @SD/rectclip{:newpath dup type/arraytype eq{aload length 4 idiv{@rectcc}rep"
"eat}{@rectcc}ifelse clip :newpath}put @SD/rectfill{gsave :newpath dup type/arr"
"aytype eq{aload length 4 idiv{@rectcc}repeat}{@rectcc}ifelse fill grestore}put"
" @SD/rectstroke{gsave :newpath dup type/arraytype eq{aload length 4 idiv{@rect"
"}repeat}{@rect}ifelse stroke grestore}put false setglobal @SD readonly pop/ini"
"tclip 0 defpr/clippath 0 defpr/sysexec{@SD exch get exec}def/adddot{dup length"
" 1 add string dup 0 46 put dup 3 -1 roll 1 exch putinterval}def/setlinewidth{d"
"up/setlinewidth sysexec applyscalevals 1(setlinewidth)prcmd}def/setlinecap 1 d"
"efpr/setlinejoin 1 defpr/setmiterlimit 1 defpr/setdash{mark 3 1 roll 2 copy/se"
"tdash sysexec applyscalevals exch aload length 1 add -1 roll counttomark(setda"
"sh)prcmd pop}def/setgstate{currentlinewidth 1(setlinewidth)prcmd currentlineca"
"p 1(setlinecap)prcmd currentlinejoin 1(setlinejoin)prcmd currentmiterlimit 1(s"
"etmiterlimit)prcmd currentrgbcolor 3(setrgbcolor)prcmd 6 array currentmatrix a"
"load pop 6(setmatrix)prcmd currentdash mark 3 1 roll exch aload length 1 add -"
"1 roll counttomark(setdash)prcmd pop}def/save{@UD begin/@saveID vmstatus pop p"
"op def end :save @saveID 1(save)prcmd}def/restore{:restore setgstate @UD/@save"
"ID known{@UD begin @saveID end}{0}ifelse 1(restore)prcmd}def/gsave 0 defpr/gre"
"store{:grestore setgstate 0(grestore)prcmd}def/grestoreall{:grestoreall setsta"
"te 0(grestoreall)prcmd}def/rotate{dup type/arraytype ne{dup 1(rotate)prcmd}if/"
"rotate sysexec}def/scale{dup type/arraytype ne{2 copy 2(scale)prcmd}if/scale s"
"ysexec}def/translate{dup type/arraytype ne{2 copy 2(translate)prcmd}if/transla"
"te sysexec}def/setmatrix{dup/setmatrix sysexec aload pop 6(setmatrix)prcmd}def"
"/initmatrix{matrix setmatrix}def/concat{matrix currentmatrix matrix concatmatr"
"ix setmatrix}def/makepattern{gsave <</mx 3 -1 roll >> begin dup/XUID[1000000 @"
"patcnt]put mx/makepattern sysexec dup dup begin PatternType @patcnt BBox aload"
" pop XStep YStep PaintType mx aload pop 15(makepattern)prcmd :newpath matrix s"
"etmatrix PaintProc 0 1(makepattern)prcmd end/@patcnt @patcnt 1 add store end g"
"restore}def/setpattern{begin PatternType 1 eq{PaintType 1 eq{XUID aload pop ex"
"ch pop 1}{:gsave[currentcolorspace aload length -1 roll pop]setcolorspace/setc"
"olor sysexec XUID aload pop exch pop currentrgbcolor :grestore 4}ifelse(setpat"
"tern)prcmd}{/setpattern sysexec}ifelse end}def/setcolor{dup type/dicttype eq{s"
"etpattern}{/setcolor sysexec}ifelse}def/setgray 1 defpr/setcmykcolor 4 defpr/s"
"ethsbcolor 3 defpr/setrgbcolor 3 defpr/.setopacityalpha{dup/.setopacityalpha s"
"ysexec 1(setopacityalpha)prcmd}def ";
