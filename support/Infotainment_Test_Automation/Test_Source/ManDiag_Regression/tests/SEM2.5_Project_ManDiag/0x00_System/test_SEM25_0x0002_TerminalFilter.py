# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x0002_TerminalFilter.py
# Author:       Srinivasa V
# Created:      Sept 1 2025
# Description:  This File contains Test case scenarios for validating 0x0002 Terminal Filter feature.
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
#@pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(7)
def test1_0x0002_TerminalFilter(ManDiag_component):
    """ Set Terminal Filter to Level One: ManDiag messages and extra info strings test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Set Operation for Terminal Filter to Level One
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to set Terminal Filter to Level One.          
                3. Post-condition:

                Expected Result:
                - The response should indicate that the Terminal Filter is set to Level One.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Set Operation for Terminal Filter to Level One test feature Validation")
    SetCmd = "6D643E 00 02 01 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Set Operation for Terminal Filter to Level One ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01", retries=2)

    # Step 3: Post Condition

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
#@pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(8)
def test2_0x0002_TerminalFilter(ManDiag_component):
    """ Set Terminal Filter to Level Two: ManDiag messages only test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Set Operation for Terminal Filter to Level Two
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to set Terminal Filter to Level Two.          
                3. Post-condition:

                Expected Result:
                - The response should indicate that the Terminal Filter is set to Level Two.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Set Operation for Terminal Filter to Level Two test feature Validation")
    SetCmd = "6D643E 00 02 01 00 01 02"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Set Operation for Terminal Filter to Level Two ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="02", retries=2)

    # Step 3: Post Condition

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #03 *************************************************
#@pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(8)
def test3_0x0002_TerminalFilter(ManDiag_component):
    """ Set Terminal Filter to Level Zero : All messages test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Set Operation for Terminal Filter to Level Zero
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to set Terminal Filter to Level Zero.          
                3. Post-condition:

                Expected Result:
                - The response should indicate that the Terminal Filter is set to Level Zero.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Set Operation for Terminal Filter to Level Zero test feature Validation")
    SetCmd = "6D643E 00 02 01 00 01 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Set Operation for Terminal Filter to Level Zero ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="00", retries=2)

    # Step 3: Post Condition

    app_logger.info("Test case passed successfully!")
