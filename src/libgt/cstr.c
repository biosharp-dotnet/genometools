/*
  Copyright (c) 2006 Gordon Gremme <gremme@zbh.uni-hamburg.de>
  Copyright (c) 2006 Center for Bioinformatics, University of Hamburg
  See LICENSE file or http://genometools.org/license.html for license details.
*/

#include <assert.h>
#include <stdlib.h>
#include "cstr.h"
#include "xansi.h"

void cstr_show(const char *cstr, unsigned long length, FILE *fp)
{
  unsigned long i;
  assert(cstr && fp);
  for (i = 0; i < length; i++)
    xfputc(cstr[i], fp);
}

char** cstr_array_prefix_first(char **cstr_array, const char *p)
{
  unsigned long i, a_len, f_len;
  char **a;
  assert(cstr_array && p);
  a_len = cstr_array_size(cstr_array);
  a = xmalloc(sizeof (char*) * (a_len + 1));
  f_len = strlen(p) + strlen(cstr_array[0]) + 2; /* blank + '\0' */
  a[0] = xmalloc(sizeof (char) * f_len);
  (void) snprintf(a[0], f_len, "%s %s", p, cstr_array[0]);
  for (i = 1; i < a_len; i++)
    a[i] = xstrdup(cstr_array[i]);
  a[a_len] = NULL;
  return a;
}

unsigned long cstr_array_size(char **cstr_array)
{
  unsigned long i = 0;
  while (cstr_array[i])
    i++;
  return i;
}

void cstr_array_free(char **cstr_array)
{
  unsigned long i = 0;
  if (!cstr_array) return;
  while (cstr_array[i])
    free(cstr_array[i++]);
  free(cstr_array);
}
