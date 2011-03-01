/*
  Copyright (c) 2011 Sascha Steinbiss <steinbiss@zbh.uni-hamburg.de>
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

#include "core/cstr_array.h"
#include "core/option.h"
#include "core/versionfunc.h"
#include "extended/toolbox.h"
#include "tools/gt_bitextract.h"
#include "tools/gt_einfo.h"
#include "tools/gt_seqdecode.h"
#include "tools/gt_seqencode.h"

static void* gt_encseq_arguments_new(void)
{
  GtToolbox *encseq_toolbox = gt_toolbox_new();
  gt_toolbox_add_tool(encseq_toolbox, "bitextract", gt_bitextract());
  gt_toolbox_add_tool(encseq_toolbox, "info", gt_einfo());
  gt_toolbox_add_tool(encseq_toolbox, "decode", gt_seqdecode());
  gt_toolbox_add_tool(encseq_toolbox, "encode", gt_seqencode());
  return encseq_toolbox;
}

static void gt_encseq_arguments_delete(void *tool_arguments)
{
  GtToolbox *encseq_toolbox = tool_arguments;
  if (!encseq_toolbox) return;
  gt_toolbox_delete(encseq_toolbox);
}

static GtOptionParser* gt_encseq_option_parser_new(void *tool_arguments)
{
  GtToolbox *encseq_toolbox = tool_arguments;
  GtOptionParser *op;
  gt_assert(encseq_toolbox);
  op = gt_option_parser_new("[option ...] tool [argument ...]",
                            "Call encseq manipulation tool <tool> and "
                            "pass argument(s) to it.");
  gt_option_parser_set_comment_func(op, gt_toolbox_show, encseq_toolbox);
  gt_option_parser_set_min_args(op, 1);
  return op;
}

static int gt_encseq_runner(int argc, const char **argv, int parsed_args,
                         void *tool_arguments, GtError *err)
{
  GtToolbox *encseq_toolbox = tool_arguments;
  GtToolfunc toolfunc;
  GtTool *tool = NULL;
  int had_err = 0;
  char **nargv = NULL;

  gt_error_check(err);
  gt_assert(encseq_toolbox);

  /* get encseq tools */
  if (!gt_toolbox_has_tool(encseq_toolbox, argv[parsed_args])) {
    gt_error_set(err, "encseq tool '%s' not found; option -help lists "
                      "possible tools", argv[parsed_args]);
    had_err = -1;
  }

  /* call encseq tool */
  if (!had_err) {
    if (!(toolfunc = gt_toolbox_get(encseq_toolbox, argv[parsed_args]))) {
      tool = gt_toolbox_get_tool(encseq_toolbox, argv[parsed_args]);
      gt_assert(tool);
    }
    nargv = gt_cstr_array_prefix_first(argv + parsed_args,
                                       gt_error_get_progname(err));
    gt_error_set_progname(err, nargv[0]);
    if (toolfunc)
      had_err = toolfunc(argc - parsed_args, (const char**) nargv, err);
    else
      had_err = gt_tool_run(tool, argc - parsed_args, (const char**) nargv,
                            err);
  }

  /* free */
  gt_cstr_array_delete(nargv);

  return had_err;
}

GtTool* gt_encseq(void)
{
  return gt_tool_new(gt_encseq_arguments_new,
                     gt_encseq_arguments_delete,
                     gt_encseq_option_parser_new,
                     NULL,
                     gt_encseq_runner);
}
