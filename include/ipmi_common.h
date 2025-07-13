#ifndef IPMI_COMMON_H
#define IPMI_COMMON_H

#include <stddef.h>
#include <stdint.h>
#define IPMI_PACKET_MAX_BUFFER 256

/**
 * @struct ipmi_msg_t
 * @brief Represents a complete IPMI message over the IPMB (I²C-based)
 * transport.
 *
 * This structure holds all components of an IPMI request or response message as
 * specified in IPMI v2.0, including headers, command fields, optional data
 * payload, and checksums.
 *
 * It is used for parsing, validating, constructing, and dispatching IPMI
 * commands between IPMCs or between a host and an IPMC over IPMB.
 */
typedef struct {
  /**
   * @brief Responder’s IPMB address (7-bit I²C address, LSB always 0).
   */
  uint8_t rs_addr;

  /**
   * @brief Net Function and LUN (Logical Unit Number).
   *
   * Upper 6 bits represent the NetFn (e.g., App, Sensor, Storage),
   * lower 2 bits represent the LUN.
   */
  uint8_t netfn_lun;

  /**
   * @brief Checksum of rs_addr and netfn_lun.
   *
   * Calculated as: 0 - (rs_addr + netfn_lun).
   */
  uint8_t checksum1;

  /**
   * @brief Requester’s IPMB address (typically shelf manager or BMC).
   */
  uint8_t rq_addr;

  /**
   * @brief Request sequence number and LUN.
   *
   * Upper 6 bits: sequence number; lower 2 bits: LUN.
   */
  uint8_t rq_seq_lun;

  /**
   * @brief IPMI command code within the specified NetFn.
   *
   * Example: 0x01 for Get Device ID under NetFn 0x06 (Application).
   */
  uint8_t cmd;

  /**
   * @brief Completition code indicating the result of the command.
   *
   * This field is only present in IPMI response messages.
   * The valud 0x00 indicates success. Nonzero values represent
   * various error or status codes.
   */
  uint8_t completion_code;

  /**
   * @brief Optional data payload for the command.
   *
   * Typically zero to ~32 bytes depending on command.
   */
  uint8_t data[IPMI_PACKET_MAX_BUFFER];

  /**
   * @brief Checksum of rq_addr, rq_seq_lun, optionally completion_code, cmd,
   * and data.
   *
   * Calculated as: 0 - (rq_addr + rq_seq_lun + cmd + sum(data)).
   */
  uint8_t checksum2;

  /**
   * @brief Length of the valid data in the data[] buffer.
   */
  size_t data_len;
} ipmi_msg_t;

/**
 * @brief Validates both checksums in a given IPMI message.
 *
 * Performs IPMB checksum verification on both the header section
 * (`rs_addr`, `netfn_lun`, `checksum1`) and the body section
 * (`rq_addr`, `rq_seq_lun`, `cmd`, `data[]`, `checksum2`).
 *
 * @param msg Pointer to the parsed IPMI message structure.
 * @return 0 if both checksums are valid,
 *         -1 if the header checksum is invalid,
 *         -2 if the body checksum is invalid.
 *         -3 if the data length is invalid.
 */
int validate_ipmi_checksum(ipmi_msg_t *msg);

/**
 * @brief Builds a valid IPMI response message.
 *
 * Constructs a response IPMI message using the requester’s original message
 * as a reference. It fills in the correct response headers, completion code,
 * and optional data, and calculates both required checksums.
 *
 * @param req Pointer to the original request message.
 * @param resp Pointer to the response message to be populated.
 * @param completion_code The response completion code (e.g., 0x00 for success).
 * @param data Pointer to the response data buffer (can be NULL if no payload).
 * @param data_len Length of the response data.
 */
int build_ipmi_response(const ipmi_msg_t *req, ipmi_msg_t *resp,
                        uint8_t completion_code, const uint8_t *data,
                        size_t data_len);
#endif // IPMI_COMMON_H
