# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x0A06_BatteryCalibration_BatteryVoltage.py
# Author:       Srinivasa V
# Created:      Aug 29 2025
# Description:  This File contains Test case scenarios for validating 0x0A06 Battery Calibration Battery Voltage feature.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2024 APTIV
#
# This file is part of the Manufacture Diagnostics Component.
#
# Manufacture Diagnostics Project is free software: you can redistribute it
# and/or modify it under the terms of Diagnostics Department ASUX.
# ------------------------------------------------------------------------------------------------------------------

import time

from infotest.core.trace_logger import app_logger
import pytest


# ************************************************************* TEST CASES #01 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(7)
def test1_0x0A06_BatteryCalibration_BatteryVoltage(ManDiag_component):
    """ SET Battery Calibration to Supply test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate SET Battery Calibration to Supply
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                                  14.4V BATT Voltage to be supplied to the UUT.
                2. Actual Test: Send SET power Enable/Disable command to SET Battery Calibration to Supply.          
                3. Post-condition: Send GET command to read the present Battery Calibration value set in Unit.
                Expected Result:
                - The response should indicate that UUT reported voltage is similar to the voltage supplied to UUT.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: SET Battery Calibration to Supply test feature Validation")
    SetCmd = "6D643E 0A 06 01 00 01 01"
    GetCmd = "6D643E 0A 06 00 00 01 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET Battery Calibration to Supply ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01",  expected_data="01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get present Battery Calibration value set in Unit ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", retries=2)

    app_logger.info("Test case passed successfully!")
