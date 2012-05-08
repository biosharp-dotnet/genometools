/*
  Copyright (c) 2011 Giorgio Gonnella <gonnella@zbh.uni-hamburg.de>
  Copyright (c) 2011 Center for Bioinformatics, University of Hamburg

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

#ifndef RADIXSORT_STR_H
#define RADIXSORT_STR_H

/*
 * *suffixes + offset = position of first suffix to sort
 * width = number of suffixes to sort
 * depth = start sorting at given depth
 * maxdepth = stop sorting at given depth (0 means infinite)
 *
 * totallength = "real" length (not the mirror logical length)
 * equallengthplus1 = sequence length, including the separator
 *
 * */

void gt_radixsort_str_eqlen(const GtTwobitencoding *twobitencoding,
    unsigned long *suffixes, unsigned long depth,
    unsigned long maxdepth, unsigned long width, unsigned long equallengthplus1,
    unsigned long totallength);

#endif