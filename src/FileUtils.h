
#pragma once

#include <sys/stat.h>

namespace
{
	off_t GetFileSize(int fd)
	{
		struct stat buf;
		int ret = fstat(fd, &buf);
		return ret == 0 ? buf.st_size : -1;
	}

	off_t GetFileSize(FILE *f)
	{
		return GetFileSize(fileno(f));
	}
}
