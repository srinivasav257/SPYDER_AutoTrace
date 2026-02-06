# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x140B_Flat_and_Center_Audio.py
# Author:       Srinivasa V
# Created:      2 Sept 2025
# Description:  This File contains Test case scenarios for validating 0x140B Flat and Center Audio feature.
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
def test1_0x140B_Flat_and_Center_Audio(ManDiag_component):
    """ Set Flat and Center Audio ON test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Set Flat and Center Audio ON
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to Set Flat and Center Audio ON.          
                3. Post-condition:

                Expected Result:
                - The response should indicate that Flat and Center Audio is ON.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Set Flat and Center Audio ON test feature Validation")
    SetCmd = "6D643E 14 0B 01 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Set Flat and Center Audio ON ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01", retries=3)

    # Step 3: Post Condition

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
#@pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(7)
def test2_0x140B_Flat_and_Center_Audio(ManDiag_component):
    """ Set Flat and Center Audio OFF test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Set Flat and Center Audio OFF
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to Set Flat and Center Audio OFF.          
                3. Post-condition:

                Expected Result:
                - The response should indicate that Flat and Center Audio is OFF.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Set Flat and Center Audio OFF test feature Validation")
    SetCmd = "6D643E 14 0B 01 00 01 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Set Flat and Center Audio OFF ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="00", retries=3)

    # Step 3: Post Condition

    app_logger.info("Test case passed successfully!")
