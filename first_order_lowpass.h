/*
  ==============================================================================

    one_pole.h
    Author:  Jan Janssen
    Source:  https://www.earlevel.com/main/2012/12/15/a-one-pole-filter/

  ==============================================================================
*/


/*
************************************************************************************************************************
*
************************************************************************************************************************
*/

#ifndef FIRST_ORDER_LOWPASS_h
#define FIRST_ORDER_LOWPASS_h

/*
************************************************************************************************************************
*           INCLUDE FILES
************************************************************************************************************************
*/

#include <stdint.h>

/*
************************************************************************************************************************
*           DATA TYPES
************************************************************************************************************************
*/

typedef struct FIRST_ORDER_LOWPASS_FILTER_T {
    
    //filter variables
    float tau;
    int samplefreq;

    //internal memory
    float X_0, Y_0;
    float K, A, B, f, wc;

    //detect filter firsts sample    
    int init;
} first_order_lowpass_filter_t;


/*
************************************************************************************************************************
*           FUNCTION PROTOTYPES
************************************************************************************************************************
*/

void first_order_lowpass_init(first_order_lowpass_filter_t *filter, int samplefreq);
void first_order_lowpass_set(first_order_lowpass_filter_t *filter, float Fc);
float first_order_lowpass_process(first_order_lowpass_filter_t *filter, float in);



#endif // FIRST_ORDER_LOWPASS_h