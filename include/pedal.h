#ifndef VCU_PEDAL_H
#define VCU_PEDAL_H

#include "vcu_types.h"

#define VCU_PEDAL_MIN_PERMILLE 0
#define VCU_PEDAL_MAX_PERMILLE 1000
#define VCU_PEDAL_MAX_DISAGREEMENT_PERMILLE 50

typedef enum
{
    VCU_PEDAL_OK = 0,
    VCU_PEDAL_INVALID_ARGUMENT,
    VCU_PEDAL_OUT_OF_RANGE,
    VCU_PEDAL_DISAGREEMENT
} vcu_pedal_status_t;

typedef struct
{
    uint16_t position_permille;
} vcu_pedal_value_t;

vcu_pedal_status_t vcu_pedal_parse(const vcu_pedal_raw_t *raw,
                                   vcu_pedal_value_t *parsed);

#endif
