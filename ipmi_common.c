#include "ipmi_common.h"
#include <sys/types.h>

uint8_t calc_checksum(uint8_t *data, size_t len) {
  uint8_t sum = 0;
  for (size_t i = 0; i < len; i++) {
    sum += data[i];
  }
  return (uint8_t)(0 - sum);
}
