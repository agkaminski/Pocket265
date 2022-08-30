#ifndef _SREC_H_
#define _SREC_H_

#include "common/types.h"

int srec_parse(const char *path, u16 offset, u8 *buff, size_t bufflen);

#endif
