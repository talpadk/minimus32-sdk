#ifndef _QUADRATURE_DECODER_H_
#define _QUADRATURE_DECODER_H_

#include "io_function.h"

typedef void(*quadrature_callback)(unsigned char up);

///A structure for representing a quadrature encoder
typedef struct {
  ///The IO function to poll the status of the A signal from the encoder
  io_inFunction aInput;

  ///The IO function to poll the status of the B signal from the encoder
  io_inFunction bInput;

  ///The call back to call when a up or down event occurs. Note: There are 4 up and down events per detent on encoders with detents
  quadrature_callback callback;

  ///Internal state variable
  unsigned char lastQuad;
} QuadratureDecoder;


typedef struct {
  quadrature_callback callback;
  char index;
} QuadratureDecoderDetentFilter;

/** 
 * Initializes the decoder structure prior to use
 * 
 * @param decoder the structure to initialize
 */
void quadrature_decoder_init(QuadratureDecoder *decoder);

/** 
 * A function that should be called every time one of the quadture pins have changed.
 * This function may in turn call the callback funtion for the decoder to notify about up or down events.
 * 
 * @param decoder the decoder which have had its pin change status.
 */
void quadrature_decoder_handleEvent(QuadratureDecoder *decoder);

void quadrature_decoder_detentFilterInit(QuadratureDecoderDetentFilter *filter);
void quadrature_decoder_detentFilterHandleEvent(QuadratureDecoderDetentFilter *filter, unsigned char up);



#endif /* _QUADRATURE_DECODER_H_ */
