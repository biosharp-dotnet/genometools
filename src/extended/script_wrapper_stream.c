/*
  Copyright (c) 2009 Sascha Steinbiss <steinbiss@zbh.uni-hamburg.de>
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

#include "core/unused_api.h"
#include "extended/node_stream_api.h"
#include "extended/script_wrapper_stream.h"

struct GtScriptWrapperStream {
  const GtNodeStream parent_instance;
  GtScriptWrapperStreamNextFunc next_func;
  GtScriptWrapperStreamFreeFunc free_func;
};

#define script_wrapper_stream_cast(GS)\
        gt_node_stream_cast(gt_script_wrapper_stream_class(), GS)

static int script_wrapper_stream_next(GtNodeStream *gs,
                                      GtGenomeNode **gn,
                                      GtError *err)
{
  GtScriptWrapperStream *script_wrapper_stream;
  int had_err = 0;
  gt_error_check(err);
  script_wrapper_stream = script_wrapper_stream_cast(gs);
  had_err = script_wrapper_stream->next_func(gn, err);
  return had_err;
}

static void script_wrapper_stream_free(GtNodeStream *gs)
{
  GtScriptWrapperStream *script_wrapper_stream;
  if (!gs) return;
  script_wrapper_stream = script_wrapper_stream_cast(gs);
  script_wrapper_stream->free_func(NULL);
}

const GtNodeStreamClass* gt_script_wrapper_stream_class(void)
{
  static const GtNodeStreamClass *nsc = NULL;
  if (!nsc) {
    nsc = gt_node_stream_class_new(sizeof (GtScriptWrapperStream),
                                   script_wrapper_stream_free,
                                   script_wrapper_stream_next);
  }
  return nsc;
}

GtNodeStream* gt_script_wrapper_stream_new(GtScriptWrapperStreamNextFunc next,
                                           GtScriptWrapperStreamFreeFunc free)
{
  GtNodeStream *gs;
  GtScriptWrapperStream *script_wrapper_stream;
  gt_assert(next && free);
  gs = gt_node_stream_create(gt_script_wrapper_stream_class(), true);
  script_wrapper_stream = script_wrapper_stream_cast(gs);
  script_wrapper_stream->next_func = next;
  script_wrapper_stream->free_func = free;
  return gs;
}
