#ifndef IPMI_NETFN_H
#define IPMI_NETFN_H

// Extract just the NetFn (Upper 6 bits)
#define IPMI_GET_NETFN(netfn_lun) ((uint8_t)(netfn_lun >> 2))

// Extract just the LUN (Lower 2 bits)
#define IPMI_GET_LUN(netfn_lun) ((uint8_t)(netfn_lun & 0x3))

// Compose the netfn_lun from netfn + lun
#define IPMI_MAKE_NETFN_LUN(netfn, lun) (((netfn) << 2) | ((lun) & 0x03))

// Determine if the request is a response (netfn is odd)
#define IPMI_IS_RESPONSE(netfn_lun) (IPMI_GET_NETFN(netfn_lun) & 0x01)
/**
 * @enum ipmi_netfn_t
 * @brief IPMI Network Function (NetFn) codes.
 *
 * These values define the category of commands for IPMI message dispatching.
 * All values are for request NetFns (even numbers). Response NetFns are +1.
 */
typedef enum {
  IPMI_NETFN_CHASSIS = 0x00,   // Chassis commands
  IPMI_NETFN_BRIDGE = 0x02,    // Bridging commands
  IPMI_NETFN_SENSOR = 0x04,    // Sensor/Event commands
  IPMI_NETFN_APP = 0x06,       // Application (e.g. Get Device ID)
  IPMI_NETFN_FIRMWARE = 0x08,  // Firmware updates
  IPMI_NETFN_STORAGE = 0x0A,   // SDR, SEL, FRU access
  IPMI_NETFN_TRANSPORT = 0x0C, // Serial, LAN, SOL
  IPMI_NETFN_GROUP_EXT = 0x2C, // Group Extension (OEM)
  IPMI_NETFN_OEM = 0x30,       // OEM-specific commands
} ipmi_netfn_t;

#endif // IPMI_NETFN_H
