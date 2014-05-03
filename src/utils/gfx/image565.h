#ifndef IMAGE565_H
#define IMAGE565_H

#include <stdint.h>

typedef struct {
  const uint16_t width;
  const uint16_t height;
  const uint8_t *bytes;
} image565;

#endif /* IMAGE565_H */
