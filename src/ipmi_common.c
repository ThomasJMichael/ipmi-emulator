#include "ipmi_common.h"
#include "ipmi_common_internal.h"
#include <stdio.h>
#include <string.h>

uint32_t sum_buffer(const uint8_t *buf, size_t buf_len) {
  uint32_t sum = 0;
  for (size_t i = 0; i < buf_len; i++) {
    sum += buf[i];
  }
  return sum;
}

uint8_t calc_checksum(uint8_t *data, size_t len) {
  uint8_t sum = 0;
  for (size_t i = 0; i < len; i++) {
    sum += data[i];
  }
  return (uint8_t)(0 - sum);
}
/**
 * @brief Computes Checksum2 for an IPMI request or response message.
 *
 * Constructs a temporary buffer containing only the fields covered by
 * Checksum2, avoiding any garbage from uninitialized static fields.
 */
uint8_t ipmi_calc_checksum2(const ipmi_msg_t *msg) {
  if (msg->data_len > IPMI_PACKET_MAX_BUFFER) {
    return -3;
  }
  uint8_t tmp[sizeof(msg->rq_addr) + sizeof(msg->rq_seq_lun) +
              IPMI_PACKET_MAX_BUFFER];
  size_t i = 0;

  tmp[i++] = msg->rq_addr;
  tmp[i++] = msg->rq_seq_lun;
  tmp[i++] = msg->cmd;

  if (IPMI_IS_RESPONSE(msg->netfn_lun)) {
    tmp[i++] = msg->completion_code;
  }

  memcpy(&tmp[i], msg->data, msg->data_len);
  i += msg->data_len;

  return calc_checksum(tmp, i);
}

// According to IPMI spec:
//   Checksum1 = 0 - (rs_addr + netfn_lun)
//   Checksum2 = 0 - (rq_addr + seq_lun + cmd [+ completion_code] + data)
int validate_ipmi_checksum(ipmi_msg_t *msg) {
  if (msg->data_len > 0 && msg->data_len > sizeof(msg->data)) {
    return -3; // Invalid length
  }

  if ((msg->rs_addr + msg->netfn_lun + msg->checksum1) % 256 != 0) {
    return -1;
  }
  uint32_t msg_sum = (msg->rq_addr + msg->rq_seq_lun + msg->cmd +
                      sum_buffer(msg->data, msg->data_len) + msg->checksum2);

  if (IPMI_IS_RESPONSE(msg->netfn_lun)) {
    msg_sum += msg->completion_code;
  }

  if (msg_sum % 256 != 0) {
    return -2;
  }
  return 0; // Both checksums are valid
}

int build_ipmi_response(const ipmi_msg_t *req, ipmi_msg_t *resp,
                        uint8_t completion_code, const uint8_t *data,
                        size_t data_len) {
  resp->rs_addr = req->rq_addr;
  resp->netfn_lun = req->netfn_lun + 1;
  resp->checksum1 = calc_checksum(&resp->rs_addr, sizeof(resp->rs_addr) +
                                                      sizeof(resp->netfn_lun));
  resp->rq_addr = req->rs_addr;
  resp->rq_seq_lun = req->rq_seq_lun;
  resp->cmd = req->cmd;
  resp->completion_code = completion_code;

  if (data_len <= IPMI_PACKET_MAX_BUFFER) {
    memcpy(resp->data, data, data_len);
    resp->data_len = data_len;
  } else {
    printf("Buffer overflow in build_ipmi_response\n");
    resp->data_len = 0;
    return -1;
  }

  resp->checksum2 = ipmi_calc_checksum2(resp);
  return 0;
}
