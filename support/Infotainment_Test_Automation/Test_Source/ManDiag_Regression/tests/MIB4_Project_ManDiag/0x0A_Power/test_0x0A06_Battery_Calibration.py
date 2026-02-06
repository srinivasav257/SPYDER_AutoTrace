# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x0A06_Battery_Calibration.py
# Author:       Srinivasa V
# Created:      Jan 02 2025
# Description:  This File contains Test case scenarios for validating 0x0A06 Battery Calibration feature.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2024 APTIV
#
# This file is part of the MnM HPCC project - Manufacture Diagnostics Component.
#
# Manufacture Diagnostics Project is free software: you can redistribute it
# and/or modify it under the terms of Diagnostics Department ASUX.
# ------------------------------------------------------------------------------------------------------------------

import time

from infotest.core.trace_logger import app_logger
import pytest


# ************************************************************* TEST CASES #01 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0A06
@pytest.mark.order(151)
def test1_0x0A06_Battery_Calibration_FeatureTest(ManDiag_component, PowerSupply_TENMA):
    """ IOC Power mode : Battery_Calibration test 14.4V to 16.6V """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Battery Calibration feature
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET Battery Calibration enable to calibrate battery voltage.          
                3. Post-condition: Send GET command to verify Battery Calibration by equating calibrated voltage to actual voltage.

                Expected Result:
                - The response should indicate that the difference in calibrated voltage and actual voltage is minimal (<300mV).
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Battery Calibration enable test feature Validation")
    SetCmd = "6D643E 0A 06 01 00 01 01"
    GetCmd = "6D643E 0A 06 00 00 00"

    # Step 1: Pre_Condition Test
    app_logger.info("Executing Pre test: Set power Supply voltage to 14.4V ")
    PowerSupply_TENMA.set_voltage(14.6)

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET Battery calibration enable ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get Calibrated voltage value and compare with supplied ")
    time.sleep(3)
    response_data = ManDiag_component.send_request_command_and_get_response(GetCmd)
    if response_data["DataLength"] == "02" and response_data["Databytes"] != "00 00":
        hex_str = response_data["Databytes"]
        decimal_value = int(hex_str.replace(" ", ""), 16)
        supplied_voltage = 1000*PowerSupply_TENMA.get_voltage()
        app_logger.info(f"supplied Voltage {supplied_voltage} and Calibrated voltage {float(decimal_value)}")
        assert 300 > (supplied_voltage - decimal_value)
    else:
        assert False

    PowerSupply_TENMA.set_voltage(12.00)
    app_logger.info("Test case passed successfully!")
