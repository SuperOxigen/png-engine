/*
 *  Image-Formats - Engine
 *
 *  Copyright (c) 2018 Alex Dale
 *  See LICENSE for details
 */
#include <stdio.h>
#include <stdlib.h>

#include "engine.h"

void engine_die(char const *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}
