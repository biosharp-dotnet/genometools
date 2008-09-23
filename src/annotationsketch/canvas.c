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

#include <assert.h>
#include <math.h>
#include <string.h>
#include "core/class_alloc.h"
#include "core/ensure.h"
#include "core/log.h"
#include "core/ma.h"
#include "core/minmax.h"
#include "core/range.h"
#include "core/str.h"
#include "core/unused_api.h"
#include "annotationsketch/block.h"
#include "annotationsketch/canvas.h"
#include "annotationsketch/canvas_members.h"
#include "annotationsketch/canvas_rep.h"
#include "annotationsketch/graphics.h"
#include "annotationsketch/line.h"
#include "annotationsketch/style.h"

#define MARGINS_DEFAULT           10
#define BAR_HEIGHT_DEFAULT        15
#define BAR_VSPACE_DEFAULT        10
#define TOY_TEXT_HEIGHT          8.0
#define TRACK_VSPACE_DEFAULT      20
#define CAPTION_BAR_SPACE_DEFAULT  7
#define MIN_LEN_BLOCK_DEFAULT     30
#define ARROW_WIDTH_DEFAULT        6
#define STROKE_WIDTH_DEFAULT     0.6
#define FONT_SIZE_DEFAULT         10

#define HEADER_SPACE              70
#define FOOTER_SPACE              20

typedef enum
{
  CLIPPED_RIGHT,
  CLIPPED_LEFT,
  CLIPPED_NONE,
  CLIPPED_BOTH
} ClipType;

struct GtCanvasClass {
  size_t size;
  GtCanvasVisitDiagramFunc visit_diagram_pre,
                           visit_diagram_post;
  GtCanvasFreeFunc free;
};

const GtCanvasClass* gt_canvas_class_new(size_t size,
                                         GtCanvasVisitDiagramFunc visit_pre,
                                         GtCanvasVisitDiagramFunc visit_post,
                                         GtCanvasFreeFunc free)
{
  GtCanvasClass *c_class = gt_class_alloc(sizeof *c_class);
  c_class->size = size;
  c_class->visit_diagram_pre = visit_pre;
  c_class->visit_diagram_post = visit_post;
  c_class->free = free;
  return c_class;
}

/* Calculate the final height of the image to be created. */
unsigned long gt_canvas_calculate_height(GtCanvas *canvas, GtDiagram *dia)
{
  GtTracklineInfo lines;
  double tmp;
  unsigned long height;
  unsigned long gt_line_height;
  assert(dia && canvas);

  /* get line information for height calculation */
  lines.total_captionlines = lines.total_lines = 0;
  gt_diagram_get_lineinfo(dia, &lines);

  /* obtain line height and spacer from style */
  if (gt_style_get_num(canvas->pvt->sty, "format", "bar_height", &tmp, NULL))
    gt_line_height = tmp;
  else
    gt_line_height = BAR_HEIGHT_DEFAULT;
  if (gt_style_get_num(canvas->pvt->sty, "format", "bar_vspace", &tmp, NULL))
    gt_line_height += tmp;
  else
    gt_line_height += BAR_VSPACE_DEFAULT;

  /* get total height of all lines */
  height  = lines.total_lines * gt_line_height;
  height += lines.total_captionlines * (TOY_TEXT_HEIGHT
                                          + CAPTION_BAR_SPACE_DEFAULT);
  /* add track caption height and spacer */
  if (canvas->pvt->show_track_captions)
  {
    if (gt_style_get_num(canvas->pvt->sty, "format", "gt_track_vspace", &tmp,
                         NULL))
      height += gt_diagram_get_number_of_tracks(dia)
                  * (TOY_TEXT_HEIGHT
                      + CAPTION_BAR_SPACE_DEFAULT
                      + tmp);
    else
      height += gt_diagram_get_number_of_tracks(dia)
                  * (TOY_TEXT_HEIGHT
                      + CAPTION_BAR_SPACE_DEFAULT
                      + TRACK_VSPACE_DEFAULT);
  }

  /* add header space and footer */
  height += HEADER_SPACE + FOOTER_SPACE;
  gt_log_log("calculated height: %lu", height);
  return height;
}

double gt_canvas_get_text_width(GtCanvas *canvas, const char *text)
{
  assert(canvas);
  if (!text) return 0.0;
  return gt_graphics_get_text_width(canvas->pvt->g, text);
}

static double convert_point(GtCanvas *canvas, long pos)
{
  return (double) ((canvas->pvt->factor *
                      MAX(0,(pos-(long) canvas->pvt->viewrange.start)))
                      + canvas->pvt->margins);
}

/* Converts base range <node_range> into a pixel range.
   If the range exceeds visibility boundaries, clipping info is set. */
GtDrawingRange gt_canvas_convert_coords(GtCanvas *canvas, GtRange node_range)
{
  GtDrawingRange converted_range;
  converted_range.clip = CLIPPED_NONE;
  node_range.end++;
  /* scale coordinates to target image width */
  /* first, check if left side has to be clipped */
  if ((long) node_range.start < (long) canvas->pvt->viewrange.start )
  {
    converted_range.clip = CLIPPED_LEFT;
    converted_range.start = MAX(0.0, canvas->pvt->margins - 5);
  }
  else
  {
    converted_range.start = convert_point(canvas, node_range.start);
  }
  /* then, check right side. */
  if ((long) node_range.end > (long) canvas->pvt->viewrange.end+1)
  {
    converted_range.clip = (converted_range.clip == CLIPPED_LEFT ?
                                                      CLIPPED_BOTH :
                                                      CLIPPED_RIGHT);
    converted_range.end = (double) canvas->pvt->width - canvas->pvt->margins
                                + 5;
  }
  else
  {
    converted_range.end = convert_point(canvas, node_range.end);
  }
  return converted_range;
}

/* Formats a given position number for short display in the ruler. */
static void format_ruler_label(char *txt, unsigned long pos, size_t buflen)
{
  assert(txt);
  double fpos;
  int logval;
  GtStr *formatstring;

  logval = (int) floor(log10(pos));
  formatstring = gt_str_new_cstr("%.");

  if (pos >= 1000000000)
  {
    fpos = (double) pos / 1000000000;
    while (pos % 10 == 0)
    {
      pos /= 10;
      logval--;
    }
    gt_str_append_ulong(formatstring, (unsigned long) logval);
    gt_str_append_cstr(formatstring, "fG");
    (void) snprintf(txt, buflen, gt_str_get(formatstring), fpos);
  }
  else if (pos >= 1000000)
  {
    fpos = (double) pos / 1000000;
    while (pos % 10 == 0)
    {
      pos /= 10;
      logval--;
    }
    gt_str_append_ulong(formatstring, (unsigned long) logval);
    gt_str_append_cstr(formatstring, "fM");
    (void) snprintf(txt, buflen, gt_str_get(formatstring), fpos);
  }
  else if (pos >= 1000)
  {
    fpos = (double) pos / 1000;
    while (pos % 10 == 0)
    {
      pos /= 10;
      logval--;
    }
    gt_str_append_ulong(formatstring, (unsigned long) logval);
    gt_str_append_cstr(formatstring, "fK");
    (void) snprintf(txt, buflen, gt_str_get(formatstring), fpos);
  } else
    (void) snprintf(txt, buflen, "%li", pos);
  gt_str_delete(formatstring);
}

/* Renders a ruler with dynamic scale labeling and optional grid. */
void gt_canvas_draw_ruler(GtCanvas *canvas)
{
  double step, minorstep, vmajor, vminor, margins;
  long base_length, tick;
  GtColor rulercol, gridcol;
  char str[BUFSIZ];
  bool showgrid;

  assert(canvas);

  margins = canvas->pvt->margins;

  if (!(gt_style_get_bool(canvas->pvt->sty, "format","show_grid", &showgrid,
                          NULL)))
    showgrid = true;

  rulercol.red = rulercol.green = rulercol.blue = .2;
  gridcol.red = gridcol.green = gridcol.blue = .93;

  /* determine range and step of the scale */
  base_length = gt_range_length(canvas->pvt->viewrange);

  /* determine tick steps */
  step = pow(10,ceil(log10(base_length))-1);
  minorstep = step/10.0;

  /* calculate starting positions */
  vminor = (double) (floor(canvas->pvt->viewrange.start / minorstep))*minorstep;
  vmajor = (double) (floor(canvas->pvt->viewrange.start / step))*step;

  /* draw major ticks */
  for (tick = vmajor; tick <= canvas->pvt->viewrange.end; tick += step)
  {
    if (tick < canvas->pvt->viewrange.start) continue;
    gt_graphics_draw_vertical_line(canvas->pvt->g,
                                convert_point(canvas, tick),
                                30,
                                rulercol,
                                10);
    format_ruler_label(str, tick, BUFSIZ);
    gt_graphics_draw_text_centered(canvas->pvt->g,
                                convert_point(canvas, tick),
                                20,
                                str);
  }
  /* draw minor ticks */
  if (minorstep >= 1)
  {
    for (tick = vminor; tick <= canvas->pvt->viewrange.end; tick += minorstep)
    {
      if (tick < canvas->pvt->viewrange.start) continue;
      if (showgrid)
      {
        gt_graphics_draw_vertical_line(canvas->pvt->g,
                                    convert_point(canvas, tick),
                                    40,
                                    gridcol,
                                    canvas->pvt->height-40-15);
      }
      gt_graphics_draw_vertical_line(canvas->pvt->g,
                                  convert_point(canvas, tick),
                                  35,
                                  rulercol,
                                  5);
    }
  }
  /* draw ruler line */
  gt_graphics_draw_horizontal_line(canvas->pvt->g,
                                canvas->pvt->margins,
                                40,
                                canvas->pvt->width-2*margins);
  /* put 3' and 5' captions at the ends */
  gt_graphics_draw_text_centered(canvas->pvt->g,
                              canvas->pvt->margins-10,
                              45-(TOY_TEXT_HEIGHT/2),
                              "5'");
  gt_graphics_draw_text_centered(canvas->pvt->g,
                              canvas->pvt->width-canvas->pvt->margins+10,
                              45-(TOY_TEXT_HEIGHT/2),
                              "3'");
}

GtCanvas* gt_canvas_create(const GtCanvasClass *cc)
{
  GtCanvas *c;
  assert(cc && cc->size);
  c = gt_calloc(1, cc->size);
  c->c_class = cc;
  c->pvt = gt_calloc(1, sizeof (GtCanvasMembers));
  return c;
}

void gt_canvas_delete(GtCanvas *canvas)
{
  if (!canvas) return;
  assert(canvas->c_class);
  if (canvas->c_class->free)
    canvas->c_class->free(canvas);
  if (canvas->pvt->g)
    gt_graphics_delete(canvas->pvt->g);
  if (canvas->pvt->bt)
    gt_bittab_delete(canvas->pvt->bt);
  gt_free(canvas->pvt);
  gt_free(canvas);
}

void* gt_canvas_cast(GT_UNUSED const GtCanvasClass *cc, GtCanvas *c)
{
  assert(cc && c && c->c_class == cc);
  return c;
}

void* gt_canvas_try_cast(GT_UNUSED const GtCanvasClass *cc, GtCanvas *c)
{
  assert(cc && c);
  if (c->c_class == cc)
    return c;
  return NULL;
}

unsigned long gt_canvas_get_height(GtCanvas *canvas)
{
  assert(canvas);
  return canvas->pvt->height;
}

int gt_canvas_visit_diagram_pre(GtCanvas *canvas, GtDiagram* diagram)
{
  assert(canvas && diagram);
  return canvas->c_class->visit_diagram_pre(canvas, diagram);
}

int gt_canvas_visit_diagram_post(GtCanvas *canvas, GtDiagram* diagram)
{
  assert(canvas && diagram);
  return canvas->c_class->visit_diagram_post(canvas, diagram);
}

int gt_canvas_visit_track_pre(GtCanvas *canvas, GtTrack *track)
{
  int had_err = 0;
  unsigned long exceeded;
  GtColor color;

  assert(canvas && track);

  gt_style_get_color(canvas->pvt->sty, "format", "gt_track_title_color", &color,
                     NULL);

  /* debug */
  gt_log_log("processing track %s", gt_str_get(gt_track_get_title(track)));

  if (canvas->pvt->show_track_captions)
  {
    /* draw track title */
    gt_graphics_draw_colored_text(canvas->pvt->g,
                               canvas->pvt->margins,
                               canvas->pvt->y,
                               color,
                               gt_str_get(gt_track_get_title(track)));

    /* draw 'line maximum exceeded' message */
    if ((exceeded = gt_track_get_number_of_discarded_blocks(track)) > 0)
    {
      char buf[BUFSIZ];
      const char *msg;
      double width;
      GtColor red;
      red.red   = 0.7;
      red.green = red.blue  = 0.4;
      if (exceeded == 1)
        msg = "(1 block not shown due to exceeded line limit)";
      else
      {
        msg = "(%lu blocks not shown due to exceeded line limit)";
        snprintf(buf, BUFSIZ, msg, exceeded);
      }
      width = gt_graphics_get_text_width(canvas->pvt->g,
                                      gt_str_get(gt_track_get_title(track)));
      gt_graphics_draw_colored_text(canvas->pvt->g,
                                 canvas->pvt->margins+width+10.0,
                                 canvas->pvt->y,
                                 red,
                                 buf);
    }
    canvas->pvt->y += TOY_TEXT_HEIGHT + CAPTION_BAR_SPACE_DEFAULT;
  }
  return had_err;
}

int gt_canvas_visit_track_post(GtCanvas *canvas, GT_UNUSED GtTrack *track)
{
  double vspace;
  assert(canvas && track);
  /* put track spacer after track */
  if (gt_style_get_num(canvas->pvt->sty, "format", "gt_track_vspace", &vspace,
                       NULL))
    canvas->pvt->y += vspace;
  else
    canvas->pvt->y += TRACK_VSPACE_DEFAULT;
  return 0;
}

int gt_canvas_visit_line_pre(GtCanvas *canvas, GtLine *line)
{
  int had_err = 0;
  assert(canvas && line);
  canvas->pvt->bt = gt_bittab_new(canvas->pvt->width);
  if (gt_line_has_captions(line))
    canvas->pvt->y += TOY_TEXT_HEIGHT + CAPTION_BAR_SPACE_DEFAULT;
  return had_err;
}

int gt_canvas_visit_line_post(GtCanvas *canvas, GT_UNUSED GtLine *line)
{
  int had_err = 0;
  double tmp;
  assert(canvas && line);
  if (gt_style_get_num(canvas->pvt->sty, "format", "bar_height", &tmp, NULL))
    canvas->pvt->y += tmp;
  else
    canvas->pvt->y += BAR_HEIGHT_DEFAULT;
  if (gt_style_get_num(canvas->pvt->sty, "format", "bar_vspace", &tmp, NULL))
    canvas->pvt->y += tmp;
  else
    canvas->pvt->y += BAR_VSPACE_DEFAULT;
  gt_bittab_delete(canvas->pvt->bt);
  canvas->pvt->bt = NULL;
  return had_err;
}

int gt_canvas_visit_block(GtCanvas *canvas, GtBlock *block)
{
  int had_err = 0, arrow_status = ARROW_NONE;
  GtRange block_range;
  GtDrawingRange draw_range;
  GtColor grey, fillcolor, strokecolor;
  double bar_height, min_len_block, arrow_width, stroke_width;
  const char* caption;
  GtStrand strand;

  assert(canvas && block);

  grey.red = grey.green = grey.blue = .85;
  strand = gt_block_get_strand(block);
  block_range = gt_block_get_range(block);
  if (!gt_style_get_num(canvas->pvt->sty, "format", "bar_height", &bar_height,
                        NULL))
    bar_height = BAR_HEIGHT_DEFAULT;
  if (!gt_style_get_num(canvas->pvt->sty, "format", "min_len_block",
                        &min_len_block,
                        NULL))
    min_len_block = MIN_LEN_BLOCK_DEFAULT;
  if (!gt_style_get_num(canvas->pvt->sty, "format", "arrow_width", &arrow_width,
                        NULL)) {
    arrow_width = ARROW_WIDTH_DEFAULT;
  }
  if (!gt_style_get_num(canvas->pvt->sty, "format", "stroke_width",
                        &stroke_width,
                        NULL))
    stroke_width = STROKE_WIDTH_DEFAULT;

  if (strand == GT_STRAND_REVERSE || strand == GT_STRAND_BOTH)
    arrow_status = ARROW_LEFT;
  if (strand == GT_STRAND_FORWARD || strand == GT_STRAND_BOTH)
    arrow_status = (arrow_status == ARROW_LEFT ? ARROW_BOTH : ARROW_RIGHT);

  /* draw block caption */
  draw_range = gt_canvas_convert_coords(canvas, block_range);
  if (gt_block_caption_is_visible(block)) {
    caption = gt_str_get(gt_block_get_caption(block));
    if (caption)
    {
      gt_graphics_draw_text(canvas->pvt->g,
                         MAX(canvas->pvt->margins, draw_range.start),
                         canvas->pvt->y -CAPTION_BAR_SPACE_DEFAULT,
                         caption);
    }
  }

  /* do not draw further details in very small blocks */
  if (!gt_block_has_only_one_fullsize_element(block)
       && draw_range.end-draw_range.start < min_len_block)
  {
    const char *btype = gt_block_get_type(block);
    gt_style_get_color(canvas->pvt->sty, btype, "fill", &fillcolor,
                     (GtFeatureNode*) gt_block_get_top_level_feature(block));
    gt_style_get_color(canvas->pvt->sty, btype, "stroke", &strokecolor,
                     (GtFeatureNode*) gt_block_get_top_level_feature(block));
    gt_graphics_draw_box(canvas->pvt->g,
                      draw_range.start,
                      canvas->pvt->y,
                      draw_range.end-draw_range.start+1,
                      bar_height,
                      fillcolor,
                      arrow_status,
                      arrow_width,
                      1,
                      strokecolor,
                      true);
    /* draw arrowheads at clipped margins */
    if (draw_range.clip == CLIPPED_LEFT || draw_range.clip == CLIPPED_BOTH)
        gt_graphics_draw_arrowhead(canvas->pvt->g,
                                canvas->pvt->margins-10,
                                canvas->pvt->y+((bar_height-8)/2),
                                grey,
                                ARROW_LEFT);
    if (draw_range.clip == CLIPPED_RIGHT || draw_range.clip == CLIPPED_BOTH)
        gt_graphics_draw_arrowhead(canvas->pvt->g,
                                canvas->pvt->width-canvas->pvt->margins+10,
                                canvas->pvt->y+((bar_height-8)/2),
                                grey,
                                ARROW_RIGHT);
    /* register coordinates in GtImageInfo object if available */
    if (canvas->pvt->ii)
    {
      GtRecMap *rm = gt_recmap_new(draw_range.start, canvas->pvt->y,
                                    draw_range.end, canvas->pvt->y+bar_height,
                                    (GtFeatureNode*) /* XXX */
                                    gt_block_get_top_level_feature(block));
      gt_image_info_add_recmap(canvas->pvt->ii, rm);
      rm->has_omitted_children = true;
    }
    return -1;
  }

  gt_style_get_color(canvas->pvt->sty, "format", "default_stroke_color",
                     &strokecolor, NULL);

  /* draw parent block boundaries */
  gt_graphics_draw_dashes(canvas->pvt->g,
                       draw_range.start,
                       canvas->pvt->y,
                       draw_range.end - draw_range.start,
                       bar_height,
                       ARROW_NONE,
                       arrow_width,
                       stroke_width,
                       strokecolor);
  return had_err;
}

int gt_canvas_visit_element(GtCanvas *canvas, GtElement *elem)
{
  int had_err = 0, arrow_status = ARROW_NONE;
  GtRange elem_range = gt_element_get_range(elem);
  GtDrawingRange draw_range;
  double elem_start, elem_width, stroke_width, bar_height, arrow_width;
  GtColor elem_color, grey, fill_color;
  const char *type;
  GtStr *style;
  GtStrand strand = gt_element_get_strand(elem);

  assert(canvas && elem);

  /* This shouldn't happen. */
  if (!gt_range_overlap(elem_range, canvas->pvt->viewrange))
    return -1;

  type = gt_element_get_type(elem);
  grey.red = grey.green = grey.blue = .85;
  if (!gt_style_get_num(canvas->pvt->sty, "format", "bar_height", &bar_height,
                        NULL))
    bar_height = BAR_HEIGHT_DEFAULT;
  if (!gt_style_get_num(canvas->pvt->sty, "format", "arrow_width", &arrow_width,
                        NULL)) {
    arrow_width = ARROW_WIDTH_DEFAULT;
  }

  if ((strand == GT_STRAND_REVERSE || strand == GT_STRAND_BOTH)
         /*&& delem == gt_dlist_first(elems)*/)
    arrow_status = ARROW_LEFT;
  if ((strand == GT_STRAND_FORWARD || strand == GT_STRAND_BOTH)
         /*&& gt_dlistelem_next(delem) == NULL*/)
    arrow_status = (arrow_status == ARROW_LEFT ? ARROW_BOTH : ARROW_RIGHT);

  gt_log_log("processing element from %lu to %lu, strand %d\n",
             elem_range.start, elem_range.end, (int) strand);

  draw_range = gt_canvas_convert_coords(canvas, elem_range);
  elem_start = draw_range.start;
  elem_width = draw_range.end - draw_range.start;

  if (gt_element_is_marked(elem)) {
    gt_style_get_color(canvas->pvt->sty, type, "stroke_marked", &elem_color,
                    gt_element_get_node_ref(elem));
    if (!gt_style_get_num(canvas->pvt->sty, "format", "stroke_marked_width",
                       &stroke_width, gt_element_get_node_ref(elem)))
    stroke_width = STROKE_WIDTH_DEFAULT;
  }
  else {
    gt_style_get_color(canvas->pvt->sty, type, "stroke", &elem_color,
                    gt_element_get_node_ref(elem));
    if (!gt_style_get_num(canvas->pvt->sty, "format", "stroke_width",
                          &stroke_width,
                          gt_element_get_node_ref(elem)))
    stroke_width = STROKE_WIDTH_DEFAULT;
  }
  gt_style_get_color(canvas->pvt->sty, type, "fill", &fill_color,
                  gt_element_get_node_ref(elem));

  if (draw_range.end-draw_range.start <= 1.1)
  {
    if (gt_bittab_bit_is_set(canvas->pvt->bt, (unsigned long) draw_range.start))
      return had_err;
    gt_graphics_draw_vertical_line(canvas->pvt->g,
                                draw_range.start,
                                canvas->pvt->y,
                                elem_color,
                                bar_height);
    gt_bittab_set_bit(canvas->pvt->bt, (unsigned long) draw_range.start);
  }

  /* register coordinates in GtImageInfo object if available */
  if (canvas->pvt->ii)
  {
    GtRecMap *rm = gt_recmap_new(elem_start, canvas->pvt->y,
                                  elem_start+elem_width,
                                  canvas->pvt->y+bar_height,
                                  (GtFeatureNode*) /* XXX */
                                  gt_element_get_node_ref(elem));
    gt_image_info_add_recmap(canvas->pvt->ii, rm);
  }

  if (draw_range.end-draw_range.start <= 1.1)
  {
    return had_err;
  }

  gt_log_log("drawing element from %f to %f, arrow status: %d\n",
             draw_range.start, draw_range.end, arrow_status);

  /* draw each element according to style set in the style */
  style = gt_str_new();
  if (!gt_style_get_str(canvas->pvt->sty, type, "style", style,
                     gt_element_get_node_ref(elem)))
    gt_str_set(style, "box");

  if (strcmp(gt_str_get(style), "box")==0)
  {
    gt_graphics_draw_box(canvas->pvt->g,
                      elem_start,
                      canvas->pvt->y,
                      elem_width,
                      bar_height,
                      fill_color,
                      arrow_status,
                      arrow_width,
                      stroke_width,
                      elem_color,
                      false);
  }
  else if (strcmp(gt_str_get(style), "caret")==0)
  {
    gt_graphics_draw_caret(canvas->pvt->g,
                        elem_start,
                        canvas->pvt->y,
                        elem_width,
                        bar_height,
                        ARROW_NONE,
                        arrow_width,
                        stroke_width,
                        elem_color);
  }
  else if (strcmp(gt_str_get(style), "dashes")==0)
  {
    gt_graphics_draw_dashes(canvas->pvt->g,
                         elem_start,
                         canvas->pvt->y,
                         elem_width,
                         bar_height,
                         arrow_status,
                         arrow_width,
                         stroke_width,
                         elem_color);
  }
  else if (strcmp(gt_str_get(style), "line")==0)
  {
    gt_graphics_draw_horizontal_line(canvas->pvt->g,
                                  elem_start,
                                  canvas->pvt->y,
                                  elem_width);
  }
  else
  {
     gt_graphics_draw_box(canvas->pvt->g,
                       elem_start,
                       canvas->pvt->y,
                       elem_width,
                       bar_height,
                       fill_color,
                       arrow_status,
                       arrow_width,
                       stroke_width,
                       elem_color,
                       false);
  }
  gt_str_delete(style);

  /* draw arrowheads at clipped margins */
  if (draw_range.clip == CLIPPED_LEFT || draw_range.clip == CLIPPED_BOTH)
      gt_graphics_draw_arrowhead(canvas->pvt->g,
                              canvas->pvt->margins-10,
                              canvas->pvt->y+((bar_height-8)/2),
                              grey,
                              ARROW_LEFT);
  if (draw_range.clip == CLIPPED_RIGHT || draw_range.clip == CLIPPED_BOTH)
      gt_graphics_draw_arrowhead(canvas->pvt->g,
                              canvas->pvt->width-canvas->pvt->margins+10,
                              canvas->pvt->y+((bar_height-8)/2),
                              grey,
                              ARROW_RIGHT);
  return had_err;
}
