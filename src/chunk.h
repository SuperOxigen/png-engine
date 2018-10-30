/*
 *  Image-Formats - PNG Image Chunk
 *
 *  Copyright (c) 2018 Alex Dale
 *  See LICENSE for details
 */
#ifndef _CHUNK_H_
#define _CHUNK_H_

#include "base.h"

typedef struct {
    /* Length of `data` only. */
    uint32_t length;
    /* Chunk type */
    uint32_t type;
    /* Data of chunk.  Data is allocated using malloc() */
    uint8_t *data;
} chunk_t;

/*
 * Function: chunk_new
 *  Creates a new PNG chunk using provided parameters.
 * Note:
 *  This allocates a copy of `data` on the heap.
 * Args:
 *    type - Numeric chunk type
 *    data - Chunk data to be copied to chunk.
 *    length - Length of the data
 *    chunk - Pointer to an uninitialized chunk
 * Return:
 *    OK on successful creation.
 */
status_t chunk_new(
    uint32_t type, uint8_t const *data, uint32_t length, chunk_t *chunk);

/*
 * Function: chunk_create
 *  Creates a new PNG chunk using provided parameters.
 *  Does not allocate any memory on the stack.
 * Args:
 *    type - Numeric chunk type
 *    data - Chunk data.
 *    length - Length of the data
 *    chunk - Pointer to an uninitialized chunk
 * Return:
 *    OK on successful creation.
 */
status_t chunk_create(
    uint32_t type, uint8_t *data, uint32_t length, chunk_t *chunk);

/*
 * Function: chunk_serialize
 *  Serializes the chunk data into the output buffer.
 *  If the output buffer is not large enough, then function will
 *  return FAILURE and the outlen will contain the size needed to
 *  fully serialize the chunk.
 * Args:
 *    chunk - Source data for serialization
 *    outbuf - Destination buffer for serialized chunk data
 *    outlen - As input, it represents the size of `outbuf`.  As
 *             output, the value is the total number of bytes that
 *             were written.
 *  Return:
 *    OK on successful serialization.  NULL_ARG if any of the input
 *    variables are null.
 */
status_t chunk_serialize(
    chunk_t const *chunk, uint8_t *outbuf, size_t *outlen);

/*
 * Function: chunk_deserialize
 *  Deserializes a chunk from an input buffer.
 * Args:
 *    inbuf - Source buffer of serialized chunk data
 *    inlen - As input, it represents the size of `inbuf`.  As output,
 *            the value is the total number of bytes that were used to
 *            deserialize the chunk.
 *    chunk - Chunk struct to be initialized.
 * Return:
 *    OK on successful deserialization.  NULL_ARG if any of the input
 *    variables are null.
 */
status_t chunk_deserialize(
    uint8_t const *inbuf, size_t *inlen, chunk_t *chunk);

/*
 * Function: chunk_calculate_crc
 *  Calculates the CRC of a PNG chunk using ISO 3309 algorithm.
 * Args:
 *    chunk - Chunk to have CRC calculated on.
 *    crc - Pointer where the calculated CRC will be stored.
 * Return:
 *    OK on successful CRC calculation.  NULL_ARG if any of the input
 *    variables are null.
 */
status_t chunk_calculate_crc(chunk_t const *chunk, uint32_t *crc);

/*
 * Function: chunk_clear
 *  Zeros the provided chunk.  Intended to be used on a stack
 *  allocated chunk struct.
 * Args:
 *    chunk - Chunk to be cleared.
 * Return:
 *    OK on successful clear.  NULL_ARG if any of the input
 *    variables are null.
 */
status_t chunk_clear(chunk_t *chunk);

/*
 * Function: chunk_free
 *  Frees the resouces of an initialized chunk and clears it.
 * Args:
 *    chunk - Chunk to be cleared.
 * Return:
 *    OK on successful reset.  NULL_ARG if any of the input
 *    variables are null.
 */
status_t chunk_free(chunk_t *chunk);

/*
 * Function: chunk_swap
 *  Swaps the contents of two chunks.
 */
status_t chunk_swap(chunk_t *chunk_a, chunk_t *chunk_b);

/*
 * Function: chunk_type_to_string
 *  Turns a valid chunk type into its string representation.
 *  Provided string buffer must be at least 5 bytes long.
 */
bool_t chunk_type_to_string(uint32_t type, char *out, size_t outlen);

bool_t chunk_type_is_valid(uint32_t type);
bool_t chunk_type_is_critical(uint32_t type);
bool_t chunk_type_is_private(uint32_t type);
bool_t chunk_type_is_reserved(uint32_t type);
bool_t chunk_type_is_safe_to_copy(uint32_t type);

#endif /* _CHUNK_H_ */
