"""
=====================================================================================================================
File Name   : io_controller.py
Description : This File contains IO controller test device class - This module controls the IO controller serial switches
Author      : Srinivasa V
Created On  : 2025-01-01

Copyright (C) 2025 APTIV - This file is part of the MnM HPCC project - Manufacture Diagnostics Component.
Manufacture Diagnostics Project is free software: you can redistribute it and/or modify it under the terms of Diagnostics
Department ASUX.

======================================================================================================================
"""

from infotest.core.base_serial import BaseSerialDevice
from infotest.core.trace_logger import app_logger
from infotest.core.exception_handler import CommandExecutionError

class IOController(BaseSerialDevice):
    """ Class for controlling Arduino-based serial switches. """

    def switch_on(self, switch_name):
        """ Turns ON a specific switch. """
        command = f"{switch_name}_ON"
        return self.serial_write_data(command)

    def switch_off(self, switch_name):
        """ Turns OFF a specific switch. """
        command = f"{switch_name}_OFF"
        return self.serial_write_data(command)

"""
Revision History:
|---------------------------------------------------------------------------------------|
| Version | Date       | Author            | Changes                                    |
|---------|------------|-------------------|--------------------------------------------|
| 1.0     | 2025-01-01 | Srinivas V        | Initial release                            |

"""
