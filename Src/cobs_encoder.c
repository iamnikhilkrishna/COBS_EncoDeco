#include "cobs.h"
#include <stdio.h>

#define COBS_MIN_ENODED_RESULT_OVERHEAD 2

uint8_t gCOBSDelimiter = COBS_DEFAULT_DELIMITER;
uint8_t gCOBSMaxNextDelimiterCount = COBS_MAX_NEXT_DELIM_COUNT;


CobsEncoderResult_t CobsEncode(const void *pSourceBuffer, size_t SrcBufferSize, void *pDestBuffer, size_t DestBufferSize, size_t *encodedOutputSize)
{
    uint8_t *pInputBuffer = (uint8_t *)pSourceBuffer;
    uint8_t *pOutputBuffer = (uint8_t *)pDestBuffer;
    CobsEncoderResult_t encodingResult = COBS_FAIL;
    if (NULL == pSourceBuffer || NULL == pDestBuffer || 0 >= SrcBufferSize || 0 >= DestBufferSize || (DestBufferSize < (SrcBufferSize + COBS_MIN_ENODED_RESULT_OVERHEAD)))
    {
        encodingResult = COBS_BAD_ARGS;
    }
    else
    {
        uint8_t nextZeroPosition = 1;
        uint8_t *pCOBSHeader = pOutputBuffer;                    // Encoded Result start with Next Zero Pos as STX
        uint8_t *pCOBSOutput = &pOutputBuffer[nextZeroPosition]; // data from Source is copied after STX if it is non delimiter byte
        size_t destCurrrentBytePos = 0;

        for (size_t currentBytePos = 0; currentBytePos < SrcBufferSize; currentBytePos++)
        {
            if (pInputBuffer[currentBytePos] != gCOBSDelimiter)
            {
                pCOBSOutput[destCurrrentBytePos] = pInputBuffer[currentBytePos]; // Copy the byte in input buffer since it is not our COBS delimiter
                destCurrrentBytePos++;                                           // Upadate destCurrrentBytePos
                nextZeroPosition++;                                              // Increment Next Zero Pos
                                                                                 // TODO: Add memory boundary checks before writing to destination location

                if (nextZeroPosition == gCOBSMaxNextDelimiterCount)
                {
                    *pCOBSHeader = COBS_MAX_NEXT_DELIM_COUNT;              // Store the next zero pos count in COBS header
                    pCOBSHeader = &pCOBSHeader[COBS_MAX_NEXT_DELIM_COUNT]; // Move the COBS Header Pointer to the current zeropos
                    nextZeroPosition = 1;                                  // Reset Next Zero Pos Count to 1
                    destCurrrentBytePos++;                                 // Updating destCurrrentBytePos Since nowCobs header and dest[destpost] point to same location

#ifdef DEBUG_COBS_ENCODER
                    printf("Input - %d , currentBytePos - %d,nextZeroPos - %d , destCurrrentBytePos -%d \n", pInputBuffer[currentBytePos], currentBytePos, nextZeroPosition, destCurrrentBytePos);
#endif
                }
            }
            else if (pInputBuffer[currentBytePos] == COBS_DEFAULT_DELIMITER)
            {
                *pCOBSHeader = nextZeroPosition;              // Store the next zero pos count in COBS header
                pCOBSHeader = &pCOBSHeader[nextZeroPosition]; // Move the COBS Header Pointer to the current zeropos
                nextZeroPosition = 1;                         // Reset Next Zero Pos Count to 1
                destCurrrentBytePos++;                        // Updating destCurrrentBytePos
#ifdef DEBUG_COBS_ENCODER

                printf("Input - %d , currentBytePos - %d,nextZeroPos - %d , destCurrrentBytePos -%d \n", pInputBuffer[currentBytePos], currentBytePos, nextZeroPosition, destCurrrentBytePos);
#endif
            }
        }
        *pCOBSHeader = nextZeroPosition;                               // Store the next zero pos count in COBS header
        pCOBSOutput[destCurrrentBytePos++] = COBS_DEFAULT_DELIMITER; // Add delimiter to the end of Encoded output
        *encodedOutputSize = ++destCurrrentBytePos;                   //Updating Total Encoded Output Size 
        encodingResult = COBS_ENCODER_OK;
#ifdef DEBUG_COBS_ENCODER
        printf(" nextzeroPos -%d ,destCurrrentBytePos -%d ,encodedOutputSize -%d \n", nextZeroPosition, destCurrrentBytePos, *encodedOutputSize);
        printf("***********************************\n");
#endif
    }

    return encodingResult;
}

#ifndef USE_COBS_DEFAULT_DELIMITER
void CobsSetCustomDelimiter(uint8_t newDelimiter)
{
    gCOBSDelimiter = newDelimiter;
}

void CobsSetCustomMaxNextDelimitorPosCount(uint8_t newMaxCountValue)
{
    gCOBSMaxNextDelimiterCount = newMaxCountValue;
}

#endif

#define SELF_TEST
#ifdef SELF_TEST
int main()
{
    uint8_t dummyInput[10] = {0,0,0,	0,	0,	0,	0,	0,	0};
    uint8_t dummyOut[15] = {[0 ... 14] = 0};

    for (int i = 0; i < 9; i++)
    {
        printf("%d\t", dummyInput[i]);
    }
    printf("\n");
    size_t outputSize = 0;
    CobsEncoderResult_t result = CobsEncode(dummyInput, 9, dummyOut, 15, &outputSize);

    if (result == COBS_ENCODER_OK)
    {
        for (int i = 0; i < outputSize; i++)
        {
            printf("%d\t", dummyOut[i]);
        }
    }
}
#endif