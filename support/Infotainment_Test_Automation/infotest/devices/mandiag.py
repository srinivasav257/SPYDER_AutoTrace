"""
=====================================================================================================================
File Name   : mandiag.py
Description : This File contains ManDiag test device class - This module sends diagnostic commands to the infotainment
              unit and processes responses.
Author      : Srinivasa V
Created On  : 2025-01-01

Copyright (C) 2025 APTIV - This file is part of the MnM HPCC project - Manufacture Diagnostics Component.
Manufacture Diagnostics Project is free software: you can redistribute it and/or modify it under the terms of Diagnostics
Department ASUX.

======================================================================================================================
"""

import sys
import time

import serial

from infotest.core.base_serial import BaseSerialDevice
from infotest.core.trace_logger import app_logger
from infotest.utilities.util_man_diag import parse_response


class Mandiag(BaseSerialDevice):
    """Class for sending diagnostic commands to the infotainment system."""

    # Constants for expected response patterns
    RESPONSE_PATTERN = "6D643C"
    DEFAULT_EXPECTED_STATUS = "01"
    DEFAULT_EXPECTED_DATA_LENGTH = "00"
    DEFAULT_EXPECTED_DATA = ""

    def __init__(self, port, baudrate=115200, timeout=1, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE,
                 bytesize=serial.EIGHTBITS, connect_retries=1, connect_delay=1.0):
        super().__init__(port, baudrate, timeout, parity, stopbits, bytesize, connect_retries, connect_delay)

    def _send_command_parse_response(self, command: str, pending_wait: int = 15, start_time: float = None) -> dict:
        """
        Sends a diagnostic command and returns the parsed response.
        Args:
            command (str): The command to send.
            pending_wait(int): max waiting time in case of pending response
        Returns:
            dict: Parsed response dictionary or None if unsuccessful.
        """

        if start_time is None:
            start_time = time.time()  # Initialize only once at the first call

        try:
            received_response = self.serial_write_data_and_await_for_response(command, pattern=self.RESPONSE_PATTERN, max_duration=5.0)
            if received_response is not None:
                response_data = parse_response(received_response)

                if response_data['Status'] == "01":
                    app_logger.debug(f"Command executed successfully for {command}")
                    return response_data

                if response_data['Status'] == "AA":
                    app_logger.info(f"Command responded with {response_data['Status']} status, retrying with 3 sec delay")
                    elapsed_time = time.time() - start_time
                    if elapsed_time >= pending_wait:
                        app_logger.info(f"Stopping retries after 15 sec for command {command}")
                        return response_data  # Stop recursion after 15 sec

                    # 5 sec gap for pending response (loop runs for 5 seconds)
                    for _ in range(10):
                        time.sleep(0.5)
                        sys.stdout.flush()  # Force flushing to prevent buffering issues

                    return self._send_command_parse_response(command, pending_wait, start_time)  # Recursively call with same start_time
                else:
                    app_logger.info(f"Max retires reached for command {command}")
                    return response_data

        except Exception as e:
            app_logger.error(f"Error while processing command '{command}', Error:  {e}")

        return {}

    def send_request_command_and_get_response(self, command: str, retries: int = 1, pending_wait: int = 15) -> dict:
        """
        Send a diagnostic command and retrieve the response.
        Args:
            command (str): The command to send.
            retries (int): Number of retry attempts (default: 3).
            pending_wait(int): max waiting time in case of pending response
        Returns:
            dict: Parsed response or None if unsuccessful.
        """
        response = {}
        for i in range(1, retries + 1):
            response = self._send_command_parse_response(command, pending_wait)
            if response and response["Status"] == "01":
                app_logger.debug(f"Validation Success - response received for command: {command} in attempt {i}")
                break
            else:
                app_logger.error(f"Validation failed - No response received for command: {command} in attempt {i}")

        app_logger.debug(f"Request  CMD: {command}")
        app_logger.debug(f"Response CMD: {response}")

        return response

    def send_request_command_and_validate_response(
            self, command: str, expected_data_length: str = DEFAULT_EXPECTED_DATA_LENGTH,
            expected_data: str = DEFAULT_EXPECTED_DATA, expected_status: str = DEFAULT_EXPECTED_STATUS, retries: int = 1, pending_wait: int = 15) -> bool:
        """
        Send a diagnostic command and validate the response against expected values.
        Args:
            command (str): The command to send.
            expected_data_length (str): Expected data length in the response.
            expected_data (str, optional): Expected data in the response.
            expected_status (str): Expected status in the response.
            retries (int): Number of retry attempts (default: 3).
            pending_wait(int): max waiting time in case of pending response
        Returns:
            bool: True if response is valid, False otherwise.
        """
        time.sleep(0.5)
        response = ""
        for i in range(1, retries + 1):
            response = self._send_command_parse_response(command, pending_wait)
            if response:
                app_logger.debug(f"Validation Success - response received for command: {command} in attempt {i}")
                break
            else:
                app_logger.debug(f"Validation failed - No response received for command: {command} in attempt {i}")

        # app_logger.debug(f"Request  CMD: {command}")
        # app_logger.debug(f"Response CMD: {response}")
        if response:
            return self._validate_mandiag_response(self, response, expected_status, expected_data_length, expected_data)
        else:
            return False

    @staticmethod
    def _validate_mandiag_response(self, response: dict, expected_status: str, expected_data_length: str, expected_data: str) -> bool:
        """
        Validate the diagnostic response against expected values.
        Args:
            response (dict): Parsed response dictionary.
            expected_status (str): Expected status in the response.
            expected_data_length (str): Expected data length in the response.
            expected_data (str, optional): Expected data in the response.
        Returns:
            bool: True if the response matches expectations, False otherwise.
        """
        try:
            status = response.get("Status")
            data_length = response.get("DataLength")
            data_bytes = response.get("Databytes")

            if status != expected_status:
                app_logger.error(f"Status mismatch - Received: {status}, Expected: {expected_status}")
                return False

            if data_length != expected_data_length:
                app_logger.error(f"Data length mismatch - Received: {data_length}, Expected: {expected_data_length}")
                return False

            if expected_data and data_length != "00" and data_bytes != expected_data:
                app_logger.error(f"Data bytes mismatch - Received: {data_bytes}, Expected: {expected_data}")
                return False

            app_logger.debug(f"Response validated successfully: {response}")
            return True

        except KeyError as e:
            app_logger.error(f"Missing expected key in response: {e}")
            return False


"""
Revision History:
|---------------------------------------------------------------------------------------|
| Version | Date       | Author            | Changes                                    |
|---------|------------|-------------------|--------------------------------------------|
| 1.0     | 2025-01-01 | Srinivas V        | Initial release                            |

"""
