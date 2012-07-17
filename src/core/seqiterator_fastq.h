/*
  Copyright (c) 2009-2010 Sascha Steinbiss <steinbiss@zbh.uni-hamburg.de>
  Copyright (c) 2009-2010 Center for Bioinformatics, University of Hamburg

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

#ifndef SEQITERATOR_FASTQ_H
#define SEQITERATOR_FASTQ_H

#include "core/error.h"
#include "core/seqiterator.h"
#include "core/str_array.h"

typedef struct GtSeqIteratorFastQ GtSeqIteratorFastQ;

/* Create a new <GtSeqIteratorQual> for all sequence files in <filenametab>. */
GtSeqIterator* gt_seqiterator_fastq_new(const GtStrArray *filenametab,
                                             GtError *err);
/* Create a new <GtSeqIteratorFastQ> for all sequence files in <filenametab>
 *containing color space reads. */
GtSeqIterator* gt_seqiterator_colorspace_fastq_new(
                                                const GtStrArray *filenametab,
                                                GtError *err);

unsigned long  gt_seqiterator_fastq_get_file_index(GtSeqIteratorFastQ*);

/* disable checking if quality description is equal to read description;
 * (it should be, but it is not in output of some tools, e.g. Coral) */
void gt_seqiterator_fastq_relax_check_of_quality_description(
    GtSeqIteratorFastQ *seqitf);

const GtSeqIteratorClass* gt_seqiterator_fastq_class(void);

#endif
