/*
 *  Image-Formats - PNG PLTE Chunk
 *
 *  Copyright (c) 2018 Alex Dale
 *  See LICENSE for details
 */

#include <arpa/inet.h>  /* htonl / ntohl */
#include <stdlib.h>
#include <string.h>

#include "engine.h"
#include "plte.h"

/*
 * PLTE specific constants.  Defined in RFC2083 Section 4.1.2.
 */

static uint32_t const kPlteType = PLTE_TYPE;

/*
 * Serialized PLTE data must be byte alligned by 3
 * (red-blue-green 8-bits/each).
 */
static uint32_t const kByteAlignment = 3;

/* Color byte offset */
static index_t const kRedIndex = 0;
static index_t const kGreenIndex = 1;
static index_t const kBlueIndex = 2;

static bool_t length_is_byte_aligned(uint32_t length)
{
    return (length % 3) == 0;
}

bool_t chunk_is_palette(chunk_t const *chunk)
{
    return (chunk && chunk->type == kPlteType &&
            length_is_byte_aligned(chunk->length));
}

bool_t palette_is_valid(palette_t const *palette)
{
    return palette && (palette->colors || palette->size == 0);
}

status_t palette_new(rgb_t const *colors, uint8_t size, palette_t *palette);

status_t palette_create(rgb_t *colors, uint8_t size, palette_t *palette);

status_t palette_free(palette_t *palette);

status_t palette_clear(palette_t *palette);

status_t palette_serialize(palette_t const *palette, uint8_t *outbuf, uint32_t *outlen)
{
    uint32_t palette_byte_size;
    uint32_t i;
    index_t idx;

    if (!palette || !outbuf || !outlen)
    {
        return STATUS_NULL_ARGUMENT;
    }

    if (palette->size == 0)
    {
        *outlen = 0;
        return STATUS_OK;
    }

    if (!palette->colors)
    {
        /* Data is missing. */
        return STATUS_ILLEGAL_ARGUMENT;
    }

    palette_byte_size = palette->size * kByteAlignment;

    if (*outlen < palette_byte_size)
    {
        *outlen = palette_byte_size;
        return STATUS_FAILURE;
    }
    *outlen = palette_byte_size;

    for (i = 0; i < palette->size; i++)
    {
        idx = i * kByteAlignment;
        outbuf[idx + kRedIndex] = palette->colors[i].red;
        outbuf[idx + kGreenIndex] = palette->colors[i].green;
        outbuf[idx + kBlueIndex] = palette->colors[i].blue;
    }

    return STATUS_OK;
}

status_t palette_deserialize(uint8_t const *inbuf, uint32_t *inlen, palette_t *palette)
{
    uint32_t i;
    index_t idx;

    if (!inbuf || !inlen || !palette)
    {
        return STATUS_NULL_ARGUMENT;
    }

    if (!length_is_byte_aligned(*inlen))
    {
        return STATUS_INCOMPLETE_PACKET;
    }

    palette->size = *inlen / 3;

    if (palette->size == 0)
    {
        palette->colors = NULL;
        return STATUS_OK;
    }

    palette->colors = (rgb_t*)malloc(sizeof(rgb_t) * palette->size);
    if (!palette->colors)
    {
        return STATUS_OUT_OF_MEMORY;
    }
    memset(palette->colors, 0, sizeof(rgb_t) * palette->size);

    for (i = 0; i < palette->size; i++)
    {
        idx = i * kByteAlignment;
        palette->colors[i].red = inbuf[idx + kRedIndex];
        palette->colors[i].green = inbuf[idx + kGreenIndex];
        palette->colors[i].blue = inbuf[idx + kBlueIndex];
    }

    return STATUS_OK;
}

status_t chunk_new_palette(palette_t const *palette, chunk_t *chunk)
{
    uint8_t *palette_data;
    uint32_t palette_length;
    status_t status;

    if (!palette || !chunk)
    {
        return STATUS_NULL_ARGUMENT;
    }

    if (!palette_is_valid(palette))
    {
        return STATUS_ILLEGAL_ARGUMENT;
    }

    palette_length = palette->size * kByteAlignment;
    palette_data = (uint8_t *)engine_allocate(palette_length);
    if (!palette_data)
    {
        return STATUS_OUT_OF_MEMORY;
    }

    status = palette_serialize(palette, palette_data, &palette_length);
    if (status != STATUS_OK)
    {
        if (palette_data)
        {
            free(palette_data);
        }
        return status;
    }

    status = chunk_create(kPlteType, palette_data, palette_length, chunk);

    if (status != STATUS_OK && palette_data)
    {
        free(palette_data);
    }
    return status;
}

status_t palette_from_chunk(chunk_t const *chunk, palette_t *palette);

status_t palette_get_color(palette_t const *palette, uint8_t index, rgb_t *color);
