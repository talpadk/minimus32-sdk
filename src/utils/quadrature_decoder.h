#ifndef _QUADRATURE_DECODER_H_
#define _QUADRATURE_DECODER_H_

#include "io_function.h"

typedef void(*quadrature_callback)(unsigned char up);

typedef struct {
  io_inFunction aInput;
  io_inFunction bInput;
  quadrature_callback callback;
  unsigned char lastQuad;
} QuadratureDecoder;

void quadrature_decoder_init(QuadratureDecoder *decoder);
void quadrature_decoder_handleEvent(QuadratureDecoder *decoder);

#endif /* _QUADRATURE_DECODER_H_ */
