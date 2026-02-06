"""
=====================================================================================================================
File Name   : mib4_can_interface.py
Description : Derived CAN device class for MIB4 Infotainment or HPCC systems.
              Provides feature-specific APIs to send/receive CAN messages through VN1630A/VN1640A Vector hardware.
Author      : Srinivasa V
Created On  : 2025-10-08

Copyright (C) 2025 APTIV - This file is part of the MnM HPCC project - Manufacture Diagnostics Component.
=====================================================================================================================
"""

import time
from typing import List, Optional

from infotest.core.can_base import CANBase
from infotest.core.trace_logger import app_logger
from infotest.core.exception_handler import CommandExecutionError


class Vector_CAN_Interface(CANBase):
    """
    MIB4_CAN_Interface extends CANBase to provide MIB4/HPCC-specific CAN message handling.

    ✅ Features:
    - Provides high-level send/receive wrappers for diagnostic CAN messages.
    - Supports filtering, validation, and formatted data extraction.
    - Integrates with existing logger and exception handler.
    """

    DEFAULT_RESPONSE_TIMEOUT = 2.0

    def __init__(self,
                 channel: int = 0,
                 bitrate: int = 500000,
                 interface: str = "vector",
                 receive_own_messages: bool = False):
        super().__init__(
            channel=channel,
            bitrate=bitrate,
            interface=interface,
            receive_own_messages=receive_own_messages,
        )
        app_logger.info("Initialized Vector CAN_Interface successfully.")

    # -------------------------------------------------------------------------------------------------------------
    # CAN Message Helpers
    # -------------------------------------------------------------------------------------------------------------

    def send_diagnostic_request(self, can_id: int, data: Optional[List[int]], extended_id: bool = False) -> bool:
        """
        Sends a diagnostic request frame to the ECU.

        :param can_id: Arbitration ID of the request (e.g., 0x7E0).
        :param data: Diagnostic payload as list of bytes.
        :param extended_id: Whether to use extended frame format.
        """
        payload = data or []
        formatted_payload = " ".join(f"0x{byte:02X}" for byte in payload) or "<empty>"
        app_logger.info(f"Sending diagnostic request to ID {hex(can_id)} with data [{formatted_payload}]")
        return self.send_message(can_id, payload, extended_id)

    def receive_diagnostic_response(self,
                                    expected_id: int,
                                    timeout: float = DEFAULT_RESPONSE_TIMEOUT) -> Optional[dict]:
        """
        Waits for a response from the ECU with the given CAN ID.

        :param expected_id: Expected CAN response ID (e.g., 0x7E8).
        :param timeout: Timeout in seconds.
        :return: Message dict if response received, else None.
        """
        app_logger.debug(f"Waiting for response from ID {hex(expected_id)} (timeout={timeout}s)")
        return self.read_until_id(expected_id, timeout=timeout)

    def send_and_receive_diag(self,
                              request_id: int = 0x1735A107,
                              response_id: int = 0x1735A507,
                              request_data: Optional[List[int]] = None,
                              extended_id: bool = False,
                              timeout: float = DEFAULT_RESPONSE_TIMEOUT) -> Optional[dict]:
        """
        Sends a diagnostic CAN request and waits for the corresponding response.

        :param request_id:  MOL ManDiag ID for request (e.g., 0x1735A107).
        :param response_id: MOL ManDiag response ID (e.g., 0x1735A507).
        :param request_data: List of bytes for diagnostic payload.
        :param extended_id: Whether extended CAN frame is used.
        :param timeout: Timeout for waiting response.
        :return: Parsed response dict or None.
        """
        try:
            self.send_diagnostic_request(request_id, request_data, extended_id)
            response = self.receive_diagnostic_response(response_id, timeout)
            if response:
                app_logger.debug(f"Response from ECU {hex(response_id)}: {response}")
                return response
            else:
                app_logger.warning(f"No response from ECU {hex(response_id)} within {timeout} seconds")
                return None
        except Exception as e:
            app_logger.error(f"Diagnostic exchange failed: {e}")
            raise CommandExecutionError("Diagnostic request/response transaction failed.") from e

    def send_and_validate_response(self,
                                   request_id: int = 0x1735A107,
                                   response_id: int = 0x1735A507,
                                   request_data: Optional[List[int]] = None,
                                   expected_data: Optional[List[int]] = None,
                                   extended_id: bool = False,
                                   timeout: float = DEFAULT_RESPONSE_TIMEOUT) -> bool:
        """
        Sends a diagnostic CAN request and waits for the corresponding response.

        :param request_id:  MOL ManDiag ID for request (e.g., 0x1735A107).
        :param response_id: MOL ManDiag response ID (e.g., 0x1735A507).
        :param request_data: List of bytes for diagnostic request payload.
        :param expected_data: List of bytes for diagnostic response payload.
        :param extended_id: Whether extended CAN frame is used.
        :param timeout: Timeout for waiting response.
        :return: Parsed response dict or None.
        """
        try:
            self.send_diagnostic_request(request_id, request_data, extended_id)
            response = self.receive_diagnostic_response(response_id, timeout)
            if response:
                app_logger.debug(f"Response from ECU {hex(response_id)}: {response}")
                if expected_data is not None and response['data'] != expected_data:
                    app_logger.error(f"Response data {response['data']} does not match expected {expected_data}")
                    return False
                else:
                    formatted_payload = " ".join(f"0x{byte:02X}" for byte in response['data']) or "<empty>"
                    app_logger.info(f"Sending diagnostic request to ID {hex(response_id)} with data [{formatted_payload}]")
                    return True
            else:
                app_logger.warning(f"No response from ECU {hex(response_id)} within {timeout} seconds")
                return False
        except Exception as e:
            app_logger.error(f"Diagnostic exchange failed: {e}")
            raise CommandExecutionError("Diagnostic request/response transaction failed.") from e

    # -------------------------------------------------------------------------------------------------------------
    # Utility Functions
    # -------------------------------------------------------------------------------------------------------------

    @staticmethod
    def build_can_data(hex_string: str) -> List[int]:
        """
        Converts a space-separated hex string (like '10 03 22 F1 90') into byte list.

        :param hex_string: String with space-separated hex values.
        :return: List of integer bytes.
        """
        try:
            return [int(x, 16) for x in hex_string.strip().split()]
        except ValueError:
            raise ValueError(f"Invalid hex string: {hex_string}")

    @staticmethod
    def format_response_data(msg: dict) -> str:
        """
        Formats CAN response data nicely for display/logging.
        """
        if not msg:
            return "No Response"
        data_bytes = " ".join(f"{x:02X}" for x in msg["data"])
        return f"ID={msg['can_id']}  DLC={msg['dlc']}  Data=[{data_bytes}]"

    # -------------------------------------------------------------------------------------------------------------
    # Example Feature APIs (customize per project feature)
    # -------------------------------------------------------------------------------------------------------------

    def request_ecu_id(self) -> Optional[str]:
        """
        Example API: Read ECU identification via standard UDS service (0x22 F1 90)
        """
        req_data = [0x03, 0x22, 0xF1, 0x90]
        resp = self.send_and_receive_diag(0x7E0, 0x7E8, req_data)
        if resp and len(resp["data"]) > 3:
            ecu_id = "".join(chr(b) for b in resp["data"][3:])
            app_logger.info(f"ECU ID: {ecu_id}")
            return ecu_id
        return None


"""
Revision History:
|---------------------------------------------------------------------------------------|
| Version | Date       | Author            | Changes                                    |
|---------|------------|-------------------|--------------------------------------------|
| 1.0     | 2025-10-08 | Srinivas V        | Initial release                            |
|---------------------------------------------------------------------------------------|
"""
