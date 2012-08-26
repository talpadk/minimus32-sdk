#ifndef MOD_VERTICAL_BYTE_FONT_H
#define MOD_VERTICAL_BYTE_FONT_H

#include <stdint.h>

typedef struct {
  const uint8_t width;
  const uint8_t height;
  const uint8_t start;
  const uint8_t end;
  const uint8_t *bytes;
} vertical_byte_font;

#endif //MOD_VERTICAL_BYTE_FONT_H
