# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x0A0B_PWD_ANT.py
# Author:       Srinivasa V
# Created:      Aug 25 2025
# Description:  This File contains Test case scenarios for validating 0x0A0B PWD Antenna ON/OFF feature.
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
def test1_0x0A0B_PWD_ANT(ManDiag_component):
    """ PWD Antenna OFF test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate PWD Antenna OFF
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to turn PWD Antenna OFF.          
                3. Post-condition: Send GET command to verify the OFF status.

                Expected Result:
                - The response should indicate that PWD Antenna is powered OFF.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: PWD Antenna OFF test feature Validation")
    SetCmd = "6D643E 0A 0B 01 00 01 00"
    GetCmd = "6D643E 0A 0B 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn OFF PWD Antenna ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01",  expected_data="00", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get PWD Antenna status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="00", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(8)
def test2_0x0A0B_PWD_ANT(ManDiag_component):
    """ PWD Antenna ON test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate PWD Antenna ON
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to turn PWD Antenna ON.          
                3. Post-condition: Send GET command to verify the ON status.

                Expected Result:
                - The response should indicate that PWD Antenna is powered ON.
                """
    app_logger.debug(test_description)
    SetCmd = "6D643E 0A 0B 01 00 01 01"
    GetCmd = "6D643E 0A 0B 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn ON PWD Antenna ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get PWD Antenna status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="01", retries=2)

    app_logger.info("Test case passed successfully!")
