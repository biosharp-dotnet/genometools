/*
  Copyright (c) 2008 Sascha Steinbiss <steinbiss@zbh.uni-hamburg.de>
  Copyright (c) 2008 Center for Bioinformatics, University of Hamburg

  Permission to use, copy, modify, and distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef LINE_BREAKER_REP_H
#define LINE_BREAKER_REP_H

#include <stdio.h>
#include "annotationsketch/line_breaker.h"

typedef bool (*LineBreakerIsOccupiedFunc)(GtLineBreaker*, GtLine*, GtBlock*);
typedef void (*LineBreakerRegisterBlockFunc)(GtLineBreaker*, GtLine*, GtBlock*);
typedef void (*LineBreakerFreeFunc)(GtLineBreaker*);

typedef struct GtLineBreakerMembers GtLineBreakerMembers;

struct GtLineBreaker {
  const GtLineBreakerClass *c_class;
  GtLineBreakerMembers *pvt;
};

const GtLineBreakerClass* gt_line_breaker_class_new(size_t size,
                                    LineBreakerIsOccupiedFunc is_occupied,
                                    LineBreakerRegisterBlockFunc register_block,
                                    LineBreakerFreeFunc free);
GtLineBreaker* gt_line_breaker_create(const GtLineBreakerClass*);
void*          gt_line_breaker_cast(const GtLineBreakerClass*,
                                    GtLineBreaker*);

#endif
