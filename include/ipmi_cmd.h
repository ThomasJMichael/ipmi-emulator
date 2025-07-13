#ifndef IPMI_CMD_H
#define IPMI_CMD_H
#include <stdint.h>

/**
 * @file ipmi_cmd.h
 * @brief Defines IPMI command codes grouped by NetFn.
 */

/* -----------------------------------------------
 * Chassis Commands (NetFn = 0x00 / 0x01)
 * --------------------------------------------- */
typedef enum {
  IPMI_CMD_GET_CHASSIS_STATUS = 0x01,
  IPMI_CMD_CHASSIS_CONTROL = 0x02,
  IPMI_CMD_CHASSIS_RESET = 0x03,
} ipmi_cmd_chassis_t;

/* -----------------------------------------------
 * Bridge Commands (NetFn = 0x02 / 0x03)
 * --------------------------------------------- */
typedef enum {
  IPMI_CMD_GET_BRIDGE_STATE = 0x00,
  IPMI_CMD_SET_BRIDGE_STATE = 0x01,
} ipmi_cmd_bridge_t;

/* -----------------------------------------------
 * Sensor/Event Commands (NetFn = 0x04 / 0x05)
 * --------------------------------------------- */
typedef enum {
  IPMI_CMD_GET_SENSOR_READING = 0x2D,
  IPMI_CMD_GET_SENSOR_THRESHOLDS = 0x27,
} ipmi_cmd_sensor_t;

/* -----------------------------------------------
 * Application Commands (NetFn = 0x06 / 0x07)
 * --------------------------------------------- */
typedef enum {
  IPMI_CMD_GET_DEVICE_ID = 0x01,
  IPMI_CMD_COLD_RESET = 0x02,
  IPMI_CMD_WARM_RESET = 0x03,
  IPMI_CMD_GET_SELF_TEST_RESULTS = 0x04,
  IPMI_CMD_SET_ACPI_POWER_STATE = 0x06,
  IPMI_CMD_GET_ACPI_POWER_STATE = 0x07,
} ipmi_cmd_app_t;

/* -----------------------------------------------
 * Storage Commands (NetFn = 0x0A / 0x0B)
 * --------------------------------------------- */
typedef enum {
  IPMI_CMD_GET_FRU_INVENTORY_AREA = 0x10,
  IPMI_CMD_READ_FRU_DATA = 0x11,
  IPMI_CMD_GET_SEL_INFO = 0x40,
  IPMI_CMD_GET_SEL_ENTRY = 0x43,
} ipmi_cmd_storage_t;

/* -----------------------------------------------
 * Transport Commands (NetFn = 0x0C / 0x0D)
 * --------------------------------------------- */
typedef enum {
  IPMI_CMD_SET_LAN_CONFIG_PARAMS = 0x01,
  IPMI_CMD_GET_LAN_CONFIG_PARAMS = 0x02,
  IPMI_CMD_SOL_ACTIVATION = 0x20,
  IPMI_CMD_GET_SOL_CONFIG_PARAMS = 0x21,
} ipmi_cmd_transport_t;

/* -----------------------------------------------
 * OEM Commands (NetFn = 0x30–0x3F range)
 * --------------------------------------------- */
// OEM commands vary by vendor — reserved for future
#endif // IPMI_CMD_H
