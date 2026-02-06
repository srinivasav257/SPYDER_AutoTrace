"""
=====================================================================================================================
File Name   : base_serial.py
Description : This File contains Serial UART test device deriver class - This module will wites and read data to/from
              serialPort.
Author      : Srinivasa V
Created On  : 2025-01-01

Copyright (C) 2025 APTIV - This file is part of the MnM HPCC project - Manufacture Diagnostics Component.
Manufacture Diagnostics Project is free software: you can redistribute it and/or modify it under the terms of Diagnostics
Department ASUX.

======================================================================================================================
"""

import re

import serial
import time
import serial.tools.list_ports

from infotest.core.trace_logger import app_logger
from infotest.core.exception_handler import SerialConnectionError, CommandExecutionError, SerialDataReadError


class BaseSerialDevice:
    """
    Abstract Base Class for Serial Communication.

    Provides:
    - Automatic checks to ensure the port is available.
    - Configurable serial parameters (baud, parity, stopbits, bytesize, etc.).
    - Logging for debugging and traceability.
    """

    def __init__(self,
                 port,
                 baudrate=9600,
                 timeout=1,
                 parity=serial.PARITY_NONE,
                 stopbits=serial.STOPBITS_ONE,
                 bytesize=serial.EIGHTBITS,
                 connect_retries=1,
                 connect_delay=1.0):
        """
        Initializes a serial connection.
        params:
             port: Serial port (e.g., "/dev/ttyUSB0" or "COM3").
             baudrate: Baud rate for communication (default 9600).
             timeout: Read timeout in seconds (default 1s).
             parity: Parity for communication (default none).
             stopbits: Stop bits for communication (default 1).
             bytesize: Data bits (default 8).
             connect_retries: Number of retries if connection fails (default 1).
             connect_delay: Delay between retries in seconds (default 1.0).
        """
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.parity = parity
        self.stopbits = stopbits
        self.bytesize = bytesize
        self.connect_retries = connect_retries
        self.connect_delay = connect_delay

        self.serial_conn = None
        self._connect()

    def _connect(self):
        """
        Establishes a serial connection with optional retries and port checks.
        """
        # Check if the port is in the list of available ports
        available = self.get_available_ports()
        if self.port not in available:
            app_logger.warning(f"Requested port {self.port} not found in available ports: {available}")

        for attempt in range(1, self.connect_retries + 1):
            try:
                self.serial_conn = serial.Serial(
                    port=self.port,
                    baudrate=self.baudrate,
                    timeout=self.timeout,
                    parity=self.parity,
                    stopbits=self.stopbits,
                    bytesize=self.bytesize
                )
                app_logger.debug(
                    f"[Attempt {attempt}] Connected to {self.port} at {self.baudrate} baud. "
                    f"(Parity={self.parity}, Stopbits={self.stopbits}, Bytesize={self.bytesize})"
                )
                return  # Successful connection
            except serial.SerialException as e:
                app_logger.error(f"[Attempt {attempt}] Failed to connect: {e}")
                if attempt < self.connect_retries:
                    app_logger.info(f"Retrying in {self.connect_delay} seconds...")
                    time.sleep(self.connect_delay)

        # If we reach here, all retries failed
        raise SerialConnectionError(
            f"Could not connect to device on port {self.port} after {self.connect_retries} attempts."
        )

    def serial_write_data(self, write_data: str) -> bool:
        """
        Sends a command via the serial port and returns the raw response.

        :param write_data: The data string to be sent.
        :return: The device's response as a string.
        :raises: CommandExecutionError if sending or reading fails.
        """
        try:
            if not self.serial_conn or not self.serial_conn.is_open:
                raise CommandExecutionError(f"Serial port {self.port} is not open.")

            self.serial_conn.write((write_data + "\r\n").encode("ascii"))
            # time.sleep(0.1)  # Brief delay for device processing
            return True

        except serial.SerialException as e:
            app_logger.error(f"Serial command failed: {write_data} => {e}")
            raise CommandExecutionError(f"Failed to execute command: {write_data}") from e

    def serial_read_data(self) -> str:
        """
        Sends a command via the serial port and returns the raw response.

        :param command: The command string to be sent.
        :return: The device's response as a string.
        :raises: CommandExecutionError if sending or reading fails.
        """
        try:
            if not self.serial_conn or not self.serial_conn.is_open:
                raise CommandExecutionError(f"Serial port {self.port} is not open.")

            # Read one line
            data_received = self.serial_conn.readline().decode(errors="ignore").strip()
            app_logger.debug(f"Data : {data_received}")
            return data_received

        except serial.SerialException as e:
            app_logger.error(f"Serial Data Read failed: {e}")
            raise SerialDataReadError(f"Failed to read serial Data") from e

    def serial_write_data_and_await_for_response(self, command, pattern="OK", max_duration=5.0):
        """
        Sends a command and waits for a certain pattern in continuous logs.
        Returns the matched line if found, else None.
        """
        self.serial_write_data(command)
        return self.serial_read_until_pattern(pattern=pattern, max_duration=max_duration)

    def serial_read_until_pattern(self, pattern, max_duration=5.0, regex=False):
        """
        Continuously reads lines until a certain pattern is found or time runs out.

        :param pattern: Substring or regex pattern to search for.
        :param max_duration: How long (seconds) to keep reading before giving up.
        :param regex: If True, treat 'pattern' as a regular expression; otherwise, use substring matching.

        :return: The matching line if found, or None if not found within max_duration.
        """
        start_time = time.time()
        while (time.time() - start_time) < max_duration:
            try:
                line = self.serial_conn.readline().decode(errors="ignore").strip()
                if line:
                    app_logger.debug(f"LogLine: {line}")

                    # Check if line matches the pattern
                    if regex:
                        if re.search(pattern, line):
                            app_logger.debug(f"Matched pattern '{pattern}' in line: {line}")
                            return line
                    else:
                        if pattern in line:
                            app_logger.debug(f"Found substring '{pattern}' in line: {line}")
                            return line

            except serial.SerialException as e:
                app_logger.error(f"Error reading line: {e}")
                break

        app_logger.warning(f"No match for pattern '{pattern}' within {max_duration} seconds.")
        return None

    def close(self):
        """
        Closes the serial connection if it's open.
        """
        if self.serial_conn and self.serial_conn.is_open:
            self.serial_conn.close()
            app_logger.info(f"Closed connection to {self.port}")

    def __del__(self):
        self.close()

    @staticmethod
    def get_available_ports():
        """
        Returns a list of available serial ports on the system.
        """
        ports = serial.tools.list_ports.comports()
        return [port.device for port in ports]

"""
Revision History:
|---------------------------------------------------------------------------------------|
| Version | Date       | Author            | Changes                                    |
|---------|------------|-------------------|--------------------------------------------|
| 1.0     | 2025-01-01 | Srinivas V        | Initial release                            |

"""
