/*
 *  Showtime Mediacenter
 *  Copyright (C) 2007-2013 Lonelycoder AB
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  This program is also available under a commercial proprietary license.
 *  For more information, contact andreas@lonelycoder.com
 */

#pragma once

#include "config.h"

#include <inttypes.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "arch/threads.h"
#include "misc/rstr.h"

#if __GNUC__ >= 4 && __GNUC_MINOR__ >=6
#define static_assert(x, y) _Static_assert(x, y)
#else
#define static_assert(x, y)
#endif

void parse_opts(int argc, char **argv);

void showtime_init(void);

void showtime_fini(void);

void showtime_swrefresh(void);

extern void panic(const char *fmt, ...) __attribute__((noreturn, format(printf, 1, 2)));

extern const char *showtime_dataroot(void);

#define HTS_GLUE(a, b) a ## b
#define HTS_JOIN(a, b) HTS_GLUE(a, b)

#define BYPASS_CACHE  ((int *)-1)
#define DISABLE_CACHE ((int *)-2)
#define NOT_MODIFIED ((void *)-1)

#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

#define ONLY_CACHED(p) ((p) != BYPASS_CACHE && (p) != NULL)

// NLS


#define _(string) nls_get_rstring(string)
#define _pl(a,b,c) nls_get_rstringp(a,b,c)
#define _p(string) nls_get_prop(string)

rstr_t *nls_get_rstring(const char *string);

struct prop;
struct prop *nls_get_prop(const char *string);

rstr_t *nls_get_rstringp(const char *string, const char *singularis, int val);

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#define URL_MAX 2048
#define HOSTNAME_MAX 256 /* FQDN is max 255 bytes including ending dot */

void showtime_shutdown(int retcode);

void showtime_flush_caches(void);

uint32_t showtime_get_version_int(void);

uint32_t showtime_parse_version_int(const char *str);

extern int64_t showtime_get_ts(void);

extern int64_t showtime_get_avtime(void);

extern const char *showtime_get_system_type(void);

extern uint64_t arch_get_seed(void);

/**
 *
 */

enum {
  TRACE_EMERG,
  TRACE_ERROR,
  TRACE_INFO,
  TRACE_DEBUG
};

#define TRACE_NO_PROP 0x1

void trace_init(void);

void trace_fini(void);

void trace(int flags, int level, const char *subsys, const char *fmt, ...);

void tracev(int flags, int level, const char *subsys, const char *fmt, va_list ap);

void trace_arch(int level, const char *prefix, const char *buf);

#define TRACE(level, subsys, fmt...) trace(0, level, subsys, fmt)

void hexdump(const char *pfx, const void *data, int len);

#define mystrdupa(n) ({ int my_l = strlen(n); \
 char *my_b = alloca(my_l + 1); \
 memcpy(my_b, n, my_l + 1); })

#define mystrndupa(n, len) ({ \
 char *my_b = alloca(len + 1); \
 my_b[len] = 0; \
 memcpy(my_b, n, len); \
})


static inline unsigned int mystrhash(const char *s)
{
  unsigned int v = 5381;
  while(*s)
    v += (v << 5) + v + *s++;
  return v;
}

static inline void mystrset(char **p, const char *s)
{
  free(*p);
  *p = s ? strdup(s) : NULL;
}


static inline const char *mystrbegins(const char *s1, const char *s2)
{
  while(*s2)
    if(*s1++ != *s2++)
      return NULL;
  return s1;
}

void my_localtime(const time_t *timep, struct tm *tm);

/*
 * Memory allocation wrappers
 * These are used whenever the caller can deal with failure 
 * Some platform may have the standard libc ones to assert() on
 * OOM conditions
 */

void *mymalloc(size_t size);

void *myrealloc(void *ptr, size_t size);

void *mycalloc(size_t count, size_t size);

void *mymemalign(size_t align, size_t size);

void runcontrol_activity(void);

void shutdown_hook_run(int early);

void *shutdown_hook_add(void (*fn)(void *opaque, int exitcode), void *opaque,
			int early);

#define SHOWTIME_EXIT_OK       0
#define SHOWTIME_EXIT_STANDBY  10
#define SHOWTIME_EXIT_POWEROFF 11
#define SHOWTIME_EXIT_LOGOUT   12
#define SHOWTIME_EXIT_RESTART  13
#define SHOWTIME_EXIT_SHELL    14
#define SHOWTIME_EXIT_REBOOT   15


typedef struct gconf {
  int exit_code;


  char *dirname;   // Directory where executable resides
  char *binary;    // Executable itself
  char *upgrade_path; // What to upgrade

  char *cache_path;
  char *persistent_path;

  int concurrency;
  int trace_level;
  int trace_to_syslog;
  int listen_on_stdin;
  int ffmpeglog;
  int noui;
  int fullscreen;
  int swrefresh;

  int can_standby;
  int can_poweroff;
  int can_open_shell;
  int can_logout;
  int can_restart;
  int can_not_exit;

  int disable_upnp;
  int disable_sd;


  int enable_bin_replace;
  int enable_omnigrade;
  int enable_http_debug;
  int disable_http_reuse;
  int enable_experimental;
  int enable_detailed_avdiff;
  int enable_hls_debug;
  int enable_ftp_debug;
  int enable_cec_debug;

  const char *devplugin;
  const char *plugin_repo;
  const char *load_jsfile;

  const char *initial_url;
  const char *initial_view;

  char *ui;
  char *skin;


  struct prop *settings_apps;
  struct prop *settings_sd;
  struct prop *settings_general;
  struct prop *settings_dev;
  struct prop *settings_network;
  struct prop_concat *settings_look_and_feel;

  hts_mutex_t state_mutex;
  hts_cond_t state_cond;

  int state_plugins_loaded;

  int fa_allow_delete;

  int ignore_the_prefix;

  uint32_t log_server_ipv4;
  int log_server_port;

  int showtime_shell_fd;

  char system_name[64];

} gconf_t;

extern gconf_t gconf;

/* From version.c */
extern const char *htsversion;
extern const char *htsversion_full;


typedef struct inithelper {
  struct inithelper *next;
  enum {
    INIT_GROUP_API,
    INIT_GROUP_IPC,
    INIT_GROUP_STATIC_APPS,
    INIT_GROUP_ASYNCIO,
  } group;
  void (*fn)(void);
} inithelper_t;

extern inithelper_t *inithelpers;

#define INITME(group_, func_)					   \
  static inithelper_t HTS_JOIN(inithelper, __LINE__) = {	   \
    .group = group_,						   \
    .fn = func_							   \
  };								   \
  static void  __attribute__((constructor))			   \
  HTS_JOIN(inithelperctor, __LINE__)(void)			   \
  {								   \
    inithelper_t *ih = &HTS_JOIN(inithelper, __LINE__);		   \
    ih->next = inithelpers;					   \
    inithelpers = ih;						   \
  }

void init_group(int group);
