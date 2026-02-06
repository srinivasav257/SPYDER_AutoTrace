"""
=====================================================================================================================
File Name   : power_supply.py
Description : This File contains TENMA72-2540 Power Control test device class - This module controls the programmable
              power supply
Author      : Srinivasa V
Created On  : 2025-01-01

Copyright (C) 2025 APTIV - This file is part of the MnM HPCC project - Manufacture Diagnostics Component.
Manufacture Diagnostics Project is free software: you can redistribute it and/or modify it under the terms of Diagnostics
Department ASUX.

======================================================================================================================
"""

import time
from infotest.core.base_serial import BaseSerialDevice
from infotest.core.trace_logger import app_logger
from infotest.core.exception_handler import CommandExecutionError


class TenmaPowerSupply(BaseSerialDevice):
    """ Class for controlling the TENMA power supply. """

    def power_on(self):
        """   Turns the power supply ON. (OUT1)   """
        command = "OUT1"
        return self.serial_write_data(command)

    def power_off(self):
        """   Turns the power supply OFF. (OUT0)   """
        command = "OUT0"
        return self.serial_write_data(command)

    def restart_power(self, delay=1):
        """    Restarts the power supply (OFF -> wait -> ON).  """
        try:
            self.power_off()
            app_logger.info("Power supply turned OFF. Waiting %s seconds...", delay)
            time.sleep(delay)
            self.power_on()
            app_logger.info("Power supply turned ON.")
            return True
        except CommandExecutionError as e:
            app_logger.error(f"Power restart failed: {e}")
            return False

    """ Returns true on Success, False on Failure"""

    def set_voltage(self, voltage, channel=1):
        command = f"VSET{channel}:{voltage}"
        return self.serial_write_data(command)

    """ Returns Voltage in Float 2 decimal points"""

    def get_voltage(self, channel=1):
        if self.serial_write_data(f"VOUT{channel}?"):
            response = float(self.serial_read_data())
            return response

    def set_current(self, current, channel=1):
        return self.serial_write_data(f"ISET{channel}:{current}")

    def get_current(self, channel=1):
        if self.serial_write_data(f"IOUT{channel}?"):
            response = float(self.serial_read_data())
            return response

    def get_voltage_setting(self, channel=1):
        if self.serial_write_data(f"VSET{channel}?"):
            response = float(self.serial_read_data())
            return response

    def get_current_setting(self, channel=1):
        if self.serial_write_data(f"ISET{channel}?"):
            response = float(self.serial_read_data())
            return response

    def set_beep(self, enable: bool):
        cmd = "BEEP1" if enable else "BEEP0"
        return self.serial_write_data(cmd)

    def set_output(self, enable: bool):
        cmd = "OUT1" if enable else "OUT0"
        return self.serial_write_data(cmd)

"""    
    def get_status(self):
        if self.serial_write_data("STATUS?"):
            response = self.serial_read_data()
            return response

    def get_identity(self):
        return self.serial_write_data("*IDN?")

    def recall_setting(self, index: int):
        return self.serial_write_data(f"RCL{index}")

    def save_setting(self, index: int):
        return self.serial_write_data(f"SAV{index}")

    def set_ocp(self, enable: bool):
        cmd = "OCP1" if enable else "OCP0"
        return self.serial_write_data(cmd)

    def set_ovp(self, enable: bool):
        cmd = "OVP1" if enable else "OVP0"
        return self.serial_write_data(cmd)
"""
# Example usage
if __name__ == "__main__":
    TenmaObj = TenmaPowerSupply("COM107")

    TenmaObj.set_current(5.8)
    TenmaObj.set_voltage(14.8)
    print(f"voltage_f :{float(TenmaObj.get_voltage())}")
    print(f"current_f{float(TenmaObj.get_current())}")

    TenmaObj.power_on()
    time.sleep(5)

    TenmaObj.set_voltage(12.50)
    print(f"voltage_f :{float(TenmaObj.get_voltage())}")
    print(f"current_f{float(TenmaObj.get_current())}")
    time.sleep(5)

    TenmaObj.power_off()

    del TenmaObj

"""
Revision History:
|---------------------------------------------------------------------------------------|
| Version | Date       | Author            | Changes                                    |
|---------|------------|-------------------|--------------------------------------------|
| 1.0     | 2025-01-01 | Srinivas V        | Initial release                            |

"""
