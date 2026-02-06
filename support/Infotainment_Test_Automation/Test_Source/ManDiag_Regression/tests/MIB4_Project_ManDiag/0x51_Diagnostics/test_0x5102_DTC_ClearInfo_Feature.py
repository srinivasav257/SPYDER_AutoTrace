# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x5102_DTC_ClearInfo_Feature.py
# Author:       Srinivasa V
# Created:      JAN 1 2025
# Description:  This File contains Test case scenarios for performing DTC Clear info test.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2025 APTIV
#
# This file is part of the MnM HPCC project - Manufacture Diagnostics Component feature test Automation.
#
# Manufacture Diagnostics Project is free software: you can redistribute it and/or modify it under the terms
# of Diagnostics Department ASUX terms and conditions.
# ------------------------------------------------------------------------------------------------------------------

import time
from infotest.core.trace_logger import app_logger
import pytest


# Global variable data

# Fixture functions


# ************************************************************* TEST CASES #01 *************************************************
# @pytest.mark.skip(reason="disabling just for debugging purpose")
@pytest.mark.order(96)
def test1_0x5102_DTC_ClearInfo_Feature(ManDiag_component):
    """ CPV-Diag: DTC Clear info test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
        Test Case: This test case verifies the functionality of Turning OFF DTC monitoring.
        Steps:
        1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
        2. Actual Test: Send DTC clear info and Validate the Response Valid status Byte, Data_length and Data.            
        3. Post-condition: Send GET DTC count and validate the response for NO DTCs recorder .

        Expected Result:
        - The response should indicate that all active DTCs list is cleared and validate the same.
        """
    app_logger.info(test_description)
    app_logger.info("Starting test: DTC Clear info feature Validation")
    SetCmdDisableDTCmonitor = "6D643E 51 01 01 00 01 00"
    GetCmdDTCMonitor = "6D643E 51 01 00 00 00"
    SetCmd = "6D643E 51 02 01 00 00"
    GetCmd = "6D643E 51 03 00 00 00"

    # Step 1: Pre_Condition Test
    app_logger.info("Executing pre Condition test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmdDisableDTCmonitor, expected_data_length="01", expected_data="00")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmdDTCMonitor, expected_data_length="01", expected_data="00")

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, retries=3)

    # Step 3: Post Test Case
    app_logger.info("Executing Post Condition test: GET and verify ")
    response_data = ManDiag_component.send_request_command_and_get_response(GetCmd)
    if response_data["DataLength"] == "02" and response_data["Databytes"] != "00 00":
        hex_str = response_data["Databytes"]
        decimal_value = int(hex_str.replace(" ", ""), 16)
        assert decimal_value == 0x00
    app_logger.info("Test case passed successfully!")
