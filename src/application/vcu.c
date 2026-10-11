#include "vcu.h"

#include "pedal.h"

static void vcu_enter_state(vcu_t *vcu, vcu_state_t state)
{
    vcu->state = state;
    vcu->state_elapsed_ms = 0U;
}

static void vcu_latch_fault(vcu_t *vcu, vcu_fault_t fault)
{
    vcu->fault = fault;
    vcu->torque_request_nm = 0U;
    vcu_enter_state(vcu, VCU_STATE_FAULT);
}

static void vcu_add_elapsed(vcu_t *vcu, uint32_t elapsed_ms)
{
    if (UINT32_MAX - vcu->state_elapsed_ms < elapsed_ms)
    {
        vcu->state_elapsed_ms = UINT32_MAX;
    }
    else
    {
        vcu->state_elapsed_ms += elapsed_ms;
    }
}

void vcu_init(vcu_t *vcu)
{
    if (vcu == 0)
    {
        return;
    }

    vcu->state = VCU_STATE_INIT;
    vcu->fault = VCU_FAULT_NONE;
    vcu->state_elapsed_ms = 0U;
    vcu->torque_request_nm = 0U;
}

void vcu_update(vcu_t *vcu, const vcu_input_frame_t *input, uint32_t elapsed_ms)
{
    vcu_pedal_value_t pedal;
    vcu_pedal_status_t pedal_status;

    if (vcu == 0)
    {
        return;
    }

    vcu->torque_request_nm = 0U;

    if (vcu->state == VCU_STATE_FAULT)
    {
        if (input != 0 && input->clear_fault && !input->enable &&
            vcu_pedal_parse(&input->pedal, &pedal) == VCU_PEDAL_OK &&
            pedal.position_permille <= VCU_PEDAL_DEADBAND_PERMILLE)
        {
            vcu->fault = VCU_FAULT_NONE;
            vcu_enter_state(vcu, VCU_STATE_INIT);
        }
        return;
    }

    if (input == 0)
    {
        vcu_latch_fault(vcu, VCU_FAULT_INVALID_INPUT);
        return;
    }

    if (elapsed_ms > VCU_MAX_CYCLE_MS)
    {
        vcu_latch_fault(vcu, VCU_FAULT_CYCLE_TIMEOUT);
        return;
    }

    pedal_status = vcu_pedal_parse(&input->pedal, &pedal);
    if (pedal_status != VCU_PEDAL_OK)
    {
        vcu_latch_fault(vcu,
                        pedal_status == VCU_PEDAL_OUT_OF_RANGE
                            ? VCU_FAULT_PEDAL_RANGE
                            : VCU_FAULT_PEDAL_DISAGREEMENT);
        return;
    }

    vcu_add_elapsed(vcu, elapsed_ms);

    switch (vcu->state)
    {
    case VCU_STATE_INIT:
        if (vcu->state_elapsed_ms >= VCU_INIT_TIMEOUT_MS)
        {
            vcu_latch_fault(vcu, VCU_FAULT_INIT_TIMEOUT);
        }
        else if (input->enable)
        {
            vcu_enter_state(vcu, VCU_STATE_READY);
        }
        break;

    case VCU_STATE_READY:
        if (!input->enable)
        {
            vcu_enter_state(vcu, VCU_STATE_INIT);
        }
        else if (pedal.position_permille > VCU_PEDAL_DEADBAND_PERMILLE)
        {
            vcu_enter_state(vcu, VCU_STATE_DRIVE);
        }
        break;

    case VCU_STATE_DRIVE:
        if (!input->enable)
        {
            vcu_enter_state(vcu, VCU_STATE_INIT);
        }
        else if (pedal.position_permille <= VCU_PEDAL_DEADBAND_PERMILLE)
        {
            vcu_enter_state(vcu, VCU_STATE_READY);
        }
        else
        {
            vcu->torque_request_nm =
                (uint16_t)(((uint32_t)pedal.position_permille * VCU_MAX_TORQUE_NM) /
                           VCU_PEDAL_MAX_PERMILLE);
        }
        break;

    case VCU_STATE_FAULT:
    default:
        vcu_latch_fault(vcu, VCU_FAULT_INVALID_INPUT);
        break;
    }
}
