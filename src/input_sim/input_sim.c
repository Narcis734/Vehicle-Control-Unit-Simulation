#include "input_sim.h"

void vcu_input_sim_init(vcu_input_sim_t *sim)
{
    if (sim == 0)
    {
        return;
    }

    sim->frame.pedal.channel_a_permille = 0;
    sim->frame.pedal.channel_b_permille = 0;
    sim->frame.enable = false;
    sim->frame.clear_fault = false;
}

void vcu_input_sim_set_pedals(vcu_input_sim_t *sim,
                              int32_t channel_a_permille,
                              int32_t channel_b_permille)
{
    if (sim == 0)
    {
        return;
    }

    sim->frame.pedal.channel_a_permille = channel_a_permille;
    sim->frame.pedal.channel_b_permille = channel_b_permille;
}

void vcu_input_sim_set_enable(vcu_input_sim_t *sim, bool enabled)
{
    if (sim != 0)
    {
        sim->frame.enable = enabled;
    }
}

void vcu_input_sim_request_fault_clear(vcu_input_sim_t *sim, bool requested)
{
    if (sim != 0)
    {
        sim->frame.clear_fault = requested;
    }
}

vcu_input_frame_t vcu_input_sim_sample(const vcu_input_sim_t *sim)
{
    vcu_input_frame_t safe_frame = {{0, 0}, false, false};

    if (sim != 0)
    {
        return sim->frame;
    }
    return safe_frame;
}
