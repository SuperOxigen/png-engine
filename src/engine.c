/*
 *  Image-Formats - Engine
 *
 *  Copyright (c) 2018 Alex Dale
 *  See LICENSE for details
 */
#include <stdio.h>
#include <stdlib.h>

#include "engine.h"

void engine_die(char_t const *msg)
{
    fprintf(stderr, "%s\n", msg ? msg : "Dead");
    exit(EXIT_FAILURE);
}

void *engine_allocate(size_t bytes)
{
    if (bytes > kMallocLimit)
    {
        return NULL;
    }
    return malloc(bytes);
}
