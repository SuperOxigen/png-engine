/*
 *  Image-Formats - Engine
 *      Provides API for the programs execution logic.
 *
 *  Copyright (c) 2018 Alex Dale
 *  See LICENSE for details
 */
#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "base.h"

void engine_die(char_t const * msg);

void *engine_allocate(size_t bytes);

#endif /* _ENGINE_H_ */
