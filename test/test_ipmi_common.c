#include "ipmi_common.h"
#include "ipmi_common_internal.h"
#include "unity.h"
#include "unity_internals.h"
#include <stdint.h>

void setUp(void) {}
void tearDown(void) {}

void test_calc_checksum_basic(void) {
  uint8_t buf[] = {0x20, 0x06}; // sum - 0x26 -> 0xDA
  uint8_t checksum = calc_checksum(buf, sizeof(buf));
  TEST_ASSERT_EQUAL_UINT8(0xDA, checksum);
}

void test_sum_buffer(void) {
  uint8_t buf[] = {1, 2, 3, 4}; // sum = 10
  uint32_t sum = sum_buffer(buf, sizeof(buf));
  TEST_ASSERT_EQUAL_UINT32(10, sum);
}

void test_ipmi_calc_checksum2_request(void) {
  // TODO: Repleace these hardcoded values with enums when defined
  ipmi_msg_t msg = {
      .rq_addr = 0x81,
      .rq_seq_lun = 0x00,
      .cmd = 0x01, // Get device ID
      .data_len = 0,
      .netfn_lun = 0x06 // Application Netfn (request)
  };
  uint8_t expected[] = {0x81, 0x00, 0x01};
  uint8_t expected_checksum = calc_checksum(expected, sizeof(expected));

  uint8_t actual_checksum = ipmi_calc_checksum2(&msg);
  TEST_ASSERT_EQUAL_UINT8(expected_checksum, actual_checksum);
}

/* Tests for validate checksum
 *  Tests for:
 *         valid checksum
 *         invalid checksum1
 *         invalid checksum2
 *         invalid data_len
 */
void test_validate_ipmi_checksum_valid_request(void) {
  ipmi_msg_t msg = {.rs_addr = 0x20,
                    .netfn_lun = 0x06, // Application request
                    .rq_addr = 0x81,
                    .rq_seq_lun = 0x00,
                    .cmd = 0x01,
                    .data_len = 0};

  msg.checksum1 = calc_checksum(&msg.rs_addr, 2);
  msg.checksum2 = ipmi_calc_checksum2(&msg);

  TEST_ASSERT_EQUAL_INT(0, validate_ipmi_checksum(&msg));
}

void test_invalid_checksum1(void) {
  ipmi_msg_t msg = {
      .rs_addr = 0x20,
      .netfn_lun = 0x06,
      .rq_addr = 0x81,
      .rq_seq_lun = 0x00,
      .cmd = 0x01,
      .data_len = 0,
      .checksum1 = 0x00, // Intentionally invalid
      .checksum2 = 0x00  // Intentionally invalid
  };

  int result = validate_ipmi_checksum(&msg);
  TEST_ASSERT_EQUAL_INT(-1, result);
}
void test_invalid_checksum2(void) {
  ipmi_msg_t msg = {
      .rs_addr = 0x20,
      .netfn_lun = 0x06,
      .rq_addr = 0x81,
      .rq_seq_lun = 0x00,
      .cmd = 0x01,
      .data_len = 0,
  };
  msg.checksum1 = calc_checksum(&msg.rs_addr, 2);
  msg.checksum2 = 0x0; // Invalid

  int result = validate_ipmi_checksum(&msg);
  TEST_ASSERT_EQUAL_INT(-2, result);
}
void test_invalid_data_len(void) {
  ipmi_msg_t msg = {
      .rs_addr = 0x20,
      .netfn_lun = 0x06,
      .rq_addr = 0x81,
      .rq_seq_lun = 0x00,
      .data = {0x01, 0x02, 0x03},
      .cmd = 0x01,
      .data_len = 999,
  };
  msg.checksum1 = calc_checksum(&msg.rs_addr, 2);
  msg.checksum2 = ipmi_calc_checksum2(&msg);

  int result = validate_ipmi_checksum(&msg);
  TEST_ASSERT_EQUAL_INT(-3, result);
}

void test_data_len_boundary_valid(void) {
  ipmi_msg_t msg = {.rs_addr = 0x20,
                    .netfn_lun = 0x06,
                    .rq_addr = 0x81,
                    .rq_seq_lun = 0x00,
                    .cmd = 0x01,
                    .data_len = sizeof(msg.data)};

  // Fill data with dummy values
  for (size_t i = 0; i < msg.data_len; i++) {
    msg.data[i] = (uint8_t)i;
  }

  msg.checksum1 = calc_checksum(&msg.rs_addr, 2);
  msg.checksum2 = ipmi_calc_checksum2(&msg);

  int result = validate_ipmi_checksum(&msg);
  TEST_ASSERT_EQUAL_INT(0, result); // Should be valid
}

void test_build_ipmi_response_minimal(void) {
  ipmi_msg_t req = {.rs_addr = 0x20,
                    .netfn_lun = 0x06,
                    .rq_addr = 0x81,
                    .rq_seq_lun = 0x00,
                    .cmd = 0x01,
                    .data_len = 0};

  req.checksum1 = calc_checksum(&req.rs_addr, 2);
  req.checksum2 = ipmi_calc_checksum2(&req);

  ipmi_msg_t resp;
  build_ipmi_response(&req, &resp, 0x00, NULL, 0);

  TEST_ASSERT_EQUAL_INT(0, validate_ipmi_checksum(&resp));
}

void test_ipmi_calc_checksum2_response(void) {
  ipmi_msg_t msg = {
      .rq_addr = 0x81,
      .rq_seq_lun = 0x00,
      .cmd = 0x01,
      .completion_code = 0x00,
      .data_len = 0,
      .netfn_lun = 0x07 // Even + 1 ⇒ response
  };

  uint8_t expected[] = {0x81, 0x00, 0x01, 0x00};
  uint8_t expected_checksum = calc_checksum(expected, sizeof(expected));
  uint8_t actual = ipmi_calc_checksum2(&msg);

  TEST_ASSERT_EQUAL_UINT8(expected_checksum, actual);
}

void test_ipmi_calc_checksum2_response_with_data(void) {
  ipmi_msg_t msg = {.rq_addr = 0x81,
                    .rq_seq_lun = 0x12,
                    .cmd = 0x01,
                    .completion_code = 0x00,
                    .netfn_lun = 0x07, // Response (odd NetFn)
                    .data_len = 4,
                    .data = {0xDE, 0xAD, 0xBE, 0xEF}};

  // Expected checksum2 input: rq_addr, rq_seq_lun, cmd, completion_code, data[]
  uint8_t expected[] = {
      0x81,                  // rq_addr
      0x12,                  // rq_seq_lun
      0x01,                  // cmd
      0x00,                  // completion_code
      0xDE, 0xAD, 0xBE, 0xEF // data payload
  };

  uint8_t expected_checksum = calc_checksum(expected, sizeof(expected));
  uint8_t actual_checksum = ipmi_calc_checksum2(&msg);

  TEST_ASSERT_EQUAL_UINT8(expected_checksum, actual_checksum);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_sum_buffer);
  RUN_TEST(test_calc_checksum_basic);
  RUN_TEST(test_ipmi_calc_checksum2_request);
  RUN_TEST(test_ipmi_calc_checksum2_response);
  RUN_TEST(test_ipmi_calc_checksum2_response_with_data);
  RUN_TEST(test_validate_ipmi_checksum_valid_request);
  RUN_TEST(test_build_ipmi_response_minimal);
  RUN_TEST(test_invalid_checksum1);
  RUN_TEST(test_invalid_checksum2);
  RUN_TEST(test_invalid_data_len);
  RUN_TEST(test_data_len_boundary_valid);
  return UNITY_END();
}
