# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x51_DTCInfo.py
# Author:       Srinivasa V
# Created:      Aug 29 2025
# Description:  This File contains Test case scenarios for validating 0x5103 Get Number of DTCs and 0x5104 Get List of DTCs feature.
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
def test1_0x51_DTCInfo(ManDiag_component):
    """ Get Number of DTCs test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Get Number of DTCs
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send GET command to get the number of DTCs present.          
                3. Post-condition: 

                Expected Result:
                - The response should return the number of DTCs present.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Get Number of DTCs test feature Validation")
    GetCmd = "6D643E 51 03 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Get Number of DTCs ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, retries=2)

    # Step 3: Post Condition

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(10)
def test2_0x51_DTCInfo(ManDiag_component):
    """ Get List of DTCs test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Get List of DTCs
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send GET command to get the list of DTCs present.          
                3. Post-condition:

                Expected Result:
                - The response should return the list of DTCs present.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Get List of DTCs test feature Validation")
    GetCmd = "6D643E 51 04 00 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Get List of DTCs ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_prefix="01", retries=2)

    # Step 3: Post Condition

    app_logger.info("Test case passed successfully!")
