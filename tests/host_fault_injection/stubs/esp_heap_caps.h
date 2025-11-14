#pragma once

#include <stddef.h>
#include <stdint.h>

#define MALLOC_CAP_SPIRAM   (1u << 0)
#define MALLOC_CAP_DMA      (1u << 1)
#define MALLOC_CAP_INTERNAL (1u << 2)

void *heap_caps_malloc(size_t size, uint32_t caps);
void heap_caps_free(void *ptr);
