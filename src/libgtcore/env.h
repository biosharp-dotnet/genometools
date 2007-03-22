/*
  Copyright (c) 2007 Gordon Gremme <gremme@zbh.uni-hamburg.de>
  Copyright (c) 2007 Center for Bioinformatics, University of Hamburg
  See LICENSE file or http://genometools.org/license.html for license details.
*/

#ifndef ENV_H
#define ENV_H

/* the enviroment class (creates and holds all singular objects) */
typedef struct Env Env;

#include <libgtcore/ma.h>
#include <libgtcore/error.h>
#include <libgtcore/fa.h>
#include <libgtcore/log.h>

Env*   env_new(void);
MA*    env_ma(const Env*);    /* return the memory allocator */
Error* env_error(const Env*); /* return the error object */
Log*   env_log(const Env*);   /* return the log object or NULL */
void   env_set_log(Env*, Log*);
int    env_delete(Env*);

/* wrapper for memory functions */
#define env_ma_malloc(env, size)\
        ma_malloc_mem(env_ma(env), size, __FILE__, __LINE__)
#define env_ma_calloc(env, nmemb, size)\
        ma_calloc_mem(env_ma(env), nmemb, size, __FILE__, __LINE__)
#define env_ma_realloc(env, ptr, size)\
        ma_realloc_mem(env_ma(env), ptr, size, __FILE__, __LINE__)
#define env_ma_free(ptr, env)\
        ma_free_mem(ptr, env_ma(env), __FILE__, __LINE__)
/* free functions get the data object (here the env object) _always_ as the
   last object */
void    env_ma_free_func(void *ptr, Env*);

/* wrapper for error functions */
void    env_error_set(Env*, const char *format, ...)
          __attribute__ ((format (printf, 2, 3)));
#define env_error_is_set(env)\
        error_is_set(env_error(env))
#define env_error_unset(env)\
        error_unset(env_error(env))
#define env_error_get(env)\
        error_get(env_error(env))
/* make sure that the error is not set, has to be used at the beginning of
   every routine which has an Env* argument */
#define env_error_check(env)\
        assert(!env || !env_error(env) || !error_is_set(env_error(env)))

/* wrapper for log functions */
void    env_log_log(Env*, const char *format, ...)
          __attribute__ ((format (printf, 2, 3)));

#endif
