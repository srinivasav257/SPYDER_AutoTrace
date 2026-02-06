# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x5004_FAN_Mode.py
# Author:       Srinivasa V
# Created:      Aug 28 2025
# Description:  This File contains Test case scenarios for validating 0x5004 FAN Mode feature.
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
def test1_0x5004_FAN_Mode(ManDiag_component):
    """ FAN ON test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate FAN ON
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to turn FAN ON.          
                3. Post-condition: Send GET command to verify the ON status.

                Expected Result:
                - The response should indicate that FAN is powered ON.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: FAN ON test feature Validation")
    SetCmd = "6D643E 50 04 01 00 01 01"
    GetCmd = "6D643E 50 04 00 00 00"

    # Step 1: Pre_Condition Test
 
    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn FAN ON ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01", retries=2)
    time.sleep(3)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get FAN ON RPM status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd,expected_data_length="03", expected_data_prefix="01", retries=2)
    time.sleep(3)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
#@pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(8)
def test2_0x5004_FAN_Mode(ManDiag_component):
    """ FAN OFF test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate FAN OFF
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to turn FAN OFF.          
                3. Post-condition: Send GET command to verify the OFF status.

                Expected Result:
                - The response should indicate that FAN is powered OFF.
                """
    app_logger.debug(test_description)
    SetCmd = "6D643E 50 04 01 00 01 00"
    GetCmd = "6D643E 50 04 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn FAN OFF ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="00",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get FAN OFF RPM status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="03", expected_data_prefix="00", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #03 *************************************************
#@pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(8)
def test3_0x5004_FAN_Mode(ManDiag_component):
    """ Set FAN to Automatic Control Mode test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate FAN set to Automatic Control Mode
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to set FAN to Automatic Control Mode.          
                3. Post-condition: Send GET command to verify the mode status.

                Expected Result:
                - The response should indicate that FAN is set to Automatic Control Mode.
                """
    app_logger.debug(test_description)
    SetCmd = "6D643E 50 04 01 00 01 02"
    GetCmd = "6D643E 50 04 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Set FAN to Automatic Control Mode ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="02",  retries=2)
    #time.sleep(3)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get FAN Automatic Mode RPM status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="03", expected_data_prefix="02", retries=2)

    app_logger.info("Test case passed successfully!")
