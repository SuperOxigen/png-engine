/*
 *  Image-Formats - PNG Color Chunks
 *
 *  Copyright (c) 2018 Alex Dale
 *  See LICENSE for details
 */
#ifndef _IMGCHUNK_H_
#define _IMGCHUNK_H_

#include "base.h"
#include "chunk.h"

/* Numeric value of "PLTE" in ASCII. */
#define PLTE_TYPE 0x504c5445u

/* This might get moved to a different file. */
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t _;
} rgb_t;

typedef struct {
    /* Number of palette entries. */
    uint8_t size;
    /* Dynamically allocated list of entries. */
    rgb_t *colors;
} palette_t;


/*
 * Function: chunk_is_palette
 *  Determines if the provide chunk is of type PLTE.
 * Args:
 *    chunk - Pointer to chunk.
 * Return:
 *    `true` if the chunk is type is PLTE and the length of the chunk
 *    data is large enough to fit all of the PLTE fields.
 */
bool_t chunk_is_palette(chunk_t const *chunk);

/*
 * Function: palette_is_valid
 *  Determines if the provide PLTE contains valid values.
 * Args:
 *    palette - Pointer to PLTE struct.
 * Return:
 *    `true` if the PLTE values are valid.
 */
bool_t palette_is_valid(palette_t const *palette);

/*
 * Function: palette_new
 *  Creates a new palette struct, copying the provide set of colors
 *  to a heap allocated copy.
 * Args:
 *    colors - Array of colors to be copied into the palette.  Can be
 *             NULL if `size` is 0.
 *    size - Number of `color_t` elements in `colors`
 *    palette - Pointer to an uninitialized palette struct.
 * Return:
 *    OK if palette was created successfully
 *    NULL_ARG if `palette` NULL or (`colors` is NULL and `size` is not 0).
 *    OUT_OF_MEM if allocation of `colors` fails.
 */
status_t palette_new(rgb_t const *colors, uint8_t size, palette_t *palette);

/*
 * Function: palette_new
 *  Creates a new palette struct, directly assigning the list of colors.
 * Args:
 *    colors - Array of colors to be giveen to the palette.  Can be
 *             NULL if `size` is 0.
 *    size - Number of `color_t` elements in `colors`
 *    palette - Pointer to an uninitialized palette struct.
 * Return:
 *    OK if palette was created successfully
 *    NULL_ARG if `palette` NULL or (`colors` is NULL and `size` is not 0).
 */
status_t palette_create(rgb_t *colors, uint8_t size, palette_t *palette);

/*
 * Function: palette_free
 *  Frees the resouces of an initialized palette and clears it.
 * Args:
 *    palette - Palette to be freed.
 * Return:
 *    OK on successful reset.
 *    NULL_ARG if any of the input variables are null.
 */
status_t palette_free(palette_t *palette);

/*
 * Function: palette_clear
 *  Zeros the provided palette.  Intended to be used on a stack allocated
 *  chunk struct.
 * Args:
 *    palette - Palette to be cleared.
 * Return:
 *    OK on successful reset.
 *    NULL_ARG if any of the input variables are null.
 */
status_t palette_clear(palette_t *palette);

/*
 * Function: palette_serialize
 *  Serializes an PLTE struct into its chunk data form.
 * Args:
 *    palette - Pointer to a valid PLTE struct.
 *    outbuf - Buffer to serialize data to
 *    outlen - On input, it should point to the length of the `outbuf`.
 *             On output, it will contain the number of bytes used
 *             if serialization was successful or the number of bytes
 *             expected if buffer was not large enough.
 * Return:
 *    OK if PLTE was successfully serialized.
 *    NULL_ARG if any of the arguments are NULL.
 *    FAILURE if the data could not fit into the provided buffer.
 */
status_t palette_serialize(
    palette_t const *palette, uint8_t *outbuf, uint32_t *outlen);

/*
 * Function: palette_deserialize
 *  Deserializes an PLTE struct from the provided buffer.
 * Args:
 *    inbuf - Buffer containing serialized date
 *    outlen - On input, it should point to the length of the `inbuf`.
 *             On output, it will contain the number of bytes used
 *             if serialization was successful or the number of bytes
 *             expected if buffer was not large enough.
 *    palette - Pointer to an uninitialized PLTE struct.
 * Return:
 *    OK if PLTE was successfully deserialized.
 *    NULL_ARG if any of the arguments are NULL.
 *    FAILURE if the provided buffer is not large enough to have
 *      contained an PLTE.
 */
status_t palette_deserialize(
    uint8_t const *inbuf, uint32_t *inlen, palette_t *palette);

/*
 * Function: chunk_new_palette
 *  Initializes a chunk struct containing an PLTE as its data.
 * Args:
 *    palette - Pointer to an PLTE struct.
 *    chunk - Pointer to an uninitialized chunk struct.
 * Return:
 *    OK if PLTE was successfully serialized into the chunk.
 *    NULL_ARG if any of the arguments are NULL.
 */
status_t chunk_new_palette(palette_t const *palette, chunk_t *chunk);

/*
 * Function: palette_from_chunk
 *  Initializes an PLTE struct from a chunk containing an PLTE
 *  payload.
 * Args:
 *    chunk - Pointer to a chunk containing an PLTE payload.
 *    palette - Pointer to an PLTE struct.
 * Return:
 *    OK if PLTE was successfully deserialized from the chunk.
 *    NULL_ARG if any of the arguments are NULL.
 */
status_t palette_from_chunk(chunk_t const *chunk, palette_t *palette);

/*
 * Function: palette_get_color
 *  Gets the RGB value stored at the specified index.  Will protect
 *  from overflows.
 * Args:
 *    palette - Pointer to an initialized PLTE struct.
 *    index - Palette color index
 *    color - Will store the result RGB.
 * Return:
 *    OK if RGB values was available in the palette.
 *    NULL_ARG if any of the arguments are NULL.
 *    FAILURE if the index is out of bounds.
 */
status_t palette_get_color(
    palette_t const *palette, uint8_t index, rgb_t *color);


#endif /* _IMGCHUNK_H_ */
