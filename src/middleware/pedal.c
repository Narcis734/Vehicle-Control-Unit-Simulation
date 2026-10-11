#include "pedal.h"

vcu_pedal_status_t vcu_pedal_parse(const vcu_pedal_raw_t *raw,
                                   vcu_pedal_value_t *parsed)
{
    int32_t difference;

    if (parsed == 0)
    {
        return VCU_PEDAL_INVALID_ARGUMENT;
    }
    parsed->position_permille = 0U;

    if (raw == 0)
    {
        return VCU_PEDAL_INVALID_ARGUMENT;
    }

    if (raw->channel_a_permille < VCU_PEDAL_MIN_PERMILLE ||
        raw->channel_a_permille > VCU_PEDAL_MAX_PERMILLE ||
        raw->channel_b_permille < VCU_PEDAL_MIN_PERMILLE ||
        raw->channel_b_permille > VCU_PEDAL_MAX_PERMILLE)
    {
        return VCU_PEDAL_OUT_OF_RANGE;
    }

    difference = raw->channel_a_permille - raw->channel_b_permille;
    if (difference < 0)
    {
        difference = -difference;
    }
    if (difference > VCU_PEDAL_MAX_DISAGREEMENT_PERMILLE)
    {
        return VCU_PEDAL_DISAGREEMENT;
    }

    parsed->position_permille =
        (uint16_t)((raw->channel_a_permille + raw->channel_b_permille) / 2);
    return VCU_PEDAL_OK;
}
