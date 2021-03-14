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

typedef enum {
    PLUGIN_INPUT,
    PLUGIN_OUTPUT,
    PLUGIN_MAKEUP,
    PLUGIN_THRESHOLD,
    PLUGIN_KNEE,
    PLUGIN_ATTACK,
    PLUGIN_RELEASE,
    PLUGIN_RATIO,
    PLUGIN_LIMIT_THRESHOLD,
    PLUGIN_MASTER_VOL
}PortIndex;

/**********************************************************************************************************************************************************/

typedef struct{
    
    //ports
    float* input;
    float* output;
    float* makeup;
    float* threshold;
    float* knee;
    float* attack;
    float* release;
    float* ratio;
    float* limit_threshold;
    float* volume;

    knee_type_t prev_knee;
    int prev_attack;
    int prev_release;
    float prev_ratio;
    float prev_threshold;

    compressor_t compressor;

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

    compressor_init(self->compressor, COMP_BUF_SIZE, DOWNWARD, 48000);

    return (LV2_Handle)self;
}
/**********************************************************************************************************************************************************/
static void connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    Compressor* self = (Compressor*)instance;

    switch ((PortIndex)port)
    {
        case PLUGIN_INPUT:
            self->input = (float*) data;
            break;
        case PLUGIN_OUTPUT:
            self->output = (float*) data;
            break;
        case PLUGIN_MAKEUP:
            self->makeup = (float*) data;
            break;
        case PLUGIN_THRESHOLD:
            self->threshold = (float*) data;
            break;
        case PLUGIN_KNEE:
            self->knee = (float*) data;
            break;
        case PLUGIN_ATTACK:
            self->attack = (float*) data;
            break;
        case PLUGIN_RELEASE:
            self->release = (float*) data;
            break;
        case PLUGIN_RATIO:
            self->ratio = (float*) data;
            break;
        case PLUGIN_LIMIT_THRESHOLD:
            self->limit_threshold = (float*) data;
            break;
        case PLUGIN_MASTER_VOL:
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

    //controls
    float makeup = *(self->makeup);
    float volume = *(self->volume);
    float threshold = *(self->threshold);
    float ratio = *(self->ratio);
    knee_type_t knee = (*(self->knee) == 0)? SOFT_KNEE : HARD_KNEE;
    int attack = *(self->attack);
    int release = *(self->release);

    if ((self->prev_threshold != threshold) || (self->prev_ratio != ratio) || (self->prev_release != release) || (self->prev_knee != knee) || (self->prev_attack != attack))
    {
        compressor_update_parameters(self->compressor, threshold, ratio, attack, release, knee);
    
        self->prev_attack = attack;
        self->prev_release = release;
        self->prev_knee = knee;
        self->prev_ratio = ratio;
        self->prev_threshold = threshold;
    }

    compressor_run(self->compressor, self->input, self->output, n_samples);
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
