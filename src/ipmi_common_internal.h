#ifndef IPMI_COMMON_INTERNAL_H
#define IPMI_COMMON_INTERNAL_H

#include "ipmi_common.h" // For ipmi_msg_t and shared constants
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Sum a buffer of bytes as an unsigned 32-bit integer.
 *
 * Used internally for checksum validation and construction.
 *
 * @param buf Pointer to the byte buffer.
 * @param buf_len Number of bytes to sum.
 * @return The sum of all bytes in the buffer.
 */
uint32_t sum_buffer(const uint8_t *buf, size_t buf_len);

/**
 * @brief Calculate IPMI-style checksum for a given byte buffer.
 *
 * Result is calculated as: (uint8_t)(0 - sum(data)).
 *
 * @param data Pointer to the byte array to checksum.
 * @param len Length of the byte array.
 * @return The calculated checksum byte.
 */
uint8_t calc_checksum(uint8_t *data, size_t len);

/**
 * @brief Compute Checksum2 for an IPMI message.
 *
 * Includes fields rq_addr, rq_seq_lun, cmd, optional completion_code (if
 * response), and the actual data payload up to data_len.
 *
 * @param msg Pointer to a fully constructed IPMI message.
 * @return The calculated checksum2 byte.
 */
uint8_t ipmi_calc_checksum2(const ipmi_msg_t *msg);

#endif // IPMI_COMMON_INTERNAL_H
