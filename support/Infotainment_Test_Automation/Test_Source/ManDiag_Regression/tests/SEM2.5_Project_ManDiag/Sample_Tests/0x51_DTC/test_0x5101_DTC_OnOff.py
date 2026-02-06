# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x5101_DTC_OnOff.py
# Author:       Srinivasa V
# Created:      Aug 22 2025
# Description:  This File contains Test case scenarios for validating 0x5101 DTC ON/OFF feature.
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
#@pytest.mark.order(9)
def test1_0x5101_DTC_OnOff(ManDiag_component):
    """ DTC ON test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate DTC ON
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to turn DTC ON.          
                3. Post-condition: Send GET command to verify the ON status.

                Expected Result:
                - The response should indicate that DTC is ON.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: DTC ON test feature Validation")
    SetCmd = "6D643E 51 01 01 00 01 01"
    GetCmd = "6D643E 51 01 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn ON DTC ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get DTC status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="01", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(10)
def test2_0x5101_DTC_OnOff(ManDiag_component):
    """ DTC OFF test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate DTC OFF
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET command to turn DTC OFF.          
                3. Post-condition: Send GET command to verify the OFF status.

                Expected Result:
                - The response should indicate that DTC is powered OFF.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: DTC OFF test feature Validation")
    SetCmd = "6D643E 51 01 01 00 01 00"
    GetCmd = "6D643E 51 01 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn OFF DTC ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="00",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get DTC status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="00", retries=2)

    app_logger.info("Test case passed successfully!")
