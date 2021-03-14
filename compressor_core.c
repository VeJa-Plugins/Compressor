/*
  ==============================================================================

	compressor_core.c
    Author: Jarno verheese &  Jan Janssen

  ==============================================================================
*/

#include "compressor_core.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define LOW_PASS_FILTER_FREQ 40

/*******************************************************************************
								local functions
*******************************************************************************/

float calculate_power(window_t *window, const float input)
{
    //store and recalculate
    //this will not effect the signal but will prevent a NaN at exactly 0.0
    float pow = 20.f * log10f((fabsf(input - 0.0000001f)));
    //remove old sample and add new one to windowPower
    //window->power += pow;
    return pow;//window->power;
}

void RMS_sensing(compressor_t *compressor, float *input, uint32_t n_samples)
{
    //dbfs value over the signal length
    for (uint32_t n = 0; n < n_samples; n++)
    {
    	compressor->rms_input_dB[n] = calculate_power(&compressor->window, input[n]);
    }
}

void generate_envelope(compressor_t *compressor, uint32_t n_samples)
{
    for (uint32_t n = 0; n < n_samples; n++)
    {
        switch (compressor->compression_mode)
        {
        	case DOWNWARD:
        		if (compressor->rms_input_dB[n] > compressor->treshold ) //attack
            	{
            	    float adjusted_ratio = powf(10.f, (-1.0f * compressor->ratio / 20.0f)) / sqrtf(2.f);
            	    compressor->envelope_window[n] =  1.0f - ((1.0f - adjusted_ratio) * compressor->envelope.attack_counter * (1.0f / compressor->envelope.attack_length));

            	    (compressor->envelope.attack_counter < compressor->envelope.attack_length) ? compressor->envelope.attack_counter++ : (compressor->envelope.attack_counter = compressor->envelope.attack_length);
            	    (compressor->envelope.release_counter > 0) ? compressor->envelope.release_counter-- : (compressor->envelope.release_counter = 0);
            	}
            	else //release
            	{
            	    float adjusted_ratio = powf(10.f, (-1.0f * compressor->ratio / 20.0f)) / sqrtf(2.f);
            	    compressor->envelope_window[n] = adjusted_ratio + ((1.0f - adjusted_ratio) * compressor->envelope.release_counter * (1.0f / compressor->envelope.release_length));

            	    (compressor->envelope.release_counter < compressor->envelope.release_length) ? compressor->envelope.release_counter++ : (compressor->envelope.release_counter = compressor->envelope.release_length);
            	    (compressor->envelope.attack_counter > 0) ? compressor->envelope.release_counter-- : (compressor->envelope.attack_counter = 0);
            	}
            break;

        	case UPWARD:
            	if (compressor->rms_input_dB[n] < compressor->treshold) //attack
            	{
            	    float adjusted_ratio = 1.0f / (powf(10.f, (-1.0f * compressor->ratio / 20.0f)) / sqrtf(2.f));
            	    compressor->envelope_window[n] = 1.0f - ((1.0f - adjusted_ratio) * compressor->envelope.attack_counter * (1.0f / compressor->envelope.attack_length));

            	    (compressor->envelope.attack_counter < compressor->envelope.attack_length) ? compressor->envelope.attack_counter++ : (compressor->envelope.attack_counter = compressor->envelope.attack_length);
            	    (compressor->envelope.release_counter > 0) ? compressor->envelope.release_counter-- : (compressor->envelope.release_counter = 0);
            	}
            	else //release
            	{
            	    float adjusted_ratio = 1.0f / (powf(10.f, (-1.0f * compressor->ratio / 20.0f)) / sqrtf(2.f));
            	    compressor->envelope_window[n] = adjusted_ratio + ((1.0f - adjusted_ratio) * compressor->envelope.release_counter * (1.0f / compressor->envelope.release_length));

            	    (compressor->envelope.release_counter < compressor->envelope.release_length) ? compressor->envelope.release_counter++ : (compressor->envelope.release_counter = compressor->envelope.release_length);
            	    (compressor->envelope.attack_counter > 0) ? compressor->envelope.attack_counter-- : (compressor->envelope.attack_counter = 0);
            	}
        	break;
     	}
    }
}


void generate_soft_knee(compressor_t *compressor, uint32_t n_samples)
{
	for (uint32_t n = 0; n < n_samples; n++)
	{
    	switch (compressor->knee_mode)
    	{
    		case SOFT_KNEE:
    			compressor->softknee_window[n] = first_order_lowpass_process(&compressor->first_order_lowpass, compressor->envelope_window[n]);
    		break;

    		case HARD_KNEE:
    		    compressor->softknee_window[n] = compressor->envelope_window[n];
    		break;
    	}
    }
}

void apply_gain(compressor_t *compressor, float *output, float *input, uint32_t n_samples)
{
    for (uint32_t n = 0; n < n_samples; n++)
    {
        output[n] = input[n] * compressor->softknee_window[n];
    }
}

/*******************************************************************************
								global functions
*******************************************************************************/

void compressor_init(compressor_t *compressor, int lookAheadSize, compressor_type_t mode, int samplerate)
{
	compressor->compression_mode = mode;

	//init envelope
	memset(&compressor->envelope, 0, sizeof(envelope_t));
    compressor->envelope._Tau = 1.0f / samplerate;

	//init lowpass filter
	first_order_lowpass_init(&compressor->first_order_lowpass, samplerate);
	first_order_lowpass_set(&compressor->first_order_lowpass, LOW_PASS_FILTER_FREQ);

}

void compressor_update_parameters(compressor_t *compressor, float treshold, float ratio, int attack_duration, int release_duration, knee_type_t knee_mode)
{
	compressor->treshold = treshold;
	compressor->ratio = ratio;
	compressor->envelope.attack_length = attack_duration;
	compressor->envelope.release_length = release_duration;
	compressor->knee_mode = knee_mode;
}

void compressor_run(compressor_t *compressor, float* input, float* output, uint32_t n_samples)
{
    RMS_sensing(compressor, input, n_samples);
    generate_envelope(compressor, n_samples);
    generate_soft_knee(compressor, n_samples);
    apply_gain(compressor, output, input, n_samples);
}