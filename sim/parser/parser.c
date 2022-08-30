#include <string.h>
#include "common/error.h"
#include "parser.h"
#include "binary.h"
#include "ihex.h"
#include "srec.h"

static const char *srec_extentions[] = {
	"s19", "s28", "s37", "s", "s1",
	"s2", "s3", "sx", "srec", "mot"
};

static const char *get_extension(const char* str)
{
	const char *ptr;

	ptr = str + strlen(str);
	for (; ptr > str; --ptr) {
		if (*ptr == '.')
			break;
	}

	if (ptr == str)
		ptr = str + strlen(str);

	DEBUG("File extentention: %s", ptr);

	return ++ptr;
}

int parse_file(const char *path, u16 offset, u8 *buff, size_t bufflen)
{
	const char *extension;
	int i;

	extension = get_extension(path);

	if (strcmp(extension, "hex") == 0 || strcmp(extension, "ihex") == 0) {
		DEBUG("Parsing as intel HEX file");
		return ihex_parse(path, offset, buff, bufflen);
	}

	for (i = 0; i < sizeof(srec_extentions) / sizeof(srec_extentions[0]); ++i) {
		if (strcmp(extension, srec_extentions[i]) == 0) {
			DEBUG("Parsing as SREC file");
			return srec_parse(path, offset, buff, bufflen);
		}
	}

	DEBUG("Parsing as binary file");

	return binary_parse(path, offset, buff, bufflen);
}
