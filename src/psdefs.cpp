/*************************************************************************
** psdefs.cpp                                                           **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2018 Martin Gieseking <martin.gieseking@uos.de>   **
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

#include "PSInterpreter.hpp"

const char *PSInterpreter::PSDEFS =
"<</Install{matrix setmatrix}/HWResolution[72 72]/PageSize[10000 10000]/Imaging"
"BBox null>>setpagedevice/@dodraw true store/@patcnt 0 store/@SD systemdict def"
"/@UD userdict def true setglobal @SD/:save @SD/save get put @SD/:restore @SD/r"
"estore get put @SD/:gsave @SD/gsave get put @SD/:grestore @SD/grestore get put"
" @SD/:grestoreall @SD/grestoreall get put @SD/:newpath @SD/newpath get put @SD"
"/:stroke @SD/stroke get put @SD/:fill @SD/fill get put @SD/:eofill @SD/eofill "
"get put @SD/:clip @SD/clip get put @SD/:eoclip @SD/eoclip get put @SD/:charpat"
"h @SD/charpath get put @SD/:show @SD/show get put @SD/.setopacityalpha known n"
"ot{@SD/.setopacityalpha{pop}put}if @SD/.setshapealpha known not{@SD/.setshapea"
"lpha{pop}put}if @SD/.setblendmode known not{@SD/.setblendmode{pop}put}if @SD/p"
"rseq{-1 1{-1 roll =only( )print}for(\\n)print}put @SD/prcmd{( )exch(\\ndvi.)3{"
"print}repeat prseq}put @SD/cvxall{{cvx}forall}put @SD/defpr{[exch[/copy @SD]cv"
"xall 5 -1 roll dup 6 1 roll[/get/exec]cvxall 6 -1 roll dup 7 1 roll 4 -1 roll "
"dup 5 1 roll dup length string cvs/prcmd cvx]cvx def}put @SD/querypos{{current"
"point}stopped{$error/newerror false put}{2(querypos)prcmd}ifelse}put @SD/apply"
"scalevals{1 0 dtransform exch dup mul exch dup mul add sqrt 0 1 dtransform exc"
"h dup mul exch dup mul add sqrt 1 0 dtransform dup mul exch dup dup mul 3 -1 r"
"oll add dup 0 eq{pop}{sqrt div}ifelse 3(applyscalevals)prcmd}put @SD/prpath{{2"
"(moveto)prcmd}{2(lineto)prcmd}{6(curveto)prcmd}{0(closepath)prcmd}pathforall}p"
"ut @SD/charpath{/@dodraw false store :charpath/@dodraw true store}put @SD/show"
"{@dodraw{dup :gsave currentpoint 2{50 mul exch}repeat :newpath moveto 50 50/sc"
"ale sysexec true charpath eofill :grestore/@dodraw false store :show/@dodraw t"
"rue store}{pop}ifelse}put @SD/awidthshow{{1 string dup 0 5 index put :gsave sh"
"ow :grestore pop 0 rmoveto 3 index eq{4 index 4 index rmoveto}if 1 index 1 ind"
"ex rmoveto}exch cshow 5{pop}repeat}put @SD/widthshow{0 0 3 -1 roll pstack awid"
"thshow}put @SD/ashow{0 0 0 6 3 roll awidthshow}put @SD/newpath{:newpath 0 1(ne"
"wpath)prcmd}put @SD/stroke{@dodraw{1 1(newpath)prcmd prpath 0(stroke)prcmd :ne"
"wpath}{:stroke}ifelse}put @SD/fill{@dodraw{1 1(newpath)prcmd prpath 0(fill)prc"
"md :newpath}{:fill}ifelse}put @SD/eofill{@dodraw{1 1(newpath)prcmd prpath 0(eo"
"fill)prcmd :newpath}{:eofill}ifelse}put @SD/clip{:clip 0 1(newpath)prcmd prpat"
"h 0(clip)prcmd}put @SD/eoclip{:eoclip 1 1(newpath)prcmd prpath 0(eoclip)prcmd}"
"put @SD/shfill{begin currentdict/ShadingType known currentdict/ColorSpace know"
"n and currentdict/DataSource known and currentdict/Function known not and Shad"
"ingType 4 ge and DataSource type/arraytype eq and{<</DeviceGray 1/DeviceRGB 3/"
"DeviceCMYK 4/bgknown currentdict/Background known/bbknown currentdict/BBox kno"
"wn>>begin currentdict ColorSpace known{ShadingType ColorSpace load bgknown{1 B"
"ackground aload pop}{0}ifelse bbknown{1 BBox aload pop}{0}ifelse ShadingType 5"
" eq{VerticesPerRow}if DataSource aload length 4 add bgknown{ColorSpace load ad"
"d}if bbknown{4 add}if ShadingType 5 eq{1 add}if(shfill)prcmd}if end}if end}put"
"/@rect{4 -2 roll moveto exch dup 0 rlineto exch 0 exch rlineto neg 0 rlineto c"
"losepath}bind def/@rectcc{4 -2 roll moveto 2 copy 0 lt exch 0 lt xor{dup 0 exc"
"h rlineto exch 0 rlineto neg 0 exch rlineto}{exch dup 0 rlineto exch 0 exch rl"
"ineto neg 0 rlineto}ifelse closepath}bind def @SD/rectclip{:newpath dup type/a"
"rraytype eq{aload length 4 idiv{@rectcc}repeat}{@rectcc}ifelse clip :newpath}p"
"ut @SD/rectfill{gsave :newpath dup type/arraytype eq{aload length 4 idiv{@rect"
"cc}repeat}{@rectcc}ifelse fill grestore}put @SD/rectstroke{gsave :newpath dup "
"type/arraytype eq{aload length 4 idiv{@rect}repeat}{@rect}ifelse stroke gresto"
"re}put false setglobal @SD readonly pop/initclip 0 defpr/clippath 0 defpr/syse"
"xec{@SD exch get exec}def/adddot{dup length 1 add string dup 0 46 put dup 3 -1"
" roll 1 exch putinterval}def/setlinewidth{dup/setlinewidth sysexec 1(setlinewi"
"dth)prcmd}def/setlinecap 1 defpr/setlinejoin 1 defpr/setmiterlimit 1 defpr/set"
"dash{mark 3 1 roll 2 copy/setdash sysexec exch aload length 1 add -1 roll coun"
"ttomark(setdash)prcmd pop}def/@setpagedevice{pop<<>>/setpagedevice sysexec[1 0"
" 0 -1 0 0]setmatrix newpath 0(setpagedevice)prcmd}def/printgstate{currentlinew"
"idth 1(setlinewidth)prcmd currentlinecap 1(setlinecap)prcmd currentlinejoin 1("
"setlinejoin)prcmd currentmiterlimit 1(setmiterlimit)prcmd currentrgbcolor 3(se"
"trgbcolor)prcmd matrix currentmatrix aload pop 6(setmatrix)prcmd applyscaleval"
"s currentdash mark 3 1 roll exch aload length 1 add -1 roll counttomark(setdas"
"h)prcmd pop}def/setgstate{/setgstate sysexec printgstate}def/save{@UD begin/@s"
"aveID vmstatus pop pop def end :save @saveID 1(save)prcmd}def/restore{:restore"
" printgstate @UD/@saveID known{@UD begin @saveID end}{0}ifelse 1(restore)prcmd"
"}def/gsave 0 defpr/grestore{:grestore printgstate 0(grestore)prcmd}def/grestor"
"eall{:grestoreall setstate 0(grestoreall)prcmd}def/rotate{dup type/arraytype n"
"e{dup 1(rotate)prcmd}if/rotate sysexec applyscalevals}def/scale{dup type/array"
"type ne{2 copy 2(scale)prcmd}if/scale sysexec applyscalevals}def/translate{dup"
" type/arraytype ne{2 copy 2(translate)prcmd}if/translate sysexec}def/setmatrix"
"{dup/setmatrix sysexec aload pop 6(setmatrix)prcmd applyscalevals}def/initmatr"
"ix{matrix setmatrix}def/concat{matrix currentmatrix matrix concatmatrix setmat"
"rix}def/makepattern{gsave<</mx 3 -1 roll>>begin dup/XUID[1000000 @patcnt]put m"
"x/makepattern sysexec dup dup begin PatternType @patcnt BBox aload pop XStep Y"
"Step PaintType mx aload pop 15(makepattern)prcmd :newpath matrix setmatrix Pai"
"ntProc 0 1(makepattern)prcmd end/@patcnt @patcnt 1 add store end grestore}def/"
"setpattern{begin PatternType 1 eq{PaintType 1 eq{XUID aload pop exch pop 1}{:g"
"save[currentcolorspace aload length -1 roll pop]setcolorspace/setcolor sysexec"
" XUID aload pop exch pop currentrgbcolor :grestore 4}ifelse(setpattern)prcmd}{"
"/setpattern sysexec}ifelse end}def/setcolor{dup type/dicttype eq{setpattern}{/"
"setcolor sysexec/currentrgbcolor sysexec setrgbcolor}ifelse}def/setgray 1 defp"
"r/setcmykcolor 4 defpr/sethsbcolor 3 defpr/setrgbcolor 3 defpr/.setopacityalph"
"a{dup/.setopacityalpha sysexec 1(setopacityalpha)prcmd}def/.setshapealpha{dup/"
".setshapealpha sysexec 1(setshapealpha)prcmd}def/.setblendmode{dup/.setblendmo"
"de sysexec<</Normal 0/Compatible 0/Multiply 1/Screen 2/Overlay 3/SoftLight 4/H"
"ardLight 5/ColorDodge 6/ColorBurn 7/Darken 8/Lighten 9/Difference 10/Exclusion"
" 11/Hue 12/Saturation 13/Color 14/Luminosity 15/CompatibleOverprint 16>>exch g"
"et 1(setblendmode)prcmd}def DELAYBIND{.bindnow}if ";
