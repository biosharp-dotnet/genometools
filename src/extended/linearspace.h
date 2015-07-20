/*
  Copyright (c) 2015 Annika <annika.seidel@studium.uni-hamburg.de>
  Copyright (c) 2015 Center for Bioinformatics, University of Hamburg

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

#ifndef LINEARSPACE_H
#define LINEARSPACE_H

#include "core/unused_api.h"
#include "core/error.h"
#include "extended/alignment.h"

GtAlignment *gt_computelinearspace2( const GtUchar *useq,
                                     const GtUword ustart,
                                     const GtUword ulen,
                                     const GtUchar *vseq,
                                     const GtUword vstart,
                                     const GtUword vlen,
                                     const GtWord matchcost,
                                     const GtWord mismatchcost,
                                     const GtWord gapcost);

GtUword gt_calc_linearalign2(const GtUchar *useq,
                             const GtUword ustart, GtUword ulen,
                             const GtUchar *vseq,
                             const GtUword vstart, GtUword vlen,
                             GtAlignment *align,
                             const GtWord matchcost,
                             const GtWord mismatchcost,
                             const GtWord gapcost);

/* extreme Codecuplizierung, entspricht linearedist mit varibalen Kosten*/
#endif
