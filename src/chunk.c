/*
 *  Image-Formats - PNG Image Chunk
 *
 *  Copyright (c) 2018 Alex Dale
 *  See LICENSE for details
 */
#include <arpa/inet.h>  /* htonl / ntohl */
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "engine.h"

#include "chunk.h"

static size_t const kChunkTypeSize = sizeof(uint32_t);

static uint32_t const kMaxLength = kSigned32Max;
static uint32_t const kU32Mask = 0xffffffffu;

static uint32_t const kCrcPolynomial = 0xedb88320u;

/* The following bit masks are to be used on host-order type fields. */
static uint32_t const kAncillaryBitMask = 0x20000000u;
static uint32_t const kPrivateBitMask = 0x00200000u;
static uint32_t const kReservedBitMask = 0x00002000u;
static uint32_t const kSafeToCopyBitMask = 0x00000020u;

status_t chunk_new(
    uint32_t type, uint8_t const *data, uint32_t length, chunk_t *chunk)
{
    status_t status;
    uint8_t *chunk_data;
    if (!chunk || (!data && length != 0))
    {
        return STATUS_NULL_ARGUMENT;
    }
    if (length > kMaxLength || !chunk_type_is_valid(type))
    {
        return STATUS_ILLEGAL_ARGUMENT;
    }

    if (length == 0)
    {
        chunk_data = NULL;
    }
    else
    {
        chunk_data = (uint8_t *)engine_allocate(length);
        if (!chunk_data)
        {
            return STATUS_OUT_OF_MEMORY;
        }
        memcpy(chunk_data, data, length);
    }
    status = chunk_create(type, chunk_data, length, chunk);
    if (status != STATUS_OK && chunk_data)
    {
        free(chunk_data);
    }
    return status;
}

status_t chunk_create(
    uint32_t type, uint8_t *data, uint32_t length, chunk_t *chunk)
{
    if (!chunk || (!data && length != 0))
    {
        return STATUS_NULL_ARGUMENT;
    }

    if (length > kMaxLength || !chunk_type_is_valid(type))
    {
        return STATUS_ILLEGAL_ARGUMENT;
    }
    chunk->type = type;
    chunk->length = length;
    chunk->data = data;
    return STATUS_OK;
}

status_t chunk_serialize(chunk_t const *chunk, uint8_t *outbuf, size_t *outlen)
{
    uint32_t serlength, nvalue, crc;
    uint8_t *optr;
    status_t status;

    /* Validate the input arguments. */
    if (!chunk || !outbuf || !outlen)
    {
        return STATUS_NULL_ARGUMENT;
    }

    /* If length is non-zero, then data must be present. */
    if (chunk->length > 0 && !chunk->data)
    {
        return STATUS_ILLEGAL_ARGUMENT;
    }

    /*
     * Serialized length = Data length size + Type size
     *                   + Length of data + CRC size
     */
    serlength = chunk->length + (sizeof(uint32_t) * 3);
    if (*outlen < serlength)
    {
        *outlen = serlength;
        return STATUS_FAILURE;
    }
    *outlen = serlength;

    status = chunk_calculate_crc(chunk, &crc);
    if (status != STATUS_OK)
    {
        return status;
    }

    /*
     * Serialization stage.
     *  Length | Type | Data | CRC
     */
    optr = outbuf;

    nvalue = htonl(chunk->length);
    memcpy(optr, &nvalue, sizeof(uint32_t));
    optr += sizeof(uint32_t);

    nvalue = htonl(chunk->type);
    memcpy(optr, &nvalue, sizeof(uint32_t));
    optr += sizeof(uint32_t);

    memcpy(optr, chunk->data, chunk->length);
    optr += chunk->length;

    nvalue = htonl(crc);
    memcpy(optr, &crc, sizeof(uint32_t));

    return STATUS_OK;
}

status_t chunk_deserialize(uint8_t const *inbuf, size_t *inlen, chunk_t *chunk)
{
    uint32_t nvalue, crc, calc_crc;
    uint8_t const *iptr;
    status_t status;
    if (!chunk || !inbuf || !inlen)
    {
        return STATUS_NULL_ARGUMENT;
    }

    if (chunk->type || chunk->length || chunk->data)
    {
        return STATUS_ILLEGAL_ARGUMENT;
    }

    /* Length field */
    iptr = inbuf;
    memcpy(&nvalue, iptr, sizeof(uint32_t));
    chunk->length = ntohl(nvalue);
    iptr += sizeof(uint32_t);

    if (chunk->length > kMaxLength)
    {
        return STATUS_BAD_PACKET;
    }

    /* Check that the provided buffer contains all of the data.  */
    if (*inlen < (chunk->length + (sizeof(uint32_t) * 3)))
    {
        return STATUS_INCOMPLETE_PACKET;
    }
    *inlen = (chunk->length + (sizeof(uint32_t) * 3));

    /* Type field */
    memcpy(&nvalue, iptr, sizeof(uint32_t));
    chunk->type = ntohl(nvalue);
    iptr += sizeof(uint32_t);

    /* Data */
    /* Check that the data file is within allocation limit. */
    chunk->data = (uint8_t *)engine_allocate(chunk->length);
    if (!chunk->data)
    {
        return STATUS_OUT_OF_MEMORY;
    }
    memcpy(chunk->data, iptr, chunk->length);
    iptr += chunk->length;

    /* CRC */
    memcpy(&nvalue, iptr, sizeof(uint32_t));
    calc_crc = ntohl(nvalue);
    status = chunk_calculate_crc(chunk, &crc);
    if (status != STATUS_OK)
    {
        return status;
    }
    /* Check is CRC is correct. */
    if (crc != calc_crc)
    {
        return STATUS_BAD_CRC;
    }

    return STATUS_OK;
}

static void crc_table_gen(uint32_t *table)
{
    uint32_t coef, idx, it;
    for (idx = 0; idx < 256; idx++)
    {
        coef = idx;
        for (it = 0; it < 8; it++)
        {
            if (it & 1) coef = kCrcPolynomial ^ (coef >> 1);
            else coef >>= 1;
        }
        table[idx] = coef;
    }
}

static uint32_t crc_update(uint32_t crc, void const *buf, uint32_t len)
{
    static uint32_t crc_table[256] = {0};
    static bool_t has_table = false;
    uint8_t const *bytebuf;
    uint32_t i;
    if (!has_table)
    {
        crc_table_gen(crc_table);
        has_table = true;
    }
    bytebuf = buf;
    for (i = 0; i < len; i++)
    {
        crc = crc_table[(crc ^ bytebuf[i]) & 0xff] ^ (crc >> 8);
    }
    return crc;
}

status_t chunk_calculate_crc(chunk_t const *chunk, uint32_t *crc_out)
{
    uint32_t nvalue, crc;

    if (!chunk || !crc_out)
    {
        return STATUS_NULL_ARGUMENT;
    }

    if (chunk->length > 0 && !chunk->data)
    {
        return STATUS_ILLEGAL_ARGUMENT;
    }

    crc = kU32Mask;
    crc = crc_update(crc, chunk->data, chunk->length);
    nvalue = htonl(chunk->type);
    crc = crc_update(crc, &nvalue, sizeof(nvalue));
    *crc_out = crc;

    return STATUS_OK;
}

status_t chunk_clear(chunk_t *chunk)
{
    if (!chunk)
    {
        return STATUS_NULL_ARGUMENT;
    }
    memset(chunk, 0, sizeof(chunk_t));
    return STATUS_OK;
}

status_t chunk_free(chunk_t *chunk)
{
    if (!chunk)
    {
        return STATUS_NULL_ARGUMENT;
    }

    if (chunk->length > 0 && chunk->data)
    {
        memset(chunk->data, 0, chunk->length);
    }

    if (chunk->data)
    {
        free(chunk->data);
    }

    return chunk_clear(chunk);
}

status_t chunk_swap(chunk_t *chunk_a, chunk_t *chunk_b)
{
    uint32_t value;
    uint8_t *data;
    if (!chunk_a || !chunk_b)
    {
        return STATUS_NULL_ARGUMENT;
    }

    value = chunk_a->length;
    chunk_a->length = chunk_b->length;
    chunk_b->length = value;

    value = chunk_a->type;
    chunk_a->type = chunk_b->type;
    chunk_b->type = value;

    data = chunk_a->data;
    chunk_a->data = chunk_b->data;
    chunk_b->data = data;

    return STATUS_OK;
}

bool_t chunk_type_to_string(uint32_t type, char_t *out, size_t outlen)
{
    uint32_t ntype;
    char_t const *ntptr;

    if (!out || outlen <= kChunkTypeSize || !chunk_type_is_valid(type))
        return false;

    ntype = htonl(type);
    ntptr = (char_t *) &ntype;
    memcpy(out, ntptr, kChunkTypeSize);
    memset(&out[kChunkTypeSize], 0, outlen - kChunkTypeSize);

    return true;
}

bool_t chunk_type_is_valid(uint32_t type)
{
    uint32_t i;
    uint8_t *ptr;
    /* Check that all type characters are ASCII A-Z or a-z (isalpha()) */
    ptr = (uint8_t *) &type;
    for (i = 0; i < kChunkTypeSize; i++)
    {
        if (!(65 <= *ptr && *ptr <= 90) && !(97 <= *ptr && *ptr <= 122))
            return false;
    }
    return true;
}

bool_t chunk_type_is_critical(uint32_t type)
{
    return (type & kAncillaryBitMask) == 0;
}

bool_t chunk_type_is_private(uint32_t type)
{
    return (type & kPrivateBitMask) != 0;
}

bool_t chunk_type_is_reserved(uint32_t type)
{
    return (type & kReservedBitMask) != 0;
}

bool_t chunk_type_is_safe_to_copy(uint32_t type)
{
    return (type & kSafeToCopyBitMask) != 0;
}
