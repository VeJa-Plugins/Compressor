/*
  ==============================================================================

    first_order_lowpass.c
    Author:  Jarno Verheesen & Jan Janssen

  ==============================================================================
*/

/*
************************************************************************************************************************
*           INCLUDE FILES
************************************************************************************************************************
*/

/*
    Math Constants are not defined in Standard C/C++. To use them, you must first define _USE_MATH_DEFINES and then include cmath or math.h.
    see https://docs.microsoft.com/en-us/cpp/c-runtime-library/math-constants?redirectedfrom=MSDN&view=msvc-160
*/

#define _USE_MATH_DEFINES // for C
#include <math.h>

#include "first_order_lowpass.h"

/*
************************************************************************************************************************
*           GLOBAL FUNCTIONS
************************************************************************************************************************
*/

void first_order_lowpass_init(first_order_lowpass_filter_t *filter, int samplefreq)
{
    filter->tau = 1.0f / (float)samplefreq;
    filter->X_0 = 0.f;
    filter->Y_0 = 0.f;
}

void first_order_lowpass_set(first_order_lowpass_filter_t *filter, float Fc)
{
    filter->f = Fc;
    filter->wc = 2.f * M_PI * filter->f;
    filter->K = filter->wc * filter->tau;
    filter->A = filter->wc * filter->tau + 2.f;
    filter->B = filter->wc * filter->tau - 2.f;

    filter->init = 0;
}

float first_order_lowpass_process(first_order_lowpass_filter_t *filter, float in)
{
    /*
                z + 1
     G(z) = K ----------
               Az + B
    */
    if (filter->init == 0)
    {
        float u_1 = filter->X_0;
        float y_1 = filter->Y_0;

        float output = (-filter->B * y_1 + filter->K * (in + u_1)) / filter->A;

        filter->X_0 = in;
        filter->Y_0 = output;

        filter->init = 1;

        return output;
    }

    float output = (-filter->B * filter->Y_0 + filter->K * (in + filter->X_0)) / filter->A;

    filter->X_0 = in;
    filter->Y_0 = output;

    return output;
}