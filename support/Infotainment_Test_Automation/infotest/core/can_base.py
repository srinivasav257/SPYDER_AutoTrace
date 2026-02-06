"""
=====================================================================================================================
File Name   : can_base.py
Description : Base CAN interface class for Vector CANoe hardware (VN1630A/VN1640A).
              Provides methods for initializing, reading, writing, and closing CAN communication channels.
Author      : Srinivasa V
Created On  : 2025-10-08

Copyright (C) 2025 APTIV - This file is part of the MnM HPCC project - Manufacture Diagnostics Component.
=====================================================================================================================
"""

import time
from typing import Optional, List, Union
from infotest.core.trace_logger import app_logger
from infotest.core.exception_handler import DeviceNotFoundError, CommandExecutionError

try:
    import can  # python-can library
except ImportError as e:
    app_logger.error("python-can library not found. Install using: pip install python-can")
    raise


class CANBase:
    """
    CANBase provides a generic API for accessing Vector CAN interfaces (VN1630A/VN1640A).

    ✅ Features:
    - Opens CAN channel automatically in constructor.
    - Graceful close handled in destructor.
    - Provides read/write helpers for CAN message communication.
    - Integrated with existing logging and exception system.
    """

    def __init__(self,
                 channel: Union[int, str] = 0,
                 bitrate: int = 500000,
                 data_bitrate: int = 2000000,
                 interface: str = "vector",
                 fd: bool = False,
                 receive_own_messages: bool = False):
        """
        Initialize CAN channel using Vector interface.

        :param channel: CAN channel index (0, 1, etc.) or name ("0", "1").
        :param bitrate: CAN bus speed (default: 500000 bps).
        :param interface: Backend driver (default: 'vector').
        :param receive_own_messages: Whether to receive self-transmitted frames.
        """
        self.channel = channel
        self.bitrate = bitrate
        self.interface = interface
        self.receive_own_messages = receive_own_messages
        self.bus = None
        self.data_bitrate = data_bitrate
        self.fd = fd
        self._open_channel()

    def _open_channel(self):
        """Opens the CAN interface."""
        try:
            self.bus = can.interface.Bus(
                channel=self.channel,
                interface=self.interface,
                bitrate=self.bitrate,
                data_bitrate=self.data_bitrate,
                receive_own_messages=self.receive_own_messages,
                fd=True
            )
            app_logger.info(f"Opened CAN interface '{self.interface}' on channel {self.channel} @ {self.bitrate} bps")

        except Exception as e:
            app_logger.error(f"Failed to open CAN channel {self.channel}: {e}")
            raise DeviceNotFoundError(f"Failed to open CAN interface '{self.interface}' channel {self.channel}") from e

    # -----------------------------------------------------------------------------------------------------
    # Message Transmission and Reception
    # -----------------------------------------------------------------------------------------------------

    def send_message(self, can_id: int, data: List[int], extended_id: bool = False):
        """
        Sends a CAN message with the given ID and data.

        :param can_id: Arbitration ID of the CAN message.
        :param data: List of byte values (0–255).
        :param extended_id: Whether to use extended frame format.
        """
        try:
            msg = can.Message(arbitration_id=can_id, data=data, is_extended_id=extended_id)
            self.bus.send(msg)
            app_logger.debug(f"TX >> ID={hex(can_id)} Data={data}")
            return True
        except Exception as e:
            app_logger.error(f"CAN TX failed for ID={hex(can_id)}: {e}")
            raise CommandExecutionError(f"Failed to send CAN message with ID={hex(can_id)}") from e

    def read_message_timeout(self, timeout: float = 1.0) -> Optional[dict]:
        """
        Reads a CAN message from the bus.

        :param timeout: Timeout in seconds for waiting a message.
        :return: Dictionary with ID, data, timestamp; None if timeout.
        """
        try:
            msg = self.bus.recv(timeout=timeout)
            if msg:
                msg_dict = {
                    "timestamp": msg.timestamp,
                    "can_id": hex(msg.arbitration_id),
                    "data": list(msg.data),
                    "dlc": msg.dlc,
                    "is_extended_id": msg.is_extended_id,
                }
                app_logger.debug(f"RX << {msg_dict}")
                return msg_dict
            else:
                app_logger.warning(f"No CAN message received within {timeout} sec.")
                return None

        except Exception as e:
            app_logger.error(f"CAN RX failed: {e}")
            raise CommandExecutionError("Failed to read CAN message.") from e

    def read_until_id(self, target_id: int, timeout: float = 5.0) -> Optional[dict]:
        """
        Continuously reads messages until a specific CAN ID is received or timeout occurs.
        """
        start_time = time.time()
        while time.time() - start_time < timeout:
            msg = self.read_message_timeout(timeout=0.1)
            if msg and msg["can_id"] == hex(target_id):
                app_logger.debug(f"Matched CAN ID {hex(target_id)}")
                return msg
        app_logger.warning(f"Timeout waiting for CAN ID {hex(target_id)}")
        return None

    # -----------------------------------------------------------------------------------------------------
    # Cleanup and Context Management
    # -----------------------------------------------------------------------------------------------------

    def close(self):
        """Closes the CAN bus."""
        try:
            if self.bus:
                self.bus.shutdown()
                app_logger.info(f"Closed CAN channel {self.channel}")
        except Exception as e:
            app_logger.warning(f"Error while closing CAN interface: {e}")

    def __enter__(self):
        """Context manager entry."""
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.close()
        return False

    def __del__(self):
        """Destructor for automatic cleanup."""
        self.close()


if __name__ == "__main__":
    can_device = CANBase(channel=3)

    can_device.manual_send_message(can_id=0x1B000010, data=[0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF], extended_id=True)
    can_device.read_message_timeout(5)

    can_device.close()
    app_logger.info("Closed CAN Interface after test session.")
"""
Revision History:
|---------------------------------------------------------------------------------------|
| Version | Date       | Author            | Changes                                    |
|---------|------------|-------------------|--------------------------------------------|
| 1.0     | 2025-10-08 | Srinivas V        | Initial release                            |
|---------------------------------------------------------------------------------------|
"""
