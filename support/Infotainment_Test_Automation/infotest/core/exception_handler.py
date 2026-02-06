"""
=====================================================================================================================
File Name   : exception_handler.py
Description : Defines custom exception classes for serial and diagnostic framework.
              Each exception includes context-rich string formatting for better readability in logs.
Author      : Srinivasa V (Seenu)
Created On  : 2025-01-01

Copyright (C) 2025 APTIV - MnM HPCC project
======================================================================================================================
"""

class SerialConnectionError(Exception):
    """Raised when a serial device fails to connect."""

    def __init__(self, message: str, port: str = None, retries: int = None):
        context = f" (Port: {port})" if port else ""
        attempt_info = f" after {retries} attempts" if retries else ""
        super().__init__(f"{message}{context}{attempt_info}")

    def __str__(self):
        return f"SerialConnectionError: {self.args[0]}"


class CommandExecutionError(Exception):
    """Raised when a serial command execution fails."""

    def __init__(self, command: str = "", message: str = ""):
        details = f"Command '{command}' failed. {message}".strip()
        super().__init__(details)

    def __str__(self):
        return f"CommandExecutionError: {self.args[0]}"


class SerialDataReadError(Exception):
    """Raised when reading serial data fails."""

    def __init__(self, message: str = "Failed to read data from serial port"):
        super().__init__(message)

    def __str__(self):
        return f"SerialDataReadError: {self.args[0]}"


class DeviceNotFoundError(Exception):
    """Raised when a requested device is not available."""

    def __init__(self, device_name: str = "Unknown", message: str = ""):
        details = f"{device_name} not found. {message}".strip()
        super().__init__(details)

    def __str__(self):
        return f"DeviceNotFoundError: {self.args[0]}"


"""
Revision History:
|---------------------------------------------------------------------------------------|
| Version | Date       | Author            | Changes                                    |
|---------|------------|-------------------|--------------------------------------------|
| 1.0     | 2025-01-01 | Srinivas V        | Initial release                            |
| 1.1     | 2025-10-05 | Srinivas V        | Added formatted messages and context info  |
|---------------------------------------------------------------------------------------|
"""
