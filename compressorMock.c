#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*
    Math Constants are not defined in Standard C/C++. To use them, you must first define _USE_MATH_DEFINES and then include cmath or math.h.
    see https://docs.microsoft.com/en-us/cpp/c-runtime-library/math-constants?redirectedfrom=MSDN&view=msvc-160
*/

#define _USE_MATH_DEFINES // for C
#include <math.h>

#include "compressor_core.h"

/**********************************************************************************************************************************************************/

#define COMP_BUF_SIZE 256
#define SIZE 24000
#define LOOPSIZE 128

int main(int argc, char* argv[])
{
    float* input = malloc(sizeof(float) * SIZE);
    float* output = malloc(sizeof(float) * SIZE);

    uint32_t Hz = 50; //Hz
    uint32_t Samplerate = 48000; //samples

    float Treshold = -3; //dB
    float Ratio = 8; //ratio

    float Attack = 100; //ms
    float Release = 100; //ms

    //instantiate compressor instance
    compressor_t* compressor = (compressor_t*)malloc(sizeof(compressor_t));

    //init compressor
    compressor_init(compressor, LOOPSIZE, DOWNWARD, 48000);

    compressor_update_parameters(compressor, Treshold, Ratio, (int)(Attack / (1000.f/Samplerate)), (int)(Release / (1000.f / Samplerate)), HARD_KNEE);

    printf("creating input signal");

    for (uint32_t i = 0; i < SIZE; i++)
    {
        //input[i] = sin(2.0f * PI * Hz * i / SIZE ) * 0.5; // should not pass -3db 
        input[i] = sinf(2.0f * M_PI * Hz * i / SIZE); // should not pass -3db 

        if (i >= 7500 && i <= 17500)
        {
            input[i] = sinf(2.0f * M_PI * Hz * i / SIZE) / 3; //should pass -6db
        }
    }

    float* runInput = malloc(sizeof(float) * LOOPSIZE);
    float* runOutput = malloc(sizeof(float) * LOOPSIZE);

    //process loop
    for (uint32_t i = 0; i < SIZE; i+=LOOPSIZE)
    {
        memcpy(runInput, input + i, sizeof(float) * LOOPSIZE);
        compressor_run(compressor, runInput, runOutput, LOOPSIZE);
        memcpy(output + i, runOutput, sizeof(float) * LOOPSIZE);
    }

    while (1);

    free(input,
        output,
        compressor,
        runInput,
        runOutput
    );

    return 0;
}