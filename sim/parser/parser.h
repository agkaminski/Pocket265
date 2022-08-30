#ifndef __PARSER_H_
#define __PARSER_H_

#include <stdlib.h>
#include "common/types.h"

int parse_file(const char *path, u16 offset, u8 *buff, size_t bufflen);

#endif
