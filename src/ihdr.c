/*
 *  Image-Formats - PNG IHDR Chunk
 *
 *  Copyright (c) 2018 Alex Dale
 *  See LICENSE for details
 */
#include <arpa/inet.h>  /* htonl / ntohl */
#include <stdlib.h>
#include <string.h>

#include "engine.h"

#include "ihdr.h"

/*
 * IHDR specific constants.  Defined in RFC2083 Section 4.1.1.
 */

static uint32_t const kIhdrType = IHDR_TYPE;

/* Size of serialized IHDR byte size. */
static uint32_t const kIhdrSize =
 (sizeof(uint32_t) * 2) + (sizeof(uint8_t) * 5);

/* Image height and width maximums */
static uint32_t const kMaxWidth = kSigned32Max;
static uint32_t const kMaxHeight = kSigned32Max;

/* Color types bits */
static uint8_t const kPaletteBitMask = 0x01;
static uint8_t const kRealcolorBitMask = 0x02;
static uint8_t const kAlphaChannelBitMask = 0x04;

/* Valid color codes */
static uint8_t const kGrayscaleColorType = 0x00;
static uint8_t const kRealcolorColorType = kRealcolorBitMask;
static uint8_t const kPaletteIndexColorType =
    kPaletteBitMask | kRealcolorBitMask;
static uint8_t const kGrayscaleAlphaColorType = kAlphaChannelBitMask;
static uint8_t const kRealcolorAlphaColorType =
    kRealcolorBitMask | kAlphaChannelBitMask;
static uint8_t const kValidColorTypes[] = {
    kGrayscaleColorType,
    kRealcolorColorType,
    kPaletteIndexColorType,
    kGrayscaleAlphaColorType,
    kRealcolorAlphaColorType
};

/* Bit Depth for Color Types */
static uint8_t const kAllowedGrayscaleDepths[] = {1, 2, 4, 8, 16};
static uint8_t const kAllowedRealcolorDepths[] = {8, 16};
static uint8_t const kAllowedPaletteIndexDepths[] = {1, 2, 4, 8};
static uint8_t const kAllowedGrayscaleAlphaDepths[] = {8, 16};
static uint8_t const kAllowedRealcolorAlphaDepths[] = {8, 16};

/* Valid compression methods. */
static uint8_t const kDeflateInflateCompressionMethod = 0;
static uint8_t const kValidCompressionMethods[] = {
    kDeflateInflateCompressionMethod
};

/* Valid filter methods. */
static uint8_t const kAdaptiveFiltering5 = 0;
static uint8_t const kValidFilterMethods[] = {
    kAdaptiveFiltering5
};

/* Valid interlace methods. */
static uint8_t const kNoInterlace = 0;
static uint8_t const kAdam7Interlace = 1;
static uint8_t const kValidInterlaceMethods[] = {
    kNoInterlace,
    kAdam7Interlace
};

/*
 * IDAT specific constants.  Defined in RFC2083 Section 4.1.3.
 */

static uint32_t const kIdatType = IDAT_TYPE;

/*
 * IDAT specific constants.  Defined in RFC2083 Section 4.1.4.
 */

static uint32_t const kIendType = IEND_TYPE;
static uint32_t const kIendSize = 0;  /* IEND chunks must be empty. */


bool_t chunk_is_ihdr(chunk_t const *chunk)
{
    return (chunk && chunk->type == kIhdrType && chunk->length == kIhdrSize);
}

bool_t chunk_is_idat(chunk_t const *chunk)
{
    return (chunk && chunk->type == kIdatType);
}

bool_t chunk_is_iend(chunk_t const *chunk)
{
    return (chunk && chunk->type == kIendType && chunk->length == kIendSize);
}

static bool_t contains_value(
    uint8_t value, uint8_t const *values, uint32_t length)
{
    uint32_t i;
    for (i = 0; i < length; i++)
    {
        if (value == values[i])
        {
            return true;
        }
    }
    return false;
}

#define CONTAINS(value, values) contains_value(value, values, sizeof(values))

bool_t ihdr_is_valid(ihdr_t const *ihdr)
{
    if (!ihdr)
    {
        return false;
    }

    /* Validate width and height */
    if (ihdr->width > kMaxWidth || ihdr->width == 0 ||
        ihdr->height > kMaxHeight || ihdr->height == 0)
    {
        return false;
    }

    /* Validate code type and bit depth. */
    if (ihdr->color_type == kGrayscaleColorType)
    {
        if (!CONTAINS(ihdr->bit_depth, kAllowedGrayscaleDepths))
            return false;
    }
    else if (ihdr->color_type == kRealcolorColorType)
    {
        if (!CONTAINS(ihdr->bit_depth, kAllowedRealcolorDepths))
            return false;
    }
    else if (ihdr->color_type == kPaletteIndexColorType)
    {
        if (!CONTAINS(ihdr->bit_depth, kAllowedPaletteIndexDepths))
            return false;
    }
    else if (ihdr->color_type == kGrayscaleAlphaColorType)
    {
        if (!CONTAINS(ihdr->bit_depth, kAllowedGrayscaleAlphaDepths))
            return false;
    }
    else if (ihdr->color_type == kRealcolorAlphaColorType)
    {
        if (!CONTAINS(ihdr->bit_depth, kAllowedRealcolorAlphaDepths))
            return false;
    }
    else
    {
        return false;
    }

    /* Validate compression method */
    if (!CONTAINS(ihdr->compression_method, kValidCompressionMethods))
    {
        return false;
    }

    /* Validate filter method. */
    if (!CONTAINS(ihdr->filter_method, kValidFilterMethods))
    {
        return false;
    }

    /* Validate filter method. */
    if (!CONTAINS(ihdr->interlace_method, kValidInterlaceMethods))
    {
        return false;
    }

    return true;
}

status_t ihdr_serialize(ihdr_t const *ihdr, uint8_t *outbuf, uint32_t *outlen)
{
    uint32_t nvalue;
    uint8_t *optr;

    if (!ihdr || !outbuf || !outlen)
    {
        return STATUS_NULL_ARGUMENT;
    }

    if (*outlen < kIhdrSize)
    {
        *outlen = kIhdrSize;
        return STATUS_FAILURE;
    }
    *outlen = kIhdrSize;

    optr = outbuf;

    /* Width - 32-bit */
    nvalue = htonl(ihdr->width);
    memcpy(optr, &nvalue, sizeof(uint32_t));
    optr += sizeof(uint32_t);

    /* Height - 32-bit */
    nvalue = htonl(ihdr->height);
    memcpy(optr, &nvalue, sizeof(uint32_t));
    optr += sizeof(uint32_t);

    /* 8-bit values */
    *optr++ = ihdr->bit_depth;
    *optr++ = ihdr->color_type;
    *optr++ = ihdr->compression_method;
    *optr++ = ihdr->filter_method;
    *optr = ihdr->interlace_method;

    return STATUS_OK;
}

status_t ihdr_deserialize(uint8_t const *inbuf, uint32_t *inlen, ihdr_t *ihdr)
{
    uint32_t nvalue;
    uint8_t const *iptr;

    if (!inbuf || !inlen || !ihdr)
    {
        return STATUS_NULL_ARGUMENT;
    }

    if (*inlen < kIhdrSize)
    {
        return STATUS_INCOMPLETE_PACKET;
    }
    *inlen = kIhdrSize;

    iptr = inbuf;

    /* Width - 32-bit */
    memcpy(&nvalue, iptr, sizeof(uint32_t));
    ihdr->width = ntohl(nvalue);
    iptr += sizeof(uint32_t);

    /* Height - 32-bit */
    memcpy(&nvalue, iptr, sizeof(uint32_t));
    ihdr->height = ntohl(nvalue);
    iptr += sizeof(uint32_t);

    /* Bit Depth - 8-bit */
    ihdr->bit_depth = *iptr;
    iptr++;

    /* Color Depth - 8-bit */
    ihdr->color_type = *iptr;
    iptr++;

    /* Compression Method - 8-bit */
    ihdr->compression_method = *iptr;
    iptr++;

    /* Filter Method - 8-bit */
    ihdr->filter_method = *iptr;
    iptr++;

    /* Interlace Method - 8-bit */
    ihdr->interlace_method = *iptr;

    return STATUS_OK;
}

status_t chunk_new_ihdr(ihdr_t const *ihdr, chunk_t *chunk)
{
    uint32_t ihdr_data_len;
    uint8_t ihdr_data[kIhdrSize];
    status_t status;

    if (!ihdr || !chunk)
    {
        return STATUS_NULL_ARGUMENT;
    }

    ihdr_data_len = kIhdrSize;
    status = ihdr_serialize(ihdr, ihdr_data, &ihdr_data_len);
    if (status != STATUS_OK)
    {
        return status;
    }

    return chunk_new(kIhdrType, ihdr_data, kIhdrSize, chunk);

    return STATUS_OK;
}

status_t ihdr_from_chunk(chunk_t const *chunk, ihdr_t *ihdr)
{
    uint32_t ihdr_data_len;
    status_t status;

    if (!ihdr || !chunk)
    {
        return STATUS_NULL_ARGUMENT;
    }

    if (!chunk_is_ihdr(chunk))
    {
        return STATUS_ILLEGAL_ARGUMENT;
    }

    ihdr_data_len = chunk->length;
    status = ihdr_deserialize(chunk->data, &ihdr_data_len, ihdr);
    return status;
}

status_t ihdr_get_sample_depth(ihdr_t const *ihdr, uint32_t *sample_depth)
{
    if (!ihdr || !sample_depth)
    {
        return STATUS_NULL_ARGUMENT;
    }

    if (!CONTAINS(ihdr->color_type, kValidColorTypes))
    {
        return STATUS_ILLEGAL_ARGUMENT;
    }

    if (ihdr->color_type == kPaletteIndexColorType)
    {
        /* Palette sample depth is always 8 bits. */
        *sample_depth = 8;
    }
    else
    {
        *sample_depth = ihdr->bit_depth;
    }

    return STATUS_OK;
}

bool_t ihdr_color_type_is_greyscale(uint8_t color_type)
{
    return (color_type == 0);
}

bool_t ihdr_color_type_is_palette(uint8_t color_type)
{
    return (color_type & kPaletteBitMask);
}

bool_t ihdr_color_type_is_realcolor(uint8_t color_type)
{
    return (color_type & kRealcolorBitMask);
}

bool_t ihdr_color_type_is_alpha_channel(uint8_t color_type)
{
    return (color_type & kAlphaChannelBitMask);
}
