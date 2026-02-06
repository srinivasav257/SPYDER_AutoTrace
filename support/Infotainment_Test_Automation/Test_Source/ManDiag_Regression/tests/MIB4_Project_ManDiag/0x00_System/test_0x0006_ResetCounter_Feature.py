# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x0006_ResetCounter_Feature.py
# Author:       Mayur Padhariya
# Created:      Oct 16 2025
# Description:  This File contains Test case scenarios for performing GET/SET Reset Counter value.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2025 APTIV
#
# This file is part of the MIB4 project - Manufacture Diagnostics Component feature test Automation.
#
# Manufacture Diagnostics Project is free software: you can redistribute it and/or modify it under the terms
# of Diagnostics Department ASUX terms and conditions.
# ------------------------------------------------------------------------------------------------------------------

import time
import serial

from infotest.core.trace_logger import app_logger
import pytest

# ************************************************************* TEST CASES #01 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0006
@pytest.mark.order(41)
def test1_0x0006_Reset_Counter_Feature(ManDiag_component):
    """ IOC-PowerMode: Reset Counter Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: The counter is expected to increment in case of watch-dog triggered resets.
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Executing Set command SW will reset the g_sw_reset_counter to 0x00.
                3. Executing GET command, we will get to know the numbers of resets.

                Expected Result:
                - ManDiag reset counter functionality verified. GET and SET operations respond as expected.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: Watchdog monitoring OFF Operation feature Validation")
    SetCmd = "6D643E 00 06 00 01 00"
    GetCmd = "6D643E 00 06 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="00")

    # Step 3: Post Test Case
    app_logger.info("Executing Post Condition test: GET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01")

    app_logger.info("Test case passed successfully!")


