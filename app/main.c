#include <stdio.h>

#include "input_sim.h"
#include "vcu.h"

static const char *vcu_state_name(vcu_state_t state)
{
    switch (state)
    {
    case VCU_STATE_INIT:
        return "INIT";
    case VCU_STATE_READY:
        return "READY";
    case VCU_STATE_DRIVE:
        return "DRIVE";
    case VCU_STATE_FAULT:
        return "FAULT";
    default:
        return "UNKNOWN";
    }
}

int main(void)
{
    vcu_t vcu;
    vcu_input_sim_t simulator;
    unsigned int step;

    vcu_init(&vcu);
    vcu_input_sim_init(&simulator);
    vcu_input_sim_set_enable(&simulator, true);

    for (step = 0U; step < 5U; ++step)
    {
        const int32_t pedal_permille = step < 2U ? 0 : (int32_t)(step - 1U) * 250;
        vcu_input_frame_t input;

        vcu_input_sim_set_pedals(&simulator, pedal_permille, pedal_permille);
        input = vcu_input_sim_sample(&simulator);
        vcu_update(&vcu, &input, 10U);
        printf("step=%u state=%s pedal=%ld torque=%u Nm\n",
               step,
               vcu_state_name(vcu.state),
               (long)pedal_permille,
               (unsigned int)vcu.torque_request_nm);
    }

    return 0;
}
