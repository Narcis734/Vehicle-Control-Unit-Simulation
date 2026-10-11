#ifndef VCU_TYPES_H
#define VCU_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    int32_t channel_a_permille;
    int32_t channel_b_permille;
} vcu_pedal_raw_t;

typedef struct
{
    vcu_pedal_raw_t pedal;
    bool enable;
    bool clear_fault;
} vcu_input_frame_t;

typedef enum
{
    VCU_STATE_INIT = 0,
    VCU_STATE_READY,
    VCU_STATE_DRIVE,
    VCU_STATE_FAULT
} vcu_state_t;

typedef enum
{
    VCU_FAULT_NONE = 0,
    VCU_FAULT_INVALID_INPUT,
    VCU_FAULT_PEDAL_RANGE,
    VCU_FAULT_PEDAL_DISAGREEMENT,
    VCU_FAULT_INIT_TIMEOUT,
    VCU_FAULT_CYCLE_TIMEOUT
} vcu_fault_t;

#endif
