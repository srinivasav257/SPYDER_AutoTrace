# Vector XL Library Driver

This folder contains the Vector XL Library driver files for CAN bus communication.

## Setup

Download the **Vector XL Library Driver** from [Vector Downloads](https://www.vector.com/int/en/products/products-a-z/libraries-drivers/xl-driver-library/).

After installation, copy the following files from the Vector XL Library SDK
(typically installed to `C:\Users\Public\Documents\Vector\XL Driver Library\`):

### Headers (`include/`)
- `vxlapi.h` — Main XL Library API header

### Libraries (`lib/`)
- `vxlapi.lib` — Import library (from `bin/` in the SDK)

### Binaries (`bin/`)
- `vxlapi.dll` — Runtime DLL (also found in `C:\Windows\System32\` if drivers are installed)
- `vxlapi64.dll` — 64-bit runtime DLL

## Notes

- The CANManager module uses **runtime DLL loading** (`QLibrary`), so the application
  will compile and run even if Vector drivers are not installed. CAN functionality
  simply won't be available.
- The `vxlapi.h` provided here is a compatible subset for compilation.
  Replace it with the official header from the Vector SDK for full API coverage.
- Requires Vector driver software installed on the target machine for actual HW access.
