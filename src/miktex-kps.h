/* web2c-miktex.h: Web2C/KPSE emutaltion              -*- C++ -*-

   Copyright (C) 2000-2004 Christian Schenk

   This file is part of the MiKTeX KPS Library.

   The MiKTeX KPS Library is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2, or
   (at your option) any later version.

   The MiKTeX KPS Library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the MiKTeX KPS Library; if not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA. */

#pragma once

#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <windows.h>

//#include <miktex.h>

#if ! defined(MIKTEXKPS__2436ECF4_E9B5_4A24_A0E0_1BCD8200F9CB__)
#  if defined(_MSC_VER)
#    define MIKTEXKPSEXPORT __declspec(dllimport)
#  else
#    define MIKTEXKPSEXPORT __declspec(dllimport)
#  endif
#endif

#if defined(_MSC_VER)
#  define MIKTEXKPSCALL __stdcall
#  define MIKTEXKPSCALLBACK __stdcall
#  define MIKTEXKPSDEPRECATED __declspec(deprecated)
#else
#  define MIKTEXKPSCALL __stdcall
#  define MIKTEXKPSCALLBACK
#  define MIKTEXKPSDEPRECATED
#endif

#define MIKTEXKPSAPI(type) MIKTEXKPSEXPORT type MIKTEXKPSCALL
#define MIKTEXKPSDATA(type) MIKTEXKPSEXPORT type

#define MIKTEX_BEGIN_EXTERN_C_BLOCK extern "C" {
#define MIKTEX_END_EXTERN_C_BLOCK }

#ifdef __cplusplus
MIKTEX_BEGIN_EXTERN_C_BLOCK
#endif

/* _________________________________________________________________________

   Basic Types
   _________________________________________________________________________ */

typedef LPTSTR string;
typedef LPCTSTR const_string;

#ifndef NO_BOOLEAN
#  ifdef __cplusplus
#    define boolean bool
#  else
#    if ! defined(false) && ! defined(true)
typedef enum { false = 0, true = 1 } MIKTEX_TEXK_BOOLEAN;
#    endif
#    define boolean unsigned char
#  endif
#endif

typedef int integer;

/* _________________________________________________________________________

   Kpathsea Types
   _________________________________________________________________________ */

typedef enum
{
  kpse_gf_format,
  kpse_pk_format,
  kpse_any_glyph_format,
  kpse_tfm_format, 
  kpse_afm_format, 
  kpse_base_format, 
  kpse_bib_format, 
  kpse_bst_format, 
  kpse_cnf_format,
  kpse_db_format,
  kpse_fmt_format,
  kpse_fontmap_format,
  kpse_mem_format,
  kpse_mf_format, 
  kpse_mfpool_format, 
  kpse_mft_format, 
  kpse_mp_format, 
  kpse_mppool_format, 
  kpse_mpsupport_format,
  kpse_ocp_format,
  kpse_ofm_format, 
  kpse_opl_format,
  kpse_otp_format,
  kpse_ovf_format,
  kpse_ovp_format,
  kpse_pict_format,
  kpse_tex_format,
  kpse_texdoc_format,
  kpse_texpool_format,
  kpse_texsource_format,
  kpse_tex_ps_header_format,
  kpse_troff_font_format,
  kpse_type1_format, 
  kpse_vf_format,
  kpse_dvips_config_format,
  kpse_ist_format,
  kpse_truetype_format,
  kpse_type42_format,
  kpse_web2c_format,
  kpse_program_text_format,
  kpse_program_binary_format,
  kpse_miscfonts_format,
  kpse_web_format,
  kpse_cweb_format,
  kpse_enc_format,
  kpse_cmap_format,
  kpse_sfd_format,
  kpse_opentype_format,
  kpse_pdftex_config_format,
  kpse_lig_format,
  kpse_texmfscripts_format,
  kpse_last_format
} kpse_file_format_type;

typedef enum
{
  kpse_glyph_source_normal,
  kpse_glyph_source_alias,
  kpse_glyph_source_maketex,
  kpse_glyph_source_fallback
} kpse_glyph_source_type;

typedef struct
{
  LPTSTR name;
  unsigned dpi;
  kpse_file_format_type format;
  kpse_glyph_source_type source;
} kpse_glyph_file_type;		

/* _________________________________________________________________________

   Constants
   _________________________________________________________________________ */

#define FOPEN_R_MODE _T("r")
#define FOPEN_W_MODE _T("w")
#define FOPEN_RBIN_MODE _T("rb")
#define FOPEN_WBIN_MODE _T("wb")

#define DIR_SEP_STRING _T("\\")

/* _________________________________________________________________________

   P?H and P?C Macros
   _________________________________________________________________________ */

#define AA(args) args /* For an arbitrary number; ARGS must be in parens.  */

#define P1H(x1) (x1)
#define P2H(p1,p2) (p1, p2)
#define P3H(p1,p2,p3) (p1, p2, p3)
#define P4H(p1,p2,p3,p4) (p1, p2, p3, p4)
#define P5H(p1,p2,p3,p4,p5) (p1, p2, p3, p4, p5)
#define P6H(p1,p2,p3,p4,p5,p6) (p1, p2, p3, p4, p5, p6)
#define P1C(t1, v1) (t1 v1)
#define P2C(t1, v1, t2, v2) (t1 v1, t2 v2)
#define P3C(t1, v1, t2, v2, t3, v3) (t1 v1, t2 v2, t3 v3)
#define P4C(t1, v1, t2, v2, t3, v3, t4, v4) (t1 v1, t2 v2, t3 v3, t4 v4)

#define PVAR1H(p1) (p1, ...)
#define PVAR1C(t1, n1,  ap) \
  (t1 n1, ...) { va_list ap; va_start (ap, n1);

/* _________________________________________________________________________

   Emulations
   _________________________________________________________________________ */

#define KPSEVERSION _T("kpathsea version 3.3.2")

#define FILESTRCASEEQ(s1, s2) (miktex_pathcmp(s1, s2) == 0)

#define IS_DIR_SEP(c) ((c) == _T('\\') || (c) == _T('/'))

#define KPSEDLL __declspec(dllimport)

#define KPSE_BITMAP_TOLERANCE(r) ((r) / 500.0 + 1)

#define SET_BINARY(f) _setmode((f), _O_BINARY)

#define STRCASECMP(s1, s2) strcasecmp(s1, s2)

#ifndef MIKTEX_NO_XFUNC
#  define XRETALLOC(addr, n, t) \
	((addr) = (t *) xrealloc(addr, (n) * sizeof(t)))
#  define XTALLOC(n, t) ((t *) xmalloc((n) * sizeof(t)))
#  define XTALLOC1(t) XTALLOC (1, t)
#endif /* ! MIKTEX_NO_XFUNC */

#define concat(s1, s2) \
  _tcscat(_tcscpy((LPTSTR)xmalloc(_tcslen(s1) + _tcslen(s2) + 1), (s1)), (s2))

#ifndef basenamechangesuffix
#  define basenamechangesuffix miktex_web2c_basenamechangesuffix
#endif

#ifndef extend_filename
#  define extend_filename miktex_web2c_extend_filename
#endif

#define kpse_set_program_enabled(fmt, value, level)

#define kpathsea_debug miktex_kpathsea_debug

#define kpathsea_version_string miktex_kpathsea_version_string

#define kpse_bitmap_tolerance miktex_kpse_bitmap_tolerance

#define kpse_find_file(name, format, must_exist) \
  miktex_kpse_find_file(name, format, must_exist)

#define kpse_tex_hush(what) 0

#define kpse_find_glyph(font_name, dpi, format, glyph_file) \
  miktex_kpse_find_glyph(font_name, dpi, format, glyph_file)

#define kpse_find_ofm(name) miktex_kpse_find_file(name, kpse_ofm_format, 1)

#define kpse_find_pict(name) miktex_kpse_find_file(name, kpse_pict_format, 1)

#define kpse_find_pk(font_name, dpi, glyph_file) \
  miktex_kpse_find_glyph (font_name, dpi, kpse_pk_format, glyph_file)

#define kpse_find_vf(name) miktex_kpse_find_file (name, kpse_vf_format, 0)

#define kpse_find_tfm(name) miktex_kpse_find_file(name, kpse_tfm_format, 1)

#define kpse_init_prog miktex_kpse_init_prog

#define kpse_magstep_fix miktex_kpse_magstep_fix

#define kpse_open_file(name, type) miktex_kpse_open_file(name, type)

#define kpse_program_name (miktex_kpse_program_invocation_name())

#define kpse_set_progname(argv0)

#define kpse_set_program_name(argv0, progname)

#define kpse_var_value(var) miktex_kpse_var_value(var)

#define open_input(f_ptr, filefmt, fopen_mode) \
  miktex_web2c_open_input(nameoffile, f_ptr, filefmt, fopen_mode)

/* <todo> */
#define printversionandexit(banner, copyright_holder, author) \
  _tprintf(_T("This is ") #banner _T("\n"))
/* </todo> */

#define program_invocation_name (miktex_kpse_program_invocation_name())

#ifndef rindex
#  define rindex strrchr
#endif

#define uexit(code) c4pexit(code)

#ifndef uppercasify
#  define uppercasify miktex_web2c_uppercasify
#endif

#define versionstring miktex_web2c_version_string

#ifndef zround
#  define zround miktex_web2c_round
#endif

#define MAKE_TEX_PK_BY_DEFAULT 1

/* _________________________________________________________________________

   Prototypes
   _________________________________________________________________________ */

MIKTEXKPSAPI(int)
miktex_kpse_bitmap_tolerance (/*[in]*/ double	dpi1,
			      /*[in]*/ double	dpi2);

MIKTEXKPSAPI(LPTSTR)
miktex_kpse_find_file (/*[in]*/ LPCTSTR			lpszFileName,
		       /*[in]*/ kpse_file_format_type	format,
		       /*[in]*/ int			bMustExist);

MIKTEXKPSAPI(LPTSTR)
miktex_kpse_find_glyph (/*[in]*/ LPTSTR				lpszFontName,
			/*[in]*/ unsigned			uDpi,
			/*[in]*/ kpse_file_format_type		format,
			/*[out]*/ kpse_glyph_file_type *	glyph_file);

MIKTEXKPSAPI(void)
miktex_kpse_init_prog (/*[in]*/ LPCTSTR		prefix,
		       /*[in]*/ unsigned	dpi,
		       /*[in]*/ LPCTSTR		mode,
		       /*[in]*/ LPCTSTR		fallback);

MIKTEXKPSAPI(unsigned)
miktex_kpse_magstep_fix (/*[in]*/ unsigned	dpi,
			 /*[in]*/ unsigned	bdpi,
			 /*[in]*/ int *		m_ret);

MIKTEXKPSAPI(FILE *)
miktex_kpse_open_file (/*[in]*/ LPCTSTR			lpszFileName,
		       /*[in]*/ kpse_file_format_type	format);

MIKTEXKPSAPI(LPCTSTR)
miktex_kpse_program_invocation_name ();

//MIKTEXKPSAPI(void)
//miktex_kpse_usage_help (/*[in]*/ const _TCHAR ** lpMessage);

MIKTEXKPSAPI(LPTSTR)
miktex_kpse_var_value (/*[in]*/ LPCTSTR lpszVarName);

MIKTEXKPSAPI(LPTSTR)
miktex_web2c_basenamechangesuffix (/*[in]*/ LPCTSTR	name,
				   /*[in]*/ LPCTSTR	old_suffix,
				   /*[in]*/ LPCTSTR	new_suffix);

MIKTEXKPSAPI(LPTSTR)
miktex_web2c_extend_filename (/*[in]*/ LPCTSTR	name,
			      /*[in]*/ LPCTSTR	default_suffix);

MIKTEXKPSAPI(int)
miktex_web2c_open_input (/*[in]*/ LPCTSTR		lpszFileName,
			 /*[in]*/ FILE **		ppfile,
			 /*[in]*/ kpse_file_format_type	format,
			 /*[in]*/ LPCTSTR		lpszMode);

MIKTEXKPSAPI(int)
miktex_web2c_round (/*[in]*/ double);

MIKTEXKPSAPI(LPTSTR)
miktex_web2c_uppercasify (/*[in]*/ LPCTSTR lpszSource);

#ifndef MIKTEX_NO_XFUNC
MIKTEXKPSAPI(int)
strcasecmp (/*[in]*/ LPCTSTR s1,
	    /*[in]*/ LPCTSTR s2);
#endif /* ! MIKTEX_NO_XFUNC */

#ifndef MIKTEX_NO_XFUNC
MIKTEXKPSAPI(LPCTSTR)
xbasename (/*[in]*/ LPCTSTR s);

MIKTEXKPSAPI(void)
xfclose (/*[in]*/ FILE *	stream,
	 /*[in]*/ LPCTSTR	filename);

MIKTEXKPSAPI(FILE *)
xfopen (/*[in]*/ LPCTSTR	name,
	/*[in]*/ LPCTSTR	mode);

MIKTEXKPSAPI(int)
xfseek (/*[in]*/ FILE *		stream,
	/*[in]*/ long		offset,
	/*[in]*/ int		where,
	/*[in]*/ LPCTSTR	filename);

MIKTEXKPSAPI(long)
xftell (/*[in]*/ FILE *		stream,
	/*[in]*/ LPCTSTR	filename);

MIKTEXKPSAPI(LPVOID)
xmalloc (/*[in]*/ unsigned long s);

MIKTEXKPSAPI(void)
xputenv (/*[in]*/ LPCTSTR lpszVarName,
	 /*[in]*/ LPCTSTR lpszValue);

MIKTEXKPSAPI(LPVOID)
xrealloc (/*[in]*/ LPVOID		p,
	  /*[in]*/ unsigned long	s);

MIKTEXKPSAPI(LPTSTR)
xstrdup (/*[in]*/ LPCTSTR s);
#endif /* ! MIKTEX_NO_XFUNC */

/* _________________________________________________________________________

   Variables
   _________________________________________________________________________ */


extern MIKTEXKPSDATA(LPTSTR)
miktex_kpathsea_version_string;

extern MIKTEXKPSDATA(LPTSTR)
miktex_web2c_version_string;

extern MIKTEXKPSDATA(unsigned)
miktex_kpathsea_debug;

/* _________________________________________________________________________ */

#ifdef __cplusplus
MIKTEX_END_EXTERN_C_BLOCK
#endif

#undef MIKTEX_BEGIN_EXTERN_C_BLOCK
#undef MIKTEX_END_EXTERN_C_BLOCK
