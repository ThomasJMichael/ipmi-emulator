# 🧱 IPMC/IPMB Emulator Plan

---

## 🔧 Technologies

* **Language**: C (ANSI C for portability)
* **IPC**: UNIX domain sockets (bidirectional, reliable)
* **Platform**: WSL2 (Ubuntu preferred)
* **Build**: `gcc` + `make`
* **Testing**: Custom CLI or `ipmitool` later

---

# ✅ PHASE 1: Common Groundwork

### 🎯 Goal: Define your IPMI message structures and tools

1. **Create `ipmi_common.h` / `ipmi_common.c`**

   * Define structs for:

     ```c
     typedef struct {
         uint8_t rs_addr;
         uint8_t netfn_lun;
         uint8_t checksum1;
         uint8_t rq_addr;
         uint8_t rq_seq_lun;
         uint8_t cmd;
         uint8_t data[256];  // Max payload
         uint8_t checksum2;
         size_t data_len;
     } ipmi_msg_t;
     ```

   * Implement functions:

     * `uint8_t calc_checksum(uint8_t *data, size_t len)`
     * `int validate_ipmi_checksum(ipmi_msg_t *msg)`
     * `void build_ipmi_response(...)`

2. Create `ipmi_netfn.h` for command constants

   * NetFn values (App = 0x06, Sensor = 0x04, etc.)
   * Command codes (Get Device ID = 0x01, etc.)

---

# ✅ PHASE 2: IPMB Bus (Router Process)

### 🎯 Goal: Simulate the shared IPMB bus

1. Create `ipmb_bus.c`

   * Listen on a known UNIX socket path (e.g., `/tmp/ipmb_bus.sock`)
   * Accept N incoming connections (one per IPMC)
   * Store each with:

     ```c
     typedef struct {
         int sock_fd;
         uint8_t ipmb_addr;
     } ipmc_client_t;
     ```

2. In the main loop:

   * Use `select()` or `poll()` to monitor all connections
   * Read IPMI message from one client
   * Parse and route based on `rs_addr` field
   * Write full message to destination socket

3. Optional: Log every transfer

---

# ✅ PHASE 3: IPMC Instance(s)

### 🎯 Goal: Create one or more simulated IPMCs

1. Create `ipmc_instance.c`

   * Connect to `/tmp/ipmb_bus.sock`
   * Register itself by sending its IPMB address (e.g., `0x20`)
   * Loop:

     * Read messages from socket
     * Validate and parse IPMI message
     * Dispatch to appropriate command handler

2. Implement command handler table:

   ```c
   typedef struct {
       uint8_t netfn;
       uint8_t cmd;
       ipmi_msg_t (*handler)(ipmi_msg_t *);
   } ipmi_cmd_entry_t;
   ```

3. Implement `Get Device ID` handler:

   * Respond with:

     * IPMI version = 2.0
     * BMC version
     * Manufacturer ID (3 bytes)
     * Product ID (2 bytes)
   * Use proper NetFn+1 for response

---

# ✅ PHASE 4: Simple Host/Client (Optional)

### 🎯 Goal: Send requests like a shelf manager

1. Create `ipmi_client.c` or test script

   * Connect to bus like another IPMC
   * Send a `Get Device ID` to one IPMC
   * Print response

> Alternatively: simulate shelf manager logic inside bus later on

---

# ✅ PHASE 5: Multiple IPMCs

### 🎯 Goal: Simulate multiple boards/devices

1. Run multiple instances of `ipmc_instance` with different IPMB addresses (e.g., `0x22`, `0x24`)
2. Have them:

   * Respond to requests
   * Optionally send events or commands to each other

---

# ✅ PHASE 6: Expand with Real Features

1. Add **Sensor Data Repository (SDR)** support

   * Mock temp/voltage/fan sensors
   * Respond to `Get Sensor Reading`

2. Add **FRU data simulation**

   * In-memory FRU binary with headers + fields
   * Support `Read FRU Data`, `Get FRU Inventory Area Info`

3. Add **System Event Log (SEL)**

   * Log events in memory
   * Respond to `Get SEL Info`, `Get SEL Entry`, `Add SEL Entry`

4. Add **event triggers**

   * One IPMC sends SEL entry to another (simulate coordination)

---

# ✅ PHASE 7: Testing & Debugging

* Add verbose logs to all processes
* Use `hexdump` to analyze packet structures
* Implement CLI in host to send raw commands
* Later: use `ipmitool` + loopback bridge if needed

---

# 🔁 Execution Flow (Runtime)

```
[host/sim client] ----\
                      |
                  [ ipmb_bus ]
                      |
         +------------+------------+
         |            |            |
     [IPMC 0x20]  [IPMC 0x22]  [IPMC 0x24]
```

Each process has:

* A socket connection to the bus
* Its own IPMB address
* Command handlers
* Optional simulated sensors/FRUs/SEL

---

# 📦 Bonus: Script to Launch Everything

```bash
#!/bin/bash
# run.sh
gcc ipmb_bus.c -o bus
gcc ipmc_instance.c -o ipmc

./bus &

./ipmc 0x20 &
./ipmc 0x22 &
./ipmc 0x24 &
```

---

## ✅ Summary of What You'll Learn

| Topic                     | You'll Learn                                           |
| ------------------------- | ------------------------------------------------------ |
| IPMI Message format       | By writing the parser and handlers                     |
| IPMB protocol             | Through simulated address routing and messaging        |
| FRU, SDR, SEL             | By implementing responses from scratch                 |
| Multi-device coordination | Via message routing and responses                      |
| Debugging real systems    | Through packet tracing, validation, and event modeling |

---
