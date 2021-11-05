#include "Convert.h"

int Convert::ToInt32(unsigned char *b, int index)
{
	int i = (b[index + 3] << 24) | (b[index + 2] << 16) | (b[index + 1] << 8) | (b[index + 0]);
	return i;
}

bool Convert::getBitAt(int buffer, int bitIndex)
{
	bool isBitSet = false;
	// .. determine if bit is set
	return isBitSet;
}

void Convert::Int32ToBytes(int n, unsigned char *bytes, int index)
{
	bytes[index+3] = (n >> 24) & 0xFF;
	bytes[index+2] = (n >> 16) & 0xFF;
	bytes[index+1] = (n >> 8) & 0xFF;
	bytes[index+0] = n & 0xFF;
}