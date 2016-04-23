#include "quadrature_decoder.h"

#include <util/atomic.h>


void quadrature_decoder_init(QuadratureDecoder *decoder){
  decoder->lastQuad = 0;
  if (decoder->aInput()){
    decoder->lastQuad += 1;
  }
  if (decoder->bInput()){
    decoder->lastQuad += 2;
  }
}

void quadrature_decoder_handleEvent(QuadratureDecoder *decoder){
  unsigned char newQuad = 0;
  unsigned char quadDiff;
  unsigned char a = decoder->aInput();
  unsigned char b = decoder->bInput();
  
  char direction = 1;
  
  if (decoder->aInput()){
    newQuad += 1;
  }
  if (decoder->bInput()){
    newQuad += 2;
  }

  quadDiff = newQuad ^ decoder->lastQuad;
  decoder->lastQuad = newQuad;

  if (quadDiff == 1){
    //A changed
    if ((a && (!b)) ||
	((!a) && b)   ){ direction = -1; }
  }
  else if (quadDiff == 2){
    //B changed
    if ((b && a) ||
	((!b) && (!a))) { direction = -1; }
  }
  else {
    //Two changes == noice
    direction = 0;
  }
  if (direction!=0){
    if (direction == 1){ decoder->callback(1); }
    else { decoder->callback(0); }
  }
}


void quadrature_decoder_detentFilterInit(QuadratureDecoderDetentFilter *filter){
  filter->index = 16;
}
void quadrature_decoder_detentFilterHandleEvent(QuadratureDecoderDetentFilter *filter, unsigned char up){
  char event = 0;
  
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    
    if (up){
      filter->index++;
    }
    else {
      filter->index--;
    }
    if (16+4 == filter->index){
      event = 1;
      filter->index = 16;
    }
    else if (16-4 >= filter->index){
      event = -1;
      filter->index = 16;
    }
    
  }
    
  if (event==1){
    filter->callback(1);
  }
  else if (event==-1){
    filter->callback(0);
  }
}
