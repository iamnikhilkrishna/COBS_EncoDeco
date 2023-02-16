#include "cobs.h"
#include <stdio.h>

#define COBS_MIN_ENODED_RESULT_OVERHEAD 2

uint8_t gCOBSDelimiter = COBS_DEFAULT_DELIMITER;
uint8_t gCOBSMaxNextDelimiterCount = COBS_MAX_NEXT_DELIM_COUNT;

CobsDecoderResult_t CobsDecode(const void *pSourceBuffer, size_t SrcBufferSize, void *pDestBuffer, size_t DestBufferSize, size_t *decodedOutputSize)
{
    uint8_t *pInputBuffer = (uint8_t *)pSourceBuffer;
    uint8_t *pOutputBuffer = (uint8_t *)pDestBuffer;
    CobsDecoderResult_t decodingResult = COBS_FAIL;
    if (NULL == pSourceBuffer || NULL == pDestBuffer || 0 >= SrcBufferSize || 0 >= DestBufferSize)
    {
        decodingResult = COBS_FAIL;
    }
    else
    {
        uint8_t *pCOBSHeader = pInputBuffer;      // Encoded Result start with Next Zero Pos as STX
        uint8_t *pCOBSencodedData = pInputBuffer; // data from Source is copied after STX if it is non delimiter byte
        size_t destCurrrentBytePos = 0;
        uint8_t nextCOBSHeaderPos = 0;
        nextCOBSHeaderPos = *pCOBSHeader;

        volatile int a = 0;

        for (size_t currentBytePos = 1; currentBytePos < SrcBufferSize; currentBytePos++)
        {

            if (currentBytePos != nextCOBSHeaderPos && nextCOBSHeaderPos > 0)
            {
                pOutputBuffer[destCurrrentBytePos] = pCOBSencodedData[currentBytePos]; // Blind copy COBS encoded byte as Output  byte since it is not our COBS header
                destCurrrentBytePos++;
            }
            else if (currentBytePos == nextCOBSHeaderPos)
            {

                if (currentBytePos < gCOBSMaxNextDelimiterCount)
                {
                    pOutputBuffer[destCurrrentBytePos] = gCOBSDelimiter;
                    destCurrrentBytePos++;
                }
                else if (currentBytePos == gCOBSMaxNextDelimiterCount)
                {
                    // Don't Update the dest with delimiter and  destCurrentBytePos.
                    a++;
                }

                pCOBSHeader = &pCOBSHeader[nextCOBSHeaderPos]; // Advancing COBS header pointer to next COBS header location
                nextCOBSHeaderPos = *pCOBSHeader;              // Updating nextCOBSHeader Position
                pCOBSencodedData = pCOBSHeader;                // Update pCOBSencodedData to point to next Data after next COBS header position that is acheived by setting COBSencodedData to nextCOBSHeader
                                                               // pointing currentBytePos to 1
                currentBytePos = 0;                            // reset current byte pos  to 0 as next updation statement updates it to required value of 1
            }
        }
        decodingResult = COBS_OK;
        *decodedOutputSize = destCurrrentBytePos-1;
    }

    return decodingResult;
}

#define SELF_TEST
#ifdef SELF_TEST
int main()
{
    uint8_t dummyInput[13] = {1,1,1,1,1,1,1,1,1,1,0};
    uint8_t dummyOut[15] = {[0 ... 14] = 0};

    for (int i = 0; i < 11; i++)
    {
        printf("%d\t", dummyInput[i]);
    }
    printf("\n");
    size_t outputSize = 0;
    // CobsEncoderResult_t result = CobsEncode(dummyInput, 10, dummyOut, 15, &outputSize);
    CobsDecoderResult_t result = CobsDecode(dummyInput, 11, dummyOut, 15, &outputSize);

    if (result == COBS_OK && outputSize)
    {
        for (int i = 0; i < outputSize; i++)
        {
            printf("%d\t", dummyOut[i]);
        }
    }
}
#endif