#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "binary.h"
#include "common/error.h"

int binary_parse(const char *path, u16 offset, u8 *buff, size_t bufflen)
{
	int count = offset, ret;
	int fd = open(path, O_RDONLY);

	if (fd < 0) {
		WARN("Could not open file %s", path);
		return -1;
	}

	DEBUG("Starting reading file %s", path);

	while (count < bufflen) {
		ret = read(fd, buff + count, (count + 256 >= bufflen) ? bufflen - count : 256);
		count += ret;

		DEBUG("bufflen %u count %u offset %u", (unsigned int)(bufflen), (unsigned int)count, offset);

		if (ret < 0) {
			WARN("Error while reading file %s", path);
			return -1;
		}
		else if (ret == 0) {
			DEBUG("Finished reading file %s", path);
			break;
		}

#ifndef NDEBUG
		DEBUG("Read %d bytes:", ret);
		for (int i = 0; i < ret; ++i) {
			if (!(i % 16)) {
				fprintf(stderr, "\n");
				fprintf(stderr, "0x%04x", count - ret + i);
				fprintf(stderr, "\t");
			}

			fprintf(stderr, "0x%02x ", *(buff + count - ret + i));
		}
		fprintf(stderr, "\n\n");
#endif
	}

	INFO("Read %u bytes from binary file %s", count - offset, path);

	return count - offset;
}
