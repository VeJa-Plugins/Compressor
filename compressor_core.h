/*
  ==============================================================================

	compressor_core.h
  Author: Jarno verheese &  Jan Janssen

  ==============================================================================
*/

#ifndef COMPRESSOR_CORE_H_INCLUDED
#define COMPRESSOR_CORE_H_INCLUDED

#define PI 3.14159265358979323846264338327950288

#include  "first_order_lowpass.h"

/*
    1. feed forward design
            ========
   in ------| gain |------ out
        |   ========  |
        |      |      |
        | =========== |
        --| control |--
          | circuit |
          ===========
   2. Controls and features
        --DONE
   - Threshold.
        --DONE
   - Ratio
        --DONE
   - Attack and Release
        --DONE
   - Soft and Hard knees
        --DONE
   - RMS sensing
        --DONE
   - Look ahead
        --TODO
*/

typedef enum {
    DOWNWARD,
    UPWARD
} compressor_type_t;

typedef enum {
    SOFT_KNEE,
    HARD_KNEE
} knee_type_t;

typedef struct WINDOW_T {
    float power;
    int size;
} window_t;

typedef struct ENVELOPE_T {
    float _Tau;
    int attack_length;
    int attack_counter;
    int release_length;
    int release_counter;
} envelope_t;

typedef struct COMPRESSOR_T {
    float gain;
    float ratio;
    float treshold;
    knee_type_t knee_mode;
    compressor_type_t compression_mode;

    //buffers
    float rms_input_dB[256];
    float threshold_window[256];
    float envelope_window[256];
    float softknee_window[256];

    envelope_t envelope;
    window_t window;
    first_order_lowpass_filter_t first_order_lowpass;

} compressor_t;

/// <summary>The compressor must be instantiated with the size of the lookahead buffer.</summary>
/// <param name="lookAheadSize">This value should be equal to the maximum input size buffer.</param> 
/// <param name="mode">This value sets the behaviour of the compressor. The modes are Downward and Upward compression.</param> 
/// <returns> none
void compressor_init(compressor_t *compressor, int lookAheadSize, compressor_type_t mode, int samplerate);

/// <summary>Method used to update the control parameters for runtime processing.</summary>
/// <param name="Treshold">Threshold value before the comressor kicks in.</param> 
/// <param name="Ratio">Compression ratio between in and output.</param> 
/// <param name="AttackDuration">Attack rate in number of samples.</param> 
/// <param name="ReleaseDuration">Release rate in number of samples.</param> 
/// <param name="KneeMode">Set the compressors knee to soft or hard.</param> 
/// <returns> none
void compressor_update_parameters(compressor_t *compressor, float treshold, float ratio, int attack_duration, int release_duration, knee_type_t knee_mode);

/// <summary>This method called to apply the Gate to the input sample. It is suited for LV2 plugins</summary>
/// <param name="input">Ppointer to address where the input is located.</param> 
/// <param name="output">Pointer to address where the output should be written to.</param> 
/// <param name="output">Number of samples present in arrays.</param> 
/// <returns> none
void compressor_run(compressor_t *compressor, float* input, float* output, uint32_t n_samples);

#endif //COMPRESSOR_CORE_H_INCLUDED
