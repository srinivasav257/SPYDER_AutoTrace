"""
=====================================================================================================================
File Name   : util_can_helper.py
Description : Utility functions for CAN communication setup and debugging.
              Includes channel discovery for Vector CAN interfaces (VN1630A/VN1640A).
Author      : Srinivasa V
Created On  : 2025-10-08

Copyright (C) 2025 APTIV - This file is part of the MnM HPCC project - Manufacture Diagnostics Component.
=====================================================================================================================
"""

import can
from infotest.core.trace_logger import app_logger

def show_vector_configs():
    configs = can.detect_available_configs("vector")
    if not configs:
        print("No Vector CAN devices found.")
        return

    print(f"{'Idx':<4} {'Device':<12} {'Serial':<8} {'Channel':<8} {'Supports FD':<10} {'Transceiver'}")
    print("-"*70)
    for i, c in enumerate(configs):
        vcfg = c["vector_channel_config"]
        dev = vcfg.name.split()[0] if hasattr(vcfg, "name") else "Unknown"
        print(f"{i:<4} {dev:<12} {c.get('serial',''):<8} {c.get('channel',''):<8} "
              f"{str(c.get('supports_fd',False)):<10} {vcfg.transceiver_name}")

