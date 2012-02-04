#ifndef MOD_BIT_FONT_H
#define MOD_BIT_FONT_H

#include <stdint.h>

typedef struct {
  const uint8_t width;
  const uint8_t height;
  const uint8_t char_size;
  const uint8_t start;
  const uint8_t end;
  const uint8_t *bits;
} bitfont;

#endif //MOD_BIT_FONT_H
