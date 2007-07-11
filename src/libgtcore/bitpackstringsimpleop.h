/*
** Copyright (C) 2007 Thomas Jahns <Thomas.Jahns@gmx.net>
**
** See LICENSE file or http://genometools.org/license.html for license details.
**
*/
#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "bitpackstring.h"

/**
 * \file bitpackstringsimpleop.h
 * \brief Trivial (i.e. inlined) operations on bitstrings.
 * \author Thomas Jahns <Thomas.Jahns@gmx.net>
 
 * The contents of this file is to be considered private
 * implementation detail but is exposed to the compiler solely for
 * performance reasons. Just pretend you didn't know how these
 * functions work.
 */

/* imitate llabs for platforms which don't have it */
#ifndef HAVE_LLABS
#define llabs(i) (((i) < 0)?-i:i)
#endif
static inline size_t
bitElemsAllocSize(BitOffset numBits)
{
  return numBits/bitElemBits + ((numBits%bitElemBits)?1:0);
}

static inline int8_t
bsGetInt8(BitString str, BitOffset offset, unsigned numBits)
{
  /* requires sign extension */
  int8_t m = 1 << (numBits - 1);
  return (bsGetUInt8(str, offset, numBits) ^ m) - m;
}

static inline int16_t
bsGetInt16(BitString str, BitOffset offset, unsigned numBits)
{
  /* requires sign extension */
  int16_t m = 1 << (numBits - 1);
  return (bsGetUInt16(str, offset, numBits) ^ m) - m;
}

static inline int32_t
bsGetInt32(BitString str, BitOffset offset, unsigned numBits)
{
  /* requires sign extension */
  int32_t m = 1 << (numBits - 1);
  return (bsGetUInt32(str, offset, numBits) ^ m) - m;
}

static inline int64_t
bsGetInt64(BitString str, BitOffset offset, unsigned numBits)
{
  /* requires sign extension */
  int64_t m = (int64_t)1 << (numBits - 1);
  return (bsGetUInt64(str, offset, numBits) ^ m) - m;
}

static inline void
bsStoreInt8(BitString str, BitOffset offset, unsigned numBits, uint8_t val)
{
  bsStoreUInt8(str, offset, numBits,  val);
}

static inline void
bsStoreInt16(BitString str, BitOffset offset, unsigned numBits, uint16_t val)
{
  bsStoreUInt16(str, offset, numBits,  val);
}

static inline void
bsStoreInt32(BitString str, BitOffset offset, unsigned numBits, uint32_t val)
{
  bsStoreUInt32(str, offset, numBits,  val);
}

static inline void
bsStoreInt64(BitString str, BitOffset offset, unsigned numBits, uint64_t val)
{
  bsStoreUInt64(str, offset, numBits,  val);
}

static inline int
requiredUInt8Bits(uint8_t v)
{
  return requiredUInt32Bits((uint32_t)v);
}

static inline int
requiredUInt16Bits(uint16_t v)
{
  return requiredUInt32Bits((uint32_t)v);
}

static inline int
requiredInt8Bits(int8_t v)
{
  if(v == INT8_MIN)
    return sizeof(v)*CHAR_BIT;
  else
    /* one extra for sign bit */
    return requiredUInt32Bits(abs(v)) + 1;
}

static inline int
requiredInt16Bits(int16_t v)
{
  if(v == INT16_MIN)
    return sizeof(v)*CHAR_BIT;
  else
    /* one extra for sign bit */
    return requiredUInt32Bits(abs(v)) + 1;
}

static inline int
requiredInt32Bits(int32_t v)
{
  if(v == INT32_MIN)
    return sizeof(v)*CHAR_BIT;
  else
    /* one extra for sign bit */
    return requiredUInt32Bits(labs(v)) + 1;
}

static inline int
requiredInt64Bits(int64_t v)
{
  if(v == INT64_MIN)
    return sizeof(v)*CHAR_BIT;
  else
    /* one extra for sign bit */
    return requiredUInt64Bits(llabs(v)) + 1;
}

static inline void
bsStoreUniformInt8Array(BitString str, BitOffset offset, unsigned numBits,
                       size_t numValues, const int8_t val[])
{
  bsStoreUniformUInt8Array(str, offset, numBits, numValues,
                           (const uint8_t *)val);
}

static inline void
bsStoreUniformInt16Array(BitString str, BitOffset offset, unsigned numBits,
                       size_t numValues, const int16_t val[])
{
  bsStoreUniformUInt16Array(str, offset, numBits, numValues,
                            (const uint16_t *)val);
}

static inline void
bsStoreUniformInt32Array(BitString str, BitOffset offset, unsigned numBits,
                       size_t numValues, const int32_t val[])
{
  bsStoreUniformUInt32Array(str, offset, numBits, numValues,
                            (const uint32_t *)val);
}

static inline void
bsStoreUniformInt64Array(BitString str, BitOffset offset, unsigned numBits,
                       size_t numValues, const int64_t val[])
{
  bsStoreUniformUInt64Array(str, offset, numBits, numValues,
                            (const uint64_t *)val);
}

static inline void
bsSignExpandArray8(int8_t val[], size_t numValues, unsigned numBits)
{
  int8_t m = 1 << (numBits - 1);
  size_t i;
  for(i = 0; i < numValues; ++i)
    val[i] = (val[i] ^ m) - m;
}

static inline void
bsSignExpandArray16(int16_t val[], size_t numValues, unsigned numBits)
{
  int16_t m = 1 << (numBits - 1);
  size_t i;
  for(i = 0; i < numValues; ++i)
    val[i] = (val[i] ^ m) - m;
}

static inline void
bsSignExpandArray32(int32_t val[], size_t numValues, unsigned numBits)
{
  int32_t m = ((int32_t)1) << (numBits - 1);
  size_t i;
  for(i = 0; i < numValues; ++i)
    val[i] = (val[i] ^ m) - m;
}

static inline void
bsSignExpandArray64(int64_t val[], size_t numValues, unsigned numBits)
{
  int64_t m = ((int64_t)1) << (numBits - 1);
  size_t i;
  for(i = 0; i < numValues; ++i)
    val[i] = (val[i] ^ m) - m;
}

static inline void
bsGetUniformInt8Array(const BitString str, BitOffset offset, unsigned numBits,
                     size_t numValues, int8_t val[])
{
  /* read blocksize many ints at once, for later sign expansion */
  int blockSize = 16 * getpagesize() / sizeof(val[0]);
  /* the factor (16) is completely arbitrary and needs some profiling */
  int8_t *blockPtr = val;
  size_t revIndex = numValues;
  BitOffset offsetTemp = offset;
  while(revIndex > blockSize)
  {
    bsGetUniformUInt8Array(str, offsetTemp, numBits,
                            blockSize, (uint8_t *)blockPtr);
    bsSignExpandArray8(blockPtr, blockSize, numBits);
    blockPtr += blockSize;
    revIndex -= blockSize;
    offsetTemp += blockSize * numBits;
  }
  bsGetUniformUInt8Array(str, offsetTemp, numBits,
                         revIndex, (uint8_t *)blockPtr);
  bsSignExpandArray8(blockPtr, revIndex, numBits);
}

static inline void
bsGetUniformInt16Array(const BitString str, BitOffset offset, unsigned numBits,
                     size_t numValues, int16_t val[])
{
  /* read blocksize many ints at once, for later sign expansion */
  int blockSize = 16 * getpagesize() / sizeof(val[0]);
  /* the factor (16) is completely arbitrary and needs some profiling */
  int16_t *blockPtr = val;
  size_t revIndex = numValues;
  BitOffset offsetTemp = offset;
  while(revIndex > blockSize)
  {
    bsGetUniformUInt16Array(str, offsetTemp, numBits,
                            blockSize, (uint16_t *)blockPtr);
    bsSignExpandArray16(blockPtr, blockSize, numBits);
    blockPtr += blockSize;
    revIndex -= blockSize;
    offsetTemp += blockSize * numBits;
  }
  bsGetUniformUInt16Array(str, offsetTemp, numBits,
                        revIndex, (uint16_t *)blockPtr);
  bsSignExpandArray16(blockPtr, revIndex, numBits);
}

static inline void
bsGetUniformInt32Array(const BitString str, BitOffset offset, unsigned numBits,
                     size_t numValues, int32_t val[])
{
  /* read blocksize many ints at once, for later sign expansion */
  int blockSize = 16 * getpagesize() / sizeof(val[0]);
  /* the factor (16) is completely arbitrary and needs some profiling */
  int32_t *blockPtr = val;
  size_t revIndex = numValues;
  BitOffset offsetTemp = offset;
  while(revIndex > blockSize)
  {
    bsGetUniformUInt32Array(str, offsetTemp, numBits,
                            blockSize, (uint32_t *)blockPtr);
    bsSignExpandArray32(blockPtr, blockSize, numBits);
    blockPtr += blockSize;
    revIndex -= blockSize;
    offsetTemp += blockSize * numBits;
  }
  bsGetUniformUInt32Array(str, offsetTemp, numBits,
                        revIndex, (uint32_t *)blockPtr);
  bsSignExpandArray32(blockPtr, revIndex, numBits);
}

static inline void
bsGetUniformInt64Array(const BitString str, BitOffset offset, unsigned numBits,
                     size_t numValues, int64_t val[])
{
  /* read blocksize many ints at once, for later sign expansion */
  int blockSize = 16 * getpagesize() / sizeof(val[0]);
  /* the factor (16) is completely arbitrary and needs some profiling */
  int64_t *blockPtr = val;
  size_t revIndex = numValues;
  BitOffset offsetTemp = offset;
  while(revIndex > blockSize)
  {
    bsGetUniformUInt64Array(str, offsetTemp, numBits,
                            blockSize, (uint64_t *)blockPtr);
    bsSignExpandArray64(blockPtr, blockSize, numBits);
    blockPtr += blockSize;
    revIndex -= blockSize;
    offsetTemp += blockSize * numBits;
  }
  bsGetUniformUInt64Array(str, offsetTemp, numBits,
                        revIndex, (uint64_t *)blockPtr);
  bsSignExpandArray64(blockPtr, revIndex, numBits);
}

