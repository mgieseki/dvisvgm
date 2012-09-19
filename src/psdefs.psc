/*************************************************************************
** psdefs.psc                                                           **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2012 Martin Gieseking <martin.gieseking@uos.de>   **
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
"3 dict dup begin/Install{matrix setmatrix}def/HWResolution[72 72]def/PageSize[10"
"000 10000]def end setpagedevice/@dodraw true store/@SD systemdict def/@UD userdi"
"ct def true setglobal @SD/:save @SD/save get put @SD/:restore @SD/restore get pu"
"t @SD/:gsave @SD/gsave get put @SD/:grestore @SD/grestore get put @SD/:grestorea"
"ll @SD/grestoreall get put @SD/:stroke @SD/stroke get put @SD/:fill @SD/fill get"
" put @SD/:eofill @SD/eofill get put @SD/:clip @SD/clip get put @SD/:eoclip @SD/e"
"oclip get put @SD/:charpath @SD/charpath get put @SD/.setopacityalpha known not{"
"@SD/.setopacityalpha{pop}put}if @SD/prval{dup type/stringtype eq{print}{30 strin"
"g cvs print}ifelse}put @SD/prseq{-1 1{-1 roll prval( )print}for(\\n)print}put @S"
"D/prcmd{( )exch(\\ndvi.)3{print}repeat prseq}put @SD/cvxall{{cvx}forall}put @SD/"
"defpr{[exch[/copy @SD]cvxall 5 -1 roll dup 6 1 roll[/get/exec]cvxall 6 -1 roll d"
"up 7 1 roll 4 -1 roll dup 5 1 roll dup length string cvs/prcmd cvx]cvx def}put @"
"SD/querypos{{currentpoint}stopped{$error/newerror false put}{2(querypos)prcmd}if"
"else}put @SD/applyscalevals{1 0 transform 0 0 transform 3 -1 roll sub dup mul 3 "
"1 roll sub dup mul add sqrt 0 1 transform 0 0 transform 3 -1 roll sub dup mul 3 "
"1 roll sub dup mul add sqrt 1 0 transform dup mul exch dup dup mul 3 -1 roll add"
" sqrt div 3(applyscalevals)prcmd}def @SD/prpath{{2(moveto)prcmd}{2(lineto)prcmd}"
"{6(curveto)prcmd}{0(closepath)prcmd}pathforall}put @SD/charpath{/@dodraw false s"
"tore :charpath/@dodraw true store}put @SD/show{@dodraw{true charpath eofill}if}p"
"ut @SD/stroke{@dodraw{0(newpath)prcmd prpath 0(stroke)prcmd newpath}{:stroke}ife"
"lse}put @SD/fill{@dodraw{0(newpath)prcmd prpath 0(fill)prcmd newpath}{:fill}ifel"
"se}put @SD/eofill{@dodraw{0(newpath)prcmd prpath 0(eofill)prcmd newpath}{:eofill"
"}ifelse}put @SD/clip{:clip 0(newpath)prcmd prpath 0(clip)prcmd}put @SD/eoclip{:e"
"oclip 0(newpath)prcmd prpath 0(eoclip)prcmd}put @SD/bop{pop pop}put/@rect{4 -2 r"
"oll moveto exch dup 0 rlineto exch 0 exch rlineto neg 0 rlineto closepath}bind d"
"ef/@rectcc{4 -2 roll moveto 2 copy 0 lt exch 0 lt xor{dup 0 exch rlineto exch 0 "
"rlineto neg 0 exch rlineto}{exch dup 0 rlineto exch 0 exch rlineto neg 0 rlineto"
"}ifelse closepath}bind def @SD/rectclip{newpath dup type/arraytype eq{aload leng"
"th 4 idiv{@rectcc}repeat}{@rectcc}ifelse clip newpath}put @SD/rectfill{gsave new"
"path dup type/arraytype eq{aload length 4 idiv{@rectcc}repeat}{@rectcc}ifelse fi"
"ll grestore}put @SD/rectstroke{gsave newpath dup type/arraytype eq{aload length "
"4 idiv{@rect}repeat}{@rect}ifelse stroke grestore}put false setglobal @SD readon"
"ly pop/initclip 0 defpr/sysexec{@SD exch get exec}def/adddot{dup length 1 add st"
"ring dup 0 46 put dup 3 -1 roll 1 exch putinterval}def/setlinewidth{dup/setlinew"
"idth sysexec applyscalevals 1(setlinewidth)prcmd}def/setlinecap 1 defpr/setlinej"
"oin 1 defpr/setmiterlimit 1 defpr/setdash{mark 3 1 roll 2 copy/setdash sysexec a"
"pplyscalevals exch aload length 1 add -1 roll counttomark(setdash)prcmd pop}def/"
"setgstate{currentlinewidth 1(setlinewidth)prcmd currentlinecap 1(setlinecap)prcm"
"d currentlinejoin 1(setlinejoin)prcmd currentmiterlimit 1(setmiterlimit)prcmd cu"
"rrentrgbcolor 3(setrgbcolor)prcmd 6 array currentmatrix aload pop 6(setmatrix)pr"
"cmd currentdash mark 3 1 roll exch aload length 1 add -1 roll counttomark(setdas"
"h)prcmd pop}def/save{@UD begin/@saveID vmstatus pop pop def end :save @saveID 1("
"save)prcmd}def/restore{:restore setgstate @UD/@saveID known{@UD begin @saveID en"
"d}{0}ifelse 1(restore)prcmd}def/gsave 0 defpr/grestore{:grestore setgstate 0(gre"
"store)prcmd}def/grestoreall{:grestoreall setstate 0(grestoreall)prcmd}def/rotate"
"{dup type/arraytype ne{dup 1(rotate)prcmd}if/rotate sysexec}def/scale{dup type/a"
"rraytype ne{2 copy 2(scale)prcmd}if/scale sysexec}def/translate{dup type/arrayty"
"pe ne{2 copy 2(translate)prcmd}if/translate sysexec}def/setmatrix{dup/setmatrix "
"sysexec aload pop 6(setmatrix)prcmd}def/initmatrix{matrix setmatrix}def/concat{m"
"atrix currentmatrix matrix concatmatrix setmatrix}def/setgray 1 defpr/setcmykcol"
"or 4 defpr/sethsbcolor 3 defpr/setrgbcolor 3 defpr/.setopacityalpha{dup/.setopac"
"ityalpha sysexec 1(setopacityalpha)prcmd}def/.handleerror errordict/handleerror "
"get def errordict begin/handleerror{0(beginerror)prcmd .handleerror 0(enderror)p"
"rcmd}.bind def end ";

// vim: set syntax=cpp:
