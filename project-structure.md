🧱 Project Structure: IPMC Emulator with IPMB Bus
Components:
    ipmb_bus (router process):
        Central message router (like I²C bus)
        Forwards IPMI packets based on IPMB RsAddr
        Manages sockets for each IPMC instance

    ipmc_instance (per device):
        Emulates a real IPMC on a VPX/VME board
        Owns a unique IPMB address (e.g., 0x20)
        Parses, handles, and responds to IPMI requests
        Supports basic commands (starts with Get Device ID)

    ipmi_client (host simulator):
        Sends IPMI requests (or use raw ipmitool)
        Can simulate a shelf manager sending messages to IPMCs

📂 Directory Layout
```
  ipmi-emulator/
  ├── Makefile
  ├── ipmb_bus.c         # Shared IPMB message router
  ├── ipmc_instance.c    # Simulated IPMC device
  ├── ipmi_common.h/.c   # IPMI types, checksums, message formats
  ├── ipmi_client.c      # (optional) test client
  └── run.sh             # Launches N IPMCs + bus
```
