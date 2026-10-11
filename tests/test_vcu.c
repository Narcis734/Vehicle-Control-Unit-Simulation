#include "unity.h"

#include "pedal.h"
#include "vcu.h"

void setUp(void)
{
}

void tearDown(void)
{
}

static vcu_input_frame_t make_input(int32_t channel_a,
                                    int32_t channel_b,
                                    bool enable,
                                    bool clear_fault)
{
    vcu_input_frame_t input;

    input.pedal.channel_a_permille = channel_a;
    input.pedal.channel_b_permille = channel_b;
    input.enable = enable;
    input.clear_fault = clear_fault;
    return input;
}

void test_pedal_parser_accepts_valid_signals_and_averages_them(void)
{
    const vcu_pedal_raw_t raw = {400, 450};
    vcu_pedal_value_t parsed = {0U};

    TEST_ASSERT_EQUAL(VCU_PEDAL_OK, vcu_pedal_parse(&raw, &parsed));
    TEST_ASSERT_EQUAL_UINT16(425U, parsed.position_permille);
}

void test_pedal_parser_rejects_values_outside_range_and_zeros_output(void)
{
    const vcu_pedal_raw_t raw = {-1, 0};
    vcu_pedal_value_t parsed = {123U};

    TEST_ASSERT_EQUAL(VCU_PEDAL_OUT_OF_RANGE, vcu_pedal_parse(&raw, &parsed));
    TEST_ASSERT_EQUAL_UINT16(0U, parsed.position_permille);
}

void test_pedal_parser_rejects_disagreeing_channels(void)
{
    const vcu_pedal_raw_t raw = {100, 151};
    vcu_pedal_value_t parsed = {0U};

    TEST_ASSERT_EQUAL(VCU_PEDAL_DISAGREEMENT, vcu_pedal_parse(&raw, &parsed));
}

void test_init_ready_drive_generates_proportional_torque(void)
{
    vcu_t vcu;
    vcu_input_frame_t input = make_input(0, 0, true, false);

    vcu_init(&vcu);
    vcu_update(&vcu, &input, 10U);
    TEST_ASSERT_EQUAL(VCU_STATE_READY, vcu.state);
    TEST_ASSERT_EQUAL_UINT16(0U, vcu.torque_request_nm);

    input = make_input(500, 500, true, false);
    vcu_update(&vcu, &input, 10U);
    TEST_ASSERT_EQUAL(VCU_STATE_DRIVE, vcu.state);
    TEST_ASSERT_EQUAL_UINT16(0U, vcu.torque_request_nm);

    vcu_update(&vcu, &input, 10U);
    TEST_ASSERT_EQUAL_UINT16(1500U, vcu.torque_request_nm);

    input = make_input(1000, 1000, true, false);
    vcu_update(&vcu, &input, 10U);
    TEST_ASSERT_EQUAL_UINT16(VCU_MAX_TORQUE_NM, vcu.torque_request_nm);
}

void test_out_of_range_input_latches_fault_and_zero_torque(void)
{
    vcu_t vcu;
    vcu_input_frame_t input = make_input(0, 0, true, false);

    vcu_init(&vcu);
    vcu_update(&vcu, &input, 10U);
    input = make_input(500, 500, true, false);
    vcu_update(&vcu, &input, 10U);
    vcu_update(&vcu, &input, 10U);
    TEST_ASSERT_EQUAL_UINT16(1500U, vcu.torque_request_nm);

    input = make_input(1001, 1001, true, false);
    vcu_update(&vcu, &input, 10U);
    TEST_ASSERT_EQUAL(VCU_STATE_FAULT, vcu.state);
    TEST_ASSERT_EQUAL(VCU_FAULT_PEDAL_RANGE, vcu.fault);
    TEST_ASSERT_EQUAL_UINT16(0U, vcu.torque_request_nm);

    input = make_input(500, 500, true, false);
    vcu_update(&vcu, &input, 10U);
    TEST_ASSERT_EQUAL(VCU_STATE_FAULT, vcu.state);
    TEST_ASSERT_EQUAL_UINT16(0U, vcu.torque_request_nm);
}

void test_disagreeing_channels_latch_fault_and_zero_active_torque(void)
{
    vcu_t vcu;
    vcu_input_frame_t input = make_input(0, 0, true, false);

    vcu_init(&vcu);
    vcu_update(&vcu, &input, 10U);
    input = make_input(500, 500, true, false);
    vcu_update(&vcu, &input, 10U);
    vcu_update(&vcu, &input, 10U);
    TEST_ASSERT_EQUAL_UINT16(1500U, vcu.torque_request_nm);

    input = make_input(400, 451, true, false);
    vcu_update(&vcu, &input, 10U);
    TEST_ASSERT_EQUAL(VCU_STATE_FAULT, vcu.state);
    TEST_ASSERT_EQUAL(VCU_FAULT_PEDAL_DISAGREEMENT, vcu.fault);
    TEST_ASSERT_EQUAL_UINT16(0U, vcu.torque_request_nm);
}

void test_fault_clear_requires_request_safe_pedals_and_disabled_enable(void)
{
    vcu_t vcu;
    vcu_input_frame_t input = make_input(1001, 1001, true, false);

    vcu_init(&vcu);
    vcu_update(&vcu, &input, 10U);
    input = make_input(0, 0, true, true);
    vcu_update(&vcu, &input, 10U);
    TEST_ASSERT_EQUAL(VCU_STATE_FAULT, vcu.state);

    input = make_input(100, 100, false, true);
    vcu_update(&vcu, &input, 10U);
    TEST_ASSERT_EQUAL(VCU_STATE_FAULT, vcu.state);

    input = make_input(0, 0, false, true);
    vcu_update(&vcu, &input, 10U);
    TEST_ASSERT_EQUAL(VCU_STATE_INIT, vcu.state);
    TEST_ASSERT_EQUAL(VCU_FAULT_NONE, vcu.fault);
}

void test_init_timeout_latches_fault_at_deadline(void)
{
    vcu_t vcu;
    vcu_input_frame_t input = make_input(0, 0, false, false);
    uint32_t step;

    vcu_init(&vcu);
    for (step = 0U; step < (VCU_INIT_TIMEOUT_MS / VCU_MAX_CYCLE_MS) - 1U; ++step)
    {
        vcu_update(&vcu, &input, VCU_MAX_CYCLE_MS);
    }
    input.enable = true;
    vcu_update(&vcu, &input, VCU_MAX_CYCLE_MS);

    TEST_ASSERT_EQUAL(VCU_STATE_FAULT, vcu.state);
    TEST_ASSERT_EQUAL(VCU_FAULT_INIT_TIMEOUT, vcu.fault);
    TEST_ASSERT_EQUAL_UINT16(0U, vcu.torque_request_nm);
}

void test_cycle_overrun_latches_fault(void)
{
    vcu_t vcu;
    vcu_input_frame_t input = make_input(0, 0, true, false);

    vcu_init(&vcu);
    vcu_update(&vcu, &input, VCU_MAX_CYCLE_MS + 1U);

    TEST_ASSERT_EQUAL(VCU_STATE_FAULT, vcu.state);
    TEST_ASSERT_EQUAL(VCU_FAULT_CYCLE_TIMEOUT, vcu.fault);
}

void test_releasing_pedal_returns_drive_to_ready_with_zero_torque(void)
{
    vcu_t vcu;
    vcu_input_frame_t input = make_input(0, 0, true, false);

    vcu_init(&vcu);
    vcu_update(&vcu, &input, 10U);
    input = make_input(100, 100, true, false);
    vcu_update(&vcu, &input, 10U);
    vcu_update(&vcu, &input, 10U);
    TEST_ASSERT_EQUAL(VCU_STATE_DRIVE, vcu.state);
    TEST_ASSERT_TRUE(vcu.torque_request_nm > 0U);

    input = make_input(10, 10, true, false);
    vcu_update(&vcu, &input, 10U);
    TEST_ASSERT_EQUAL(VCU_STATE_READY, vcu.state);
    TEST_ASSERT_EQUAL_UINT16(0U, vcu.torque_request_nm);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_pedal_parser_accepts_valid_signals_and_averages_them);
    RUN_TEST(test_pedal_parser_rejects_values_outside_range_and_zeros_output);
    RUN_TEST(test_pedal_parser_rejects_disagreeing_channels);
    RUN_TEST(test_init_ready_drive_generates_proportional_torque);
    RUN_TEST(test_out_of_range_input_latches_fault_and_zero_torque);
    RUN_TEST(test_disagreeing_channels_latch_fault_and_zero_active_torque);
    RUN_TEST(test_fault_clear_requires_request_safe_pedals_and_disabled_enable);
    RUN_TEST(test_init_timeout_latches_fault_at_deadline);
    RUN_TEST(test_cycle_overrun_latches_fault);
    RUN_TEST(test_releasing_pedal_returns_drive_to_ready_with_zero_torque);
    return UNITY_END();
}
