#ifndef MKR_CONVER_H_INCLUDED
#define MKR_CONVER_H_INCLUDED

class Convert
{
public:
	static int ToInt32(unsigned char *b, int index);
	static void Int32ToBytes(int val, unsigned char *buf, int index);
	static bool getBitAt(int buffer, int bitIndex);

	// .. lots of great stuff
};


#endif