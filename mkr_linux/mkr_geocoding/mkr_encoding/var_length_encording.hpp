#pragma once

/**
 * C++ utilities for variable-length integer encoding
 * Compile with -std=c++11 or higher
 *
 * Version 1.1: Use size_t for size argument, improve function signatures
 *
 * License: CC0 1.0 Universal
 * Originally published on http://techoverflow.net
 * Copyright (c) 2015 Uli Koehler
 */
#ifndef MY__VARIABLE_LENGTH_INTEGER_H
#define MY__VARIABLE_LENGTH_INTEGER_H

#include <cstdint>

 /**
  * Encodes an unsigned variable-length integer using the MSB algorithm.
  * This function assumes that the value is stored as little endian.
  * @param value The input value. Any standard integer type is allowed.
  * @param output A pointer to a piece of reserved memory. Must have a minimum size dependent on the input size (32 bit = 5 bytes, 64 bit = 10 bytes).
  * @return The number of bytes used in the output memory.
  */
template<typename int_t = uint64_t>
size_t encodeVarint(int_t value, uint8_t* output) {
	size_t outputSize = 0;
	//While more than 7 bits of data are left, occupy the last output byte
	// and set the next byte flag
	while (value > 127) {
		//|128: Set the next byte flag
		output[outputSize] = ((uint8_t)(value & 127)) | 128;
		//Remove the seven bits we just wrote
		value >>= 7;
		outputSize++;
	}
	output[outputSize++] = ((uint8_t)value) & 127;
	return outputSize;
}

/**
 * Decodes an unsigned variable-length integer using the MSB algorithm.
 * @param value A variable-length encoded integer of arbitrary size.
 * @param inputSize How many bytes are
 */
template<typename int_t = uint64_t>
int_t decodeVarint(const uint8_t* input, size_t max_inputSize) {
	int_t ret = 0;
	for (size_t i = 0; i < max_inputSize; i++) {
		ret |= (input[i] & 127) << (7 * i);
		//If the next-byte flag is set
		if (!(input[i] & 128)) {
			break;
		}
	}
	return ret;
}

//https://stackoverflow.com/questions/6251889/type-condition-in-template
#include <type_traits> 

template <typename intType, typename uintType>
uintType zz_map(intType x)
{
	if (std::is_same<uintType, uint64_t>::value)
		return ((uintType)x << 1) ^ -((uintType)x >> 63);  // error: unary minus operator applied to unsigned type, result still unsigned -> VS2017 No (/sdl-)
	else if (std::is_same<uintType, uint32_t>::value)
		return ((uintType)x << 1) ^ -((uintType)x >> 31);

}
template <typename intType, typename uintType>
intType zz_unmap(uintType y)
{
	return (intType)((y >> 1) ^ -(y & 0x1));
}

#endif //__VARIABLE_LENGTH_INTEGER_H

