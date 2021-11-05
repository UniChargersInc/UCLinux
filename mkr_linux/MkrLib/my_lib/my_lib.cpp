#include "my_lib.h"
//#include "MkrLib.h"

char *mkr_my_strdup(const char *src)
{
#if 1
	if (src == 0)
		return 0;
	size_t len = strlen(src) + 1;
	if (len > 0)
	{
		char *dst = (char*)calloc(len, sizeof(char));
		if (dst != 0)
		{
			strcpy(dst, src);
			return dst;
		}
	}
	return 0;
#else
	return strdup(src);
#endif
}