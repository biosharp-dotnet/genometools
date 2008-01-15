/*
  Copyright (c) 2007-2008 Gordon Gremme <gremme@zbh.uni-hamburg.de>
  Copyright (c) 2007-2008 Center for Bioinformatics, University of Hamburg

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

#ifndef HELPER_H
#define HELPER_H

#include "lua.h"
#include "libgtcore/error.h"

#ifdef LIBGTVIEW
#include "libgtview/config.h"

void put_config_in_registry(lua_State*, Config*);
Config* get_config_from_registry(lua_State*);
#endif

int  luaset_modules_path(lua_State*, Error*);

void luaset_arg(lua_State*, const char *argv_0, const char **argv);
void lua_export_metatable(lua_State*, const char *metatable_desc);

void lua_push_strarray_as_table(lua_State*, StrArray*);

/* Propagate the error given in <err> (which must be set) to <L>.
   Takes ownership of the error and deletes it. */
int  luagt_error(lua_State *L, Error *err);

#endif
