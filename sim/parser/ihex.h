#ifndef _IHEX_H_
#define _IHEX_H_

#include "common/types.h"

int ihex_parse(const char *path, u16 offset, u8 *buff, size_t bufflen);

#endif
