/*
  Copyright (c) 2009 Stefan Kurtz <kurtz@zbh.uni-hamburg.de>
  Copyright (c) 2009 Center for Bioinformatics, University of Hamburg

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

#include "encseq-def.h"
#include "idxlocalidp.h"

typedef struct
{
  unsigned long umax,
                vmax;
} Maxscorecoord;

Scoretype localsimilarityscore(Scoretype *scol,
                               Maxscorecoord *maxpair,
                               const Scorevalues *scorevalues,
                               const Uchar *useq,
                               unsigned long ulen,
                               const Encodedsequence *vencseq,
                               Seqpos startpos,
                               Seqpos endpos)
{
  Scoretype val, we, nw, *scolptr, maximalscore = 0;
  const Uchar *uptr;
  Uchar vcurrent;
  Seqpos j;

  maxpair->umax = maxpair->vmax = 0;
  for (scolptr = scol; scolptr <= scol + ulen; scolptr++)
  {
    *scolptr = 0;
  }
  for (j = startpos; j < endpos; j++)
  {
    nw = 0;
    vcurrent = getencodedchar(vencseq,j,Forwardmode);
    gt_assert(vcurrent != SEPARATOR);
    for (scolptr = scol+1, uptr = useq; uptr < useq + ulen; scolptr++, uptr++)
    {
      gt_assert(*uptr != SEPARATOR);
      we = *scolptr;
      *scolptr = *(scolptr-1) + scorevalues->gapextend;
      if ((val = nw + REPLACEMENTSCORE(scorevalues,*uptr,vcurrent)) > *scolptr)
      {
        *scolptr = val;
      }
      if ((val = we + scorevalues->gapextend) > *scolptr)
      {
        *scolptr = val;
      }
      if (*scolptr < 0)
      {
        *scolptr = 0;
      } else
      {
        if (*scolptr > maximalscore)
        {
          maximalscore = *scolptr;
          maxpair->umax = (unsigned long) (uptr - useq + 1);
          maxpair->vmax = (unsigned long) (j - startpos + 1);
        }
      }
      nw = we;
    }
  }
  return maximalscore;
}
