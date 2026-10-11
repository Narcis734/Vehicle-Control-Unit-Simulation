#ifndef VCU_INPUT_SIM_H
#define VCU_INPUT_SIM_H

#include "vcu_types.h"

typedef struct
{
    vcu_input_frame_t frame;
} vcu_input_sim_t;

void vcu_input_sim_init(vcu_input_sim_t *sim);
void vcu_input_sim_set_pedals(vcu_input_sim_t *sim,
                              int32_t channel_a_permille,
                              int32_t channel_b_permille);
void vcu_input_sim_set_enable(vcu_input_sim_t *sim, bool enabled);
void vcu_input_sim_request_fault_clear(vcu_input_sim_t *sim, bool requested);
vcu_input_frame_t vcu_input_sim_sample(const vcu_input_sim_t *sim);

#endif
