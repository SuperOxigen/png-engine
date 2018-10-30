/*
 *  Image-Formats - PNG Color Chunks
 *
 *  Copyright (c) 2018 Alex Dale
 *  See LICENSE for details
 */

#include <arpa/inet.h>  /* htonl / ntohl */
#include <stdlib.h>
#include <string.h>

#include "engine.h"
#include "clrchunk.h"

/*
 * PLTE specific constants.  Defined in RFC2083 Section 4.1.2.
 */

static uint32_t const kPlteType = PLTE_TYPE;

/*
 * Serialized PLTE data must be byte alligned by 3
 * (red-blue-green 8-bits/each).
 */
static uint32_t const kPaletteByteAlignment = 3;
static uint32_t const kMaxPaletteColors = 255;

/* Color byte offset */
static index_t const kRedIndex = 0;
static index_t const kGreenIndex = 1;
static index_t const kBlueIndex = 2;

static bool_t data_length_is_valid(uint32_t length)
{
    return (length % 3) == 0 && (length / 3) <= kMaxPaletteColors;
}

bool_t chunk_is_palette(chunk_t const *chunk)
{
    return (chunk && chunk->type == kPlteType &&
            data_length_is_valid(chunk->length));
}

bool_t palette_is_valid(palette_t const *palette)
{
    return palette && (palette->colors || palette->size == 0);
}

status_t palette_new(rgb_t const *colors, uint8_t size, palette_t *palette)
{
    status_t status;
    rgb_t *palette_colors;

    if (!palette || (!colors && size != 0))
    {
        return STATUS_NULL_ARGUMENT;
    }

    if (size == 0)
    {
        palette_colors = NULL;
    }
    else
    {
        palette_colors = (rgb_t *)engine_allocate(sizeof(rgb_t) * size);
        if (!palette_colors)
        {
            return STATUS_OUT_OF_MEMORY;
        }
        memcpy(palette_colors, colors, sizeof(rgb_t) * size);
    }
    status = palette_create(palette_colors, size, palette);
    if (status != STATUS_OK && palette_colors)
    {
        free(palette_colors);
    }
    return status;
}

status_t palette_create(rgb_t *colors, uint8_t size, palette_t *palette)
{
    if (!palette || (!colors && size != 0))
    {
        return STATUS_NULL_ARGUMENT;
    }
    palette->size = size;
    palette->colors = colors;
    return STATUS_OK;
}

status_t palette_free(palette_t *palette)
{
    if (!palette)
    {
        return STATUS_NULL_ARGUMENT;
    }

    if (palette->colors && palette->size > 0)
    {
        memset(palette->colors, 0, sizeof(rgb_t) * palette->size);
    }

    if (palette->colors)
    {
        free(palette->colors);
    }

    return palette_clear(palette);
}

status_t palette_clear(palette_t *palette)
{
    if (!palette)
    {
        return STATUS_NULL_ARGUMENT;
    }
    memset(palette, 0, sizeof(palette_t));
    return STATUS_OK;
}

status_t palette_serialize(
    palette_t const *palette, uint8_t *outbuf, uint32_t *outlen)
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

    palette_byte_size = palette->size * kPaletteByteAlignment;

    if (*outlen < palette_byte_size)
    {
        *outlen = palette_byte_size;
        return STATUS_FAILURE;
    }
    *outlen = palette_byte_size;

    for (i = 0; i < palette->size; i++)
    {
        idx = i * kPaletteByteAlignment;
        outbuf[idx + kRedIndex] = palette->colors[i].red;
        outbuf[idx + kGreenIndex] = palette->colors[i].green;
        outbuf[idx + kBlueIndex] = palette->colors[i].blue;
    }

    return STATUS_OK;
}

status_t palette_deserialize(
    uint8_t const *inbuf, uint32_t *inlen, palette_t *palette)
{
    uint32_t i;
    index_t idx;

    if (!inbuf || !inlen || !palette)
    {
        return STATUS_NULL_ARGUMENT;
    }

    if (!data_length_is_valid(*inlen))
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
        idx = i * kPaletteByteAlignment;
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

    palette_length = palette->size * kPaletteByteAlignment;
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

status_t palette_from_chunk(chunk_t const *chunk, palette_t *palette)
{
    uint32_t palette_length;
    if (!palette || !chunk)
    {
        return STATUS_NULL_ARGUMENT;
    }

    if (!chunk_is_palette(chunk))
    {
        return STATUS_ILLEGAL_ARGUMENT;
    }

    palette_length = chunk->length;
    return palette_deserialize(chunk->data, &palette_length, palette);
}

status_t palette_get_color(
    palette_t const *palette, uint8_t index, rgb_t *color)
{
    rgb_t const *palette_color;
    if (!palette || !color)
    {
        return STATUS_NULL_ARGUMENT;
    }

    if (!palette_is_valid(palette))
    {
        return STATUS_ILLEGAL_ARGUMENT;
    }

    if (index >= palette->size)
    {
        return STATUS_FAILURE;
    }

    palette_color = &palette->colors[index];
    color->red = palette_color->red;
    color->green = palette_color->green;
    color->blue = palette_color->blue;
    return STATUS_OK;
}
