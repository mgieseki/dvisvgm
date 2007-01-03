/* Copyright (C) 2001-2005 Peter Selinger.
   This file is part of potrace. It is free software and it is covered
   by the GNU General Public License. See the file COPYING for details. */

/* $Id: flate.h,v 1.7 2005/02/22 21:31:23 selinger Exp $ */

#ifndef FLATE_H
#define FLATE_H

int dummy_xship(FILE *f, int filter, char *s, int len);
int flate_xship(FILE *f, int filter, char *s, int len);
int pdf_xship(FILE *f, int filter, char *s, int len);
int lzw_xship(FILE *f, int filter, char *s, int len);
int a85_xship(FILE *f, int filter, char *s, int len);

#endif /* FLATE_H */
