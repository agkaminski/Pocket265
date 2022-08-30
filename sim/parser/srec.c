#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "srec.h"
#include "common/error.h"

typedef struct {
	u16 addr;
	u8 size;
	u8 type;
	u8 data[];
} srecline_t;

static int srec_nextLine(int fd, char *buff, size_t bufflen)
{
	char c;
	int ret, count = 0;

	ret = read(fd, &c, 1);

	while (ret > 0 && c != ':')
		ret = read(fd, &c, 1);

	while (ret > 0 && c != '\n' && c != '\r' && c != '\0') {
		if (count > bufflen - 2) {
			WARN("Buffer depleted, aborting");
			return -1;
		}

		buff[count] = c;
		count += ret;

		ret = read(fd, &c, 1);
	}

	if (ret < 0) {
		WARN("Error reading file");
		return -1;
	}

	buff[count + 1] = '\0';

	DEBUG("Read line %s", buff);

	return count;
}

static int srec_hextonibble(char *hex, u8 *nibble)\
{
	if (*hex == '\0')
		return -1;

	if (*hex >= '0' && *hex <= '9')
		*nibble = *hex - '0';
	else if (*hex >= 'a' && *hex <= 'f')
		*nibble = *hex - 'a' + 10;
	else if (*hex >= 'A' && *hex <= 'F')
		*nibble = *hex - 'A' + 10;
	else
		return -1;

	return 0;
}

static int srec_hextobyte(char *buff, u8 *data)
{
	u8 low, high;

	if (srec_hextonibble(&buff[0], &high))
		return -1;
	if (srec_hextonibble(&buff[1], &low))
		return -1;

	*data = (high << 4)  + low;

	return 0;
}

static int srec_parseLine(srecline_t *data, size_t datalen, char *buff, size_t bufflen)
{
	unsigned int i;
	u8 tmp;
	u8 chksum;

	if (bufflen < 12) {
		WARN("Buffer can't hold single SREC line");
		return -1;
	}

	if (*buff != 'S') {
		WARN("Corrupted SREC file, no S start code");
		return -1;
	}

	buff += 1;

	data->type = *buff - '0';

	if (data->type > 9) {
		WARN("Corrupted SREC file, type is not 0 to 9");
		return -1;
	}

	buff += 1;

	if (srec_hextobyte(buff, &data->size)) {
		WARN("Corrupted SREC file, not a hex value");
		return -1;
	}

	buff += 2;
	chksum = data->size;

	if (data->size + 12 > bufflen || data->size > datalen) {
		WARN("Buffer can't hold this SREC line (data size %u)", data->size);
		return -1;
	}

	if (srec_hextobyte(buff, &tmp)) {
		WARN("Corrupted ihex file, not a SREC value");
		return -1;
	}

	buff += 2;
	data->addr = tmp;
	chksum += tmp;

	if (srec_hextobyte(buff, &tmp)) {
		WARN("Corrupted SREC file, not a hex value");
		return -1;
	}

	buff += 2;
	chksum += tmp;
	data->addr = (data->addr << 8) + tmp;

	for (i = 0; i < data->size; ++i) {
		if (srec_hextobyte(buff, &data->data[i])) {
			WARN("Corrupted SREC file, not a hex value");
			return -1;
		}

		buff += 2;
		chksum += data->data[i];
	}

	if (srec_hextobyte(buff, &tmp)) {
		WARN("Corrupted SREC file, not a hex value");
		return -1;
	}

	chksum += tmp;

	if (chksum != 0) {
		WARN("Checksum error");
		return -1;
	}

#ifndef NDEBUG
	DEBUG("SREC info: size 0x%02x, addr 0x%04x, type 0x%02x, data:", data->size, data->addr, data->type);
	for (i = 0; i < data->size; ++i) {
		if (!(i % 16)) {
			fprintf(stderr, "\n");
			fprintf(stderr, "0x%04x", data->addr + i);
			fprintf(stderr, "\t");
		}

		fprintf(stderr, "0x%02x ", data->data[i]);
	}
	fprintf(stderr, "\n\n");
#endif

	return 0;
}

int srec_parse(const char *path, u16 offset, u8 *buff, size_t bufflen)
{
	srecline_t *data;
	char line[256 + 12];
	u8 data_buff[256 + sizeof(srecline_t)];
	int fd, ret, count = 0, linecnt = 0;

	data = (srecline_t *)data_buff;

	if ((fd = open(path, O_RDONLY)) < 0) {
		WARN("Could not open file %s", path);
		return -1;
	}

	while (1) {
		ret = srec_nextLine(fd, line, sizeof(line));

		if (ret < 0) {
			close(fd);
			return -1;
		}
		else if (ret == 0) {
			break;
		}

		++linecnt;

		if (srec_parseLine(data, 256, line, sizeof(line)) < 0) {
			close(fd);
			return -1;
		}

		if (data->type == 9) {
			DEBUG("EOF record");
			break;
		}

		if (data->type == 0) {
			DEBUG("Header record");
			continue;
		}

		if (data->type != 1) {
			WARN("Unsupported record type 0x%02x, ignoring", data->type);
			continue;
		}

		if (data->addr + data->size + offset > bufflen) {
			data->size = bufflen - data->addr - offset;
			WARN("Not writing data above buffer limit");
		}

		memcpy(buff + data->addr + offset, data->data, data->size);
		count += data->size;
	}

	close(fd);

	DEBUG("Parsed %d lines, extracted %d bytes. Done.", linecnt, count);
	INFO("Read %u bytes from SREC file %s", count, path);

	return count;
}
