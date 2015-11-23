/*************************************************************************
** psdefs.cpp                                                           **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2015 Martin Gieseking <martin.gieseking@uos.de>   **
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
"<</Install{matrix setmatrix}/HWResolution[72 72]/PageSize[10000 10000]/Imaging"
"BBox null>>setpagedevice/@dodraw true store/@patcnt 0 store/@SD systemdict def"
"/@UD userdict def true setglobal @SD/:save @SD/save get put @SD/:restore @SD/r"
"estore get put @SD/:gsave @SD/gsave get put @SD/:grestore @SD/grestore get put"
" @SD/:grestoreall @SD/grestoreall get put @SD/:newpath @SD/newpath get put @SD"
"/:stroke @SD/stroke get put @SD/:fill @SD/fill get put @SD/:eofill @SD/eofill "
"get put @SD/:clip @SD/clip get put @SD/:eoclip @SD/eoclip get put @SD/:charpat"
"h @SD/charpath get put @SD/:show @SD/show get put @SD/.setopacityalpha known n"
"ot{@SD/.setopacityalpha{pop}put}if @SD/prseq{-1 1{-1 roll =only( )print}for(\\"
"n)print}put @SD/prcmd{( )exch(\\ndvi.)3{print}repeat prseq}put @SD/cvxall{{cvx"
"}forall}put @SD/defpr{[exch[/copy @SD]cvxall 5 -1 roll dup 6 1 roll[/get/exec]"
"cvxall 6 -1 roll dup 7 1 roll 4 -1 roll dup 5 1 roll dup length string cvs/prc"
"md cvx]cvx def}put @SD/querypos{{currentpoint}stopped{$error/newerror false pu"
"t}{2(querypos)prcmd}ifelse}put @SD/applyscalevals{1 0 transform 0 0 transform "
"3 -1 roll sub dup mul 3 1 roll sub dup mul add sqrt 0 1 transform 0 0 transfor"
"m 3 -1 roll sub dup mul 3 1 roll sub dup mul add sqrt 1 0 transform dup mul ex"
"ch dup dup mul 3 -1 roll add sqrt div 3(applyscalevals)prcmd}put @SD/prpath{{2"
"(moveto)prcmd}{2(lineto)prcmd}{6(curveto)prcmd}{0(closepath)prcmd}pathforall}p"
"ut @SD/charpath{/@dodraw false store :charpath/@dodraw true store}put @SD/show"
"{@dodraw{dup :gsave currentpoint 2{50 mul exch}repeat :newpath moveto 50 50/sc"
"ale sysexec true charpath eofill :grestore/@dodraw false store :show/@dodraw t"
"rue store}{pop}ifelse}put @SD/awidthshow{show pop pop pop pop pop}put @SD/newp"
"ath{:newpath 0 1(newpath)prcmd}put @SD/stroke{@dodraw{1 1(newpath)prcmd prpath"
" 0(stroke)prcmd :newpath}{:stroke}ifelse}put @SD/fill{@dodraw{1 1(newpath)prcm"
"d prpath 0(fill)prcmd :newpath}{:fill}ifelse}put @SD/eofill{@dodraw{1 1(newpat"
"h)prcmd prpath 0(eofill)prcmd :newpath}{:eofill}ifelse}put @SD/clip{:clip 0 1("
"newpath)prcmd prpath 0(clip)prcmd}put @SD/eoclip{:eoclip 1 1(newpath)prcmd prp"
"ath 0(eoclip)prcmd}put @SD/shfill{begin currentdict/ShadingType known currentd"
"ict/ColorSpace known and currentdict/DataSource known and currentdict/Function"
" known not and ShadingType 4 ge and DataSource type/arraytype eq and{<</Device"
"Gray 1/DeviceRGB 3/DeviceCMYK 4/bgknown currentdict/Background known/bbknown c"
"urrentdict/BBox known>>begin currentdict ColorSpace known{ShadingType ColorSpa"
"ce load bgknown{1 Background aload pop}{0}ifelse bbknown{1 BBox aload pop}{0}i"
"felse ShadingType 5 eq{VerticesPerRow}if DataSource aload length 4 add bgknown"
"{ColorSpace load add}if bbknown{4 add}if ShadingType 5 eq{1 add}if(shfill)prcm"
"d}if end}if end}put/@rect{4 -2 roll moveto exch dup 0 rlineto exch 0 exch rlin"
"eto neg 0 rlineto closepath}bind def/@rectcc{4 -2 roll moveto 2 copy 0 lt exch"
" 0 lt xor{dup 0 exch rlineto exch 0 rlineto neg 0 exch rlineto}{exch dup 0 rli"
"neto exch 0 exch rlineto neg 0 rlineto}ifelse closepath}bind def @SD/rectclip{"
":newpath dup type/arraytype eq{aload length 4 idiv{@rectcc}repeat}{@rectcc}ife"
"lse clip :newpath}put @SD/rectfill{gsave :newpath dup type/arraytype eq{aload "
"length 4 idiv{@rectcc}repeat}{@rectcc}ifelse fill grestore}put @SD/rectstroke{"
"gsave :newpath dup type/arraytype eq{aload length 4 idiv{@rect}repeat}{@rect}i"
"felse stroke grestore}put false setglobal @SD readonly pop/initclip 0 defpr/cl"
"ippath 0 defpr/sysexec{@SD exch get exec}def/adddot{dup length 1 add string du"
"p 0 46 put dup 3 -1 roll 1 exch putinterval}def/setlinewidth{dup/setlinewidth "
"sysexec applyscalevals 1(setlinewidth)prcmd}def/setlinecap 1 defpr/setlinejoin"
" 1 defpr/setmiterlimit 1 defpr/setdash{mark 3 1 roll 2 copy/setdash sysexec ap"
"plyscalevals exch aload length 1 add -1 roll counttomark(setdash)prcmd pop}def"
"/setgstate{currentlinewidth 1(setlinewidth)prcmd currentlinecap 1(setlinecap)p"
"rcmd currentlinejoin 1(setlinejoin)prcmd currentmiterlimit 1(setmiterlimit)prc"
"md currentrgbcolor 3(setrgbcolor)prcmd 6 array currentmatrix aload pop 6(setma"
"trix)prcmd currentdash mark 3 1 roll exch aload length 1 add -1 roll counttoma"
"rk(setdash)prcmd pop}def/save{@UD begin/@saveID vmstatus pop pop def end :save"
" @saveID 1(save)prcmd}def/restore{:restore setgstate @UD/@saveID known{@UD beg"
"in @saveID end}{0}ifelse 1(restore)prcmd}def/gsave 0 defpr/grestore{:grestore "
"setgstate 0(grestore)prcmd}def/grestoreall{:grestoreall setstate 0(grestoreall"
")prcmd}def/rotate{dup type/arraytype ne{dup 1(rotate)prcmd}if/rotate sysexec}d"
"ef/scale{dup type/arraytype ne{2 copy 2(scale)prcmd}if/scale sysexec}def/trans"
"late{dup type/arraytype ne{2 copy 2(translate)prcmd}if/translate sysexec}def/s"
"etmatrix{dup/setmatrix sysexec aload pop 6(setmatrix)prcmd}def/initmatrix{matr"
"ix setmatrix}def/concat{matrix currentmatrix matrix concatmatrix setmatrix}def"
"/makepattern{gsave<</mx 3 -1 roll>>begin dup/XUID[1000000 @patcnt]put mx/makep"
"attern sysexec dup dup begin PatternType @patcnt BBox aload pop XStep YStep Pa"
"intType mx aload pop 15(makepattern)prcmd :newpath matrix setmatrix PaintProc "
"0 1(makepattern)prcmd end/@patcnt @patcnt 1 add store end grestore}def/setpatt"
"ern{begin PatternType 1 eq{PaintType 1 eq{XUID aload pop exch pop 1}{:gsave[cu"
"rrentcolorspace aload length -1 roll pop]setcolorspace/setcolor sysexec XUID a"
"load pop exch pop currentrgbcolor :grestore 4}ifelse(setpattern)prcmd}{/setpat"
"tern sysexec}ifelse end}def/setcolor{dup type/dicttype eq{setpattern}{/setcolo"
"r sysexec}ifelse}def/setgray 1 defpr/setcmykcolor 4 defpr/sethsbcolor 3 defpr/"
"setrgbcolor 3 defpr/.setopacityalpha{dup/.setopacityalpha sysexec 1(setopacity"
"alpha)prcmd}def ";
