/* Copyright (C) 2001-2005 Peter Selinger.
   This file is part of potrace. It is free software and it is covered
   by the GNU General Public License. See the file COPYING for details. */

/* $Id: decompose.h,v 1.6 2005/02/22 21:31:23 selinger Exp $ */

#ifndef DECOMPOSE_H
#define DECOMPOSE_H

#include "potracelib.h"
#include "progress.h"

int bm_to_pathlist(const potrace_bitmap_t *bm, path_t **plistp, const potrace_param_t *param, progress_t *progress);

#endif /* DECOMPOSE_H */

