#ifndef __RARCH_FEATURES_H
#define __RARCH_FEATURES_H

#include <stddef.h>
#include <stdbool.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_ZLIB
static const bool _zlib_supp = true;
#else
static const bool _zlib_supp = false;
#endif

#endif
