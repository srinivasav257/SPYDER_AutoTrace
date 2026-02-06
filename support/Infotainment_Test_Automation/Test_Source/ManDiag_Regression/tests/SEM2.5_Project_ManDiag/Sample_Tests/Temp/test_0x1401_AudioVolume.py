# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x1401_AudioVolume.py
# Author:       Srinivasa V
# Created:      Aug 29 2025
# Description:  This File contains Test case scenarios for validating 0x1401 Audio Volume feature.
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
def test1_0x1401_AudioVolume(ManDiag_component):
    """ Set Audio Volume to Zero test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Audio Volume set to Zero
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to Set Audio Volume to Zero.          
                3. Post-condition: Send GET command to get Volume.

                Expected Result:
                - The response should indicate that Volume is set to Zero.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Set Audio Volume to Zero test feature Validation")
    SetCmd = "6D643E 14 01 01 00 01 00"
    GetCmd = "6D643E 14 01 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Set Audio Volume to Zero ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="00", retries=5)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get Volume ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="00", retries=3)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
#@pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(8)
def test2_0x1401_AudioVolume(ManDiag_component):
    """ Set Audio Volume to Max test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Audio Volume set to Max
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to Set Audio Volume to Max.          
                3. Post-condition: Send GET command to get Volume.

                Expected Result:
                - The response should indicate that Volume is set to Max.
                """
    app_logger.debug(test_description)
    SetCmd = "6D643E 14 01 01 00 01 23"
    GetCmd = "6D643E 14 01 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Set Audio Volume to Max ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="23", retries=5)


    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get Volume ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="23", retries=3)

    app_logger.info("Test case passed successfully!")
