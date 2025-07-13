#ifndef IPMI_DISPATCH_H
#define IPMI_DISPATCH_H
#include "ipmi_common.h"

int ipmi_dispatch_command(const ipmi_msg_t *req, ipmi_msg_t *resp);
int ipmi_is_valid_command(uint8_t netfn, uint8_t cmd);

#endif // IPMI_DISPATCH_H
