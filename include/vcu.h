#ifndef VCU_H
#define VCU_H

#include "vcu_types.h"

#define VCU_MAX_TORQUE_NM 3000U
#define VCU_PEDAL_DEADBAND_PERMILLE 10U
#define VCU_INIT_TIMEOUT_MS 5000U
#define VCU_MAX_CYCLE_MS 100U

typedef struct
{
    vcu_state_t state;
    vcu_fault_t fault;
    uint32_t state_elapsed_ms;
    uint16_t torque_request_nm;
} vcu_t;

void vcu_init(vcu_t *vcu);
void vcu_update(vcu_t *vcu, const vcu_input_frame_t *input, uint32_t elapsed_ms);

#endif
