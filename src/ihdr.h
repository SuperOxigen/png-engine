/*
 *  Image-Formats - PNG IHDR Chunk
 *
 *  Copyright (c) 2018 Alex Dale
 *  See LICENSE for details
 */
#ifndef _IHDR_H_
#define _IHDR_H_

#include "base.h"
#include "chunk.h"

/* Numeric value of "IHDR" in ASCII. */
#define IHDR_TYPE  0x49484452

typedef struct {
    /*
     * Width and height are in pixels.
     * Must not be 0 and must be less than 2^31-1.
     */
    uint32_t width;
    uint32_t height;
    /* Number of bits per sample. */
    uint8_t bit_depth;
    /* Bit mask of color types. */
    uint8_t color_type;
    uint8_t compression_method;
    uint8_t filter_method;
    uint8_t interlace_method;
} ihdr_t;

/*
 * Function: chunk_is_ihdr
 *  Determines if the provide chunk is of type Image Header.  Does not
 *  determine if the contained IHDR is valid.
 * Args:
 *    chunk - Pointer to chunk.
 * Return:
 *    `true` if the chunk is type is IHDR and the length of the chunk
 *    data is large enough to fit all of the IHDR fields.
 */
bool_t chunk_is_ihdr(chunk_t const *chunk);

/*
 * Function: ihdr_is_valid
 *  Determines if the provide IHDR contains valid values.
 * Args:
 *    ihdr - Pointer to IHDR struct.
 * Return:
 *    `true` if the IHDR values are valid.
 */
bool_t ihdr_is_valid(ihdr_t const *ihdr);

/*
 * Function: ihdr_serialize
 *  Serializes an IHDR struct into its chunk data form.
 * Args:
 *    ihdr - Pointer to a valid IHDR struct.
 *    outbuf - Buffer to serialize data to
 *    outlen - On input, it should point to the length of the `outbuf`.
 *             On output, it will contain the number of bytes used
 *             if serialization was successful or the number of bytes
 *             expected if buffer was not large enough.
 * Return:
 *    OK if IHDR was successfully serialized.
 *    NULL_ARG if any of the arguments are NULL.
 *    FAILURE if the data could not fit into the provided buffer.
 */
status_t ihdr_serialize(ihdr_t const *ihdr, uint8_t *outbuf, uint32_t *outlen);

/*
 * Function: ihdr_deserialize
 *  Deserializes an IHDR struct from the provided buffer.
 * Note:
 *  This function does not check that the deserialized values are
 *  valid.
 * Args:
 *    inbuf - Buffer containing serialized date
 *    outlen - On input, it should point to the length of the `inbuf`.
 *             On output, it will contain the number of bytes used
 *             if serialization was successful or the number of bytes
 *             expected if buffer was not large enough.
 *    ihdr - Pointer to an uninitialized IHDR struct.
 * Return:
 *    OK if IHDR was successfully deserialized.
 *    NULL_ARG if any of the arguments are NULL.
 *    FAILURE if the provided buffer is not large enough to have
 *      contained an IHDR.
 */
status_t ihdr_deserialize(uint8_t const *inbuf, uint32_t *inlen, ihdr_t *ihdr);

/*
 * Function: chunk_new_ihdr
 *  Initializes a chunk struct containing an IHDR as its data.
 * Args:
 *    ihdr - Pointer to an IHDR struct.
 *    chunk - Pointer to an uninitialized chunk struct.
 * Return:
 *    OK if IHDR was successfully serialized into the chunk.
 *    NULL_ARG if any of the arguments are NULL.
 */
status_t chunk_new_ihdr(ihdr_t const *ihdr, chunk_t *chunk);

/*
 * Function: ihdr_from_chunk
 *  Initializes an IHDR struct from a chunk containing an IHDR
 *  payload.
 * Note:
 *  This function does not check that the deserialized values are
 *  valid.
 * Args:
 *    chunk - Pointer to a chunk containing an IHDR payload.
 *    ihdr - Pointer to an IHDR struct.
 * Return:
 *    OK if IHDR was successfully deserialized from the chunk.
 *    NULL_ARG if any of the arguments are NULL.
 */
status_t ihdr_from_chunk(chunk_t const *chunk, ihdr_t *ihdr);

/*
 * Function: ihdr_get_sample_depth
 *  Determines the sample depth specified by the IHDR header.  This
 *  information is derived from the color type and bit depth.
 * Args:
 *    ihdr - Pointer to an initialized IHDR struct.
 * Return:
 *    OK if IHDR was successfully deserialized from the chunk.
 *    NULL_ARG if any of the arguments are NULL.
 *    ILLEGAL_ARG if the color type is unknown.
 */
status_t ihdr_get_sample_depth(ihdr_t const *ihdr, uint32_t *sample_depth);

/* Color type */
bool_t ihdr_color_type_is_greyscale(uint8_t color_type);
bool_t ihdr_color_type_is_palette(uint8_t color_type);
bool_t ihdr_color_type_is_realcolor(uint8_t color_type);
bool_t ihdr_color_type_is_alpha_channel(uint8_t color_type);


#endif /* _IHDR_H_ */
