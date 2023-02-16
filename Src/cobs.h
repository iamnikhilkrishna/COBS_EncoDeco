#ifndef COBS_H
#define COBS_H

#include <stdint.h>
#include <stddef.h>

#define USE_COBS_DEFAULT_DELIMITER  //Use default delimiter 0x00
#define COBS_DEFAULT_DELIMITER 0x00
#define COBS_MAX_NEXT_DELIM_COUNT 0xFF

typedef enum {
COBS_BAD_ARGS,
COBS_ENCODER_FAIL,
COBS_ENCODER_OK,
}CobsEncoderResult_t;


typedef enum {
COBS_FAIL,
COBS_OK,
}CobsDecoderResult_t;
  
#ifdef __cplusplus
extern "C"
{
#endif


CobsEncoderResult_t CobsEncode(const void* pSourceBuffer,size_t SrcBufferSize,
 void* pDestBuffer, size_t DestBufferSize ,size_t* encodedOutputSize );

 #ifndef USE_COBS_DEFAULT_DELIMITER
void CobsSetCustomDelimiter(uint8_t newDelimiter);
void CobsSetCustomMaxNextDelimitorPosCount(uint8_t newMaxCountValue);
#endif


CobsDecoderResult_t CobsDecode(const void* pSourceBuffer,size_t SrcBufferSize,
 void* pDestBuffer, size_t DestBufferSize ,size_t* encodedOutputSize );

#ifdef __cplusplus
}
#endif




#endif //COBS_H