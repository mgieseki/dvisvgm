/*************************************************************************
** psdefs.cpp                                                           **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2019 Martin Gieseking <martin.gieseking@uos.de>   **
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
"BBox null>>setpagedevice/@dodraw true store/@nulldev false store/@patcnt 0 sto"
"re/@SD systemdict def/@UD userdict def true setglobal @SD/:save @SD/save get p"
"ut @SD/:restore @SD/restore get put @SD/:gsave @SD/gsave get put @SD/:grestore"
" @SD/grestore get put @SD/:grestoreall @SD/grestoreall get put @SD/:newpath @S"
"D/newpath get put @SD/:stroke @SD/stroke get put @SD/:fill @SD/fill get put @S"
"D/:eofill @SD/eofill get put @SD/:clip @SD/clip get put @SD/:eoclip @SD/eoclip"
" get put @SD/:charpath @SD/charpath get put @SD/:show @SD/show get put @SD/:st"
"ringwidth @SD/stringwidth get put @SD/:nulldevice @SD/nulldevice get put @SD/."
"setopacityalpha known not{@SD/.setopacityalpha{pop}put}if @SD/.setshapealpha k"
"nown not{@SD/.setshapealpha{pop}put}if @SD/.setblendmode known not{@SD/.setble"
"ndmode{pop}put}if @SD/prseq{-1 1{-1 roll =only( )print}for(\\n)print}put @SD/p"
"rcmd{( )exch(\\ndvi.)3{print}repeat prseq}put @SD/cvxall{{cvx}forall}put @SD/d"
"efpr{[exch[/copy @SD]cvxall 5 -1 roll dup 6 1 roll[/get/exec]cvxall 6 -1 roll "
"dup 7 1 roll 4 -1 roll dup 5 1 roll dup length string cvs/prcmd cvx]cvx def}pu"
"t @SD/querypos{{currentpoint}stopped{$error/newerror false put}{2(querypos)prc"
"md}ifelse}put @SD/applyscalevals{1 0 dtransform exch dup mul exch dup mul add "
"sqrt 0 1 dtransform exch dup mul exch dup mul add sqrt 1 0 dtransform dup mul "
"exch dup dup mul 3 -1 roll add dup 0 eq{pop}{sqrt div}ifelse 3(applyscalevals)"
"prcmd}put @SD/prpath{{2(moveto)prcmd}{2(lineto)prcmd}{6(curveto)prcmd}{0(close"
"path)prcmd}pathforall}put @SD/nulldevice{/@nulldev true store :nulldevice 1 1("
"setnulldevice)prcmd}put @SD/charpath{/@dodraw false store :charpath/@dodraw tr"
"ue store}put @SD/stringwidth{/@dodraw false store :stringwidth/@dodraw true st"
"ore}put @SD/show{@dodraw @nulldev not and{dup :gsave currentpoint 2{50 mul exc"
"h}repeat :newpath moveto 50 50/scale sysexec true charpath fill :grestore/@dod"
"raw false store :show/@dodraw true store}{:show}ifelse}put @SD/varxyshow{exch "
"dup type/arraytype eq{<</arr 3 -1 roll/prc 5 -1 roll/chr 1 string/idx 0>>begin"
"{chr 0 3 -1 roll put :gsave chr show :grestore currentpoint prc moveto/idx idx"
" 1 add store}forall end}{pop show}ifelse}put @SD/xyshow{{exch arr idx 2 mul ge"
"t add exch arr idx 2 mul 1 add get add}varxyshow}put @SD/xshow{{exch arr idx g"
"et add exch}varxyshow}put @SD/yshow{{arr idx get add}varxyshow}put @SD/awidths"
"how{{1 string dup 0 5 index put :gsave show :grestore pop 0 rmoveto 3 index eq"
"{4 index 4 index rmoveto}if 1 index 1 index rmoveto}exch cshow 5{pop}repeat}pu"
"t @SD/widthshow{0 0 3 -1 roll pstack awidthshow}put @SD/ashow{0 0 0 6 3 roll a"
"widthshow}put @SD/newpath{:newpath 1 1(newpath)prcmd}put @SD/stroke{@dodraw @n"
"ulldev not and{prcolor 0 1(newpath)prcmd prpath 0(stroke)prcmd :newpath}{:stro"
"ke}ifelse}put @SD/fill{@dodraw @nulldev not and{prcolor 0 1(newpath)prcmd prpa"
"th 0(fill)prcmd :newpath}{:fill}ifelse}put @SD/eofill{@dodraw @nulldev not and"
"{prcolor 0 1(newpath)prcmd prpath 0(eofill)prcmd :newpath}{:eofill}ifelse}put "
"@SD/clip{:clip @nulldev not{0 1(newpath)prcmd prpath 0(clip)prcmd}if}put @SD/e"
"oclip{:eoclip @nulldev not{0 1(newpath)prcmd prpath 0(eoclip)prcmd}}put @SD/sh"
"fill{begin currentdict/ShadingType known currentdict/ColorSpace known and curr"
"entdict/DataSource known and currentdict/Function known not and ShadingType 4 "
"ge and DataSource type/arraytype eq and{<</DeviceGray 1/DeviceRGB 3/DeviceCMYK"
" 4/bgknown currentdict/Background known/bbknown currentdict/BBox known>>begin "
"currentdict ColorSpace known{ShadingType ColorSpace load bgknown{1 Background "
"aload pop}{0}ifelse bbknown{1 BBox aload pop}{0}ifelse ShadingType 5 eq{Vertic"
"esPerRow}if DataSource aload length 4 add bgknown{ColorSpace load add}if bbkno"
"wn{4 add}if ShadingType 5 eq{1 add}if(shfill)prcmd}if end}if end}put/@rect{4 -"
"2 roll moveto exch dup 0 rlineto exch 0 exch rlineto neg 0 rlineto closepath}b"
"ind def/@rectcc{4 -2 roll moveto 2 copy 0 lt exch 0 lt xor{dup 0 exch rlineto "
"exch 0 rlineto neg 0 exch rlineto}{exch dup 0 rlineto exch 0 exch rlineto neg "
"0 rlineto}ifelse closepath}bind def @SD/rectclip{:newpath dup type/arraytype e"
"q{aload length 4 idiv{@rectcc}repeat}{@rectcc}ifelse clip :newpath}put @SD/rec"
"tfill{gsave :newpath dup type/arraytype eq{aload length 4 idiv{@rectcc}repeat}"
"{@rectcc}ifelse fill grestore}put @SD/rectstroke{gsave :newpath dup type/array"
"type eq{aload length 4 idiv{@rect}repeat}{@rect}ifelse stroke grestore}put fal"
"se setglobal @SD readonly pop/initclip 0 defpr/clippath 0 defpr/sysexec{@SD ex"
"ch get exec}def/adddot{dup length 1 add string dup 0 46 put dup 3 -1 roll 1 ex"
"ch putinterval}def/setlinewidth{dup/setlinewidth sysexec 1(setlinewidth)prcmd}"
"def/setlinecap 1 defpr/setlinejoin 1 defpr/setmiterlimit 1 defpr/setdash{mark "
"3 1 roll 2 copy/setdash sysexec exch aload length 1 add -1 roll counttomark(se"
"tdash)prcmd pop}def/@setpagedevice{pop<<>>/setpagedevice sysexec matrix setmat"
"rix newpath 0(setpagedevice)prcmd}def/@checknulldev{@nulldev{currentpagedevice"
" maxlength 0 ne{/@nulldev false store 0 1(setnulldevice)prcmd}if}if}def/prcolo"
"r{currentrgbcolor 3(setrgbcolor)prcmd}def/printgstate{@dodraw @nulldev not and"
"{matrix currentmatrix aload pop 6(setmatrix)prcmd applyscalevals currentlinewi"
"dth 1(setlinewidth)prcmd currentlinecap 1(setlinecap)prcmd currentlinejoin 1(s"
"etlinejoin)prcmd currentmiterlimit 1(setmiterlimit)prcmd currentrgbcolor 3(set"
"rgbcolor)prcmd currentdash mark 3 1 roll exch aload length 1 add -1 roll count"
"tomark(setdash)prcmd pop}if}def/setgstate{/setgstate sysexec printgstate}def/s"
"ave{@UD begin/@saveID vmstatus pop pop def end :save @saveID 1(save)prcmd}def/"
"restore{:restore @checknulldev printgstate @UD/@saveID known{@UD begin @saveID"
" end}{0}ifelse 1(restore)prcmd}def/gsave 0 defpr/grestore{:grestore @checknull"
"dev printgstate 0(grestore)prcmd}def/grestoreall{:grestoreall @checknulldev se"
"tstate 0(grestoreall)prcmd}def/rotate{dup type/arraytype ne @dodraw and{dup 1("
"rotate)prcmd}if/rotate sysexec applyscalevals}def/scale{dup type/arraytype ne "
"@dodraw and{2 copy 2(scale)prcmd}if/scale sysexec applyscalevals}def/translate"
"{dup type/arraytype ne @dodraw and{2 copy 2(translate)prcmd}if/translate sysex"
"ec}def/setmatrix{dup/setmatrix sysexec @dodraw{aload pop 6(setmatrix)prcmd app"
"lyscalevals}{pop}ifelse}def/initmatrix{matrix setmatrix}def/concat{matrix curr"
"entmatrix matrix concatmatrix setmatrix}def/makepattern{gsave<</mx 3 -1 roll>>"
"begin dup/XUID[1000000 @patcnt]put mx/makepattern sysexec dup dup begin Patter"
"nType @patcnt BBox aload pop XStep YStep PaintType mx aload pop 15(makepattern"
")prcmd :newpath matrix setmatrix PaintProc 0 1(makepattern)prcmd end/@patcnt @"
"patcnt 1 add store end grestore}def/setpattern{begin PatternType 1 eq{PaintTyp"
"e 1 eq{XUID aload pop exch pop 1}{:gsave[currentcolorspace aload length -1 rol"
"l pop]setcolorspace/setcolor sysexec XUID aload pop exch pop currentrgbcolor :"
"grestore 4}ifelse(setpattern)prcmd}{/setpattern sysexec}ifelse end}def/setcolo"
"r{dup type/dicttype eq{setpattern}{/setcolor sysexec/currentrgbcolor sysexec s"
"etrgbcolor}ifelse}def/setgray 1 defpr/setcmykcolor 4 defpr/sethsbcolor 3 defpr"
"/setrgbcolor 3 defpr/.setopacityalpha{dup/.setopacityalpha sysexec 1(setopacit"
"yalpha)prcmd}def/.setshapealpha{dup/.setshapealpha sysexec 1(setshapealpha)prc"
"md}def/.setblendmode{dup/.setblendmode sysexec<</Normal 0/Compatible 0/Multipl"
"y 1/Screen 2/Overlay 3/SoftLight 4/HardLight 5/ColorDodge 6/ColorBurn 7/Darken"
" 8/Lighten 9/Difference 10/Exclusion 11/Hue 12/Saturation 13/Color 14/Luminosi"
"ty 15/CompatibleOverprint 16>>exch get 1(setblendmode)prcmd}def/@pdfpagecount{"
"(r)file runpdfbegin pdfpagecount runpdfend}def/@pdfpagebox{(r)file runpdfbegin"
" dup dup 1 lt exch pdfpagecount gt or{pop}{pdfgetpage/MediaBox pget pop aload "
"pop}ifelse runpdfend}def DELAYBIND{.bindnow}if ";

