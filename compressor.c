#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#include "compressor_core.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://VeJaPlugins.com/plugins/Release/Compressor"

#define MAP(x, Imin, Imax, Omin, Omax)      ( x - Imin ) * (Omax -  Omin)  / (Imax - Imin) + Omin;

#define COMP_BUF_SIZE 256
#define SAMPLERATE 48000

typedef enum {
    INPUT_L,
    INPUT_R,
    OUTPUT_L,
    OUTPUT_R,
    THRES,
    KNEE,
    ATTACK,
    RELEASE,
    RATIO,
    MAKEUP,
    MASTER_VOL
}PortIndex;

/**********************************************************************************************************************************************************/

typedef struct{

    //ports
    float* input_left;
    float* input_right;
    float* output_left;
    float* output_right;

    float* threshold;
    float* knee;
    float* attack;
    float* release;
    float* ratio;
    float* makeup;

    float* volume;

    float *bfr_l;
    float *bfr_r;

    float prev_threshold;
    float prev_knee;
    float prev_attack;
    float prev_release;
    float prev_ratio;
    float prev_makeup;

    sf_compressor_state_st compressor_state;

} Compressor;

/**********************************************************************************************************************************************************/
//                                                                 local functions                                                                        //
/**********************************************************************************************************************************************************/

/**********************************************************************************************************************************************************/
static LV2_Handle
instantiate(const LV2_Descriptor*   descriptor,
double                              samplerate,
const char*                         bundle_path,
const LV2_Feature* const* features)
{
    Compressor* self = (Compressor*)malloc(sizeof(Compressor));

    self->bfr_l = (float*)malloc(256*sizeof(float));
    self->bfr_r = (float*)malloc(256*sizeof(float));

    compressor_init(&self->compressor_state, samplerate);

    return (LV2_Handle)self;
}
/**********************************************************************************************************************************************************/
static void connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    Compressor* self = (Compressor*)instance;

    switch ((PortIndex)port)
    {
        case INPUT_L:
            self->input_left = (float*) data;
            break;
        case INPUT_R:
            self->input_right = (float*) data;
            break;

        case OUTPUT_L:
            self->output_left = (float*) data;
            break;
        case OUTPUT_R:
            self->output_right = (float*) data;
            break;

        case THRES:
            self->threshold = (float*) data;
            break;
        case KNEE:
            self->knee = (float*) data;
            break;
        case ATTACK:
            self->attack = (float*) data;
            break;
        case RELEASE:
            self->release = (float*) data;
            break;
        case RATIO:
            self->ratio = (float*) data;
            break;
        case MAKEUP:
            self->makeup = (float*) data;
            break;

        case MASTER_VOL:
            self->volume = (float*) data;
            break;
    }
}
/**********************************************************************************************************************************************************/
void activate(LV2_Handle instance)
{
    // TODO: include the activate function code here
}

/**********************************************************************************************************************************************************/
void run(LV2_Handle instance, uint32_t n_samples)
{
    Compressor* self = (Compressor*)instance;    

    if ((self->prev_threshold != (float)*self->threshold) || (self->prev_knee != (float)*self->knee) || (self->prev_attack != (float)*self->attack) || (self->prev_release != (float)*self->release) || (self->prev_ratio != (float)*self->ratio) || (self->prev_makeup != (float)*self->makeup) )
    {
        compressor_set_params(&self->compressor_state, (float)*self->threshold,
                                (float)*self->knee, (float)*self->ratio, ((float)*self->attack / 1000), ((float)*self->release / 1000), (float)*self->makeup);

        self->prev_threshold = (float)*self->threshold;
        self->prev_knee = (float)*self->knee;
        self->prev_attack = (float)*self->attack;
        self->prev_release = (float)*self->release;
        self->prev_ratio = (float)*self->ratio;
        self->prev_makeup = (float)*self->makeup;
    }

    compressor_process(&self->compressor_state, n_samples, self->input_left, self->input_left, self->bfr_l, self->bfr_r);

    for (uint32_t i = 0; i < n_samples; i++)
    {
        self->output_left[i] = self->bfr_l[i] * cmop_db2lin((float)*self->volume);
        self->output_right[i] = self->bfr_r[i] * cmop_db2lin((float)*self->volume);
    }
}

/**********************************************************************************************************************************************************/
void deactivate(LV2_Handle instance)
{
    // TODO: include the deactivate function code here
}
/**********************************************************************************************************************************************************/
void cleanup(LV2_Handle instance)
{
    free(instance);
}
/**********************************************************************************************************************************************************/
const void* extension_data(const char* uri)
{
    return NULL;
}
/**********************************************************************************************************************************************************/
static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    instantiate,
    connect_port,
    activate,
    run,
    deactivate,
    cleanup,
    extension_data
};
/**********************************************************************************************************************************************************/
LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}
/**********************************************************************************************************************************************************/
