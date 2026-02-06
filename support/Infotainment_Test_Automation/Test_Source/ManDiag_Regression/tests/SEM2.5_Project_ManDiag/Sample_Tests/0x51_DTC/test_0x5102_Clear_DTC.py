# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x5102_Clear_DTC.py
# Author:       Srinivasa V
# Created:      Aug 22 2025
# Description:  This File contains Test case scenarios for validating 0x5102 Clear DTC fetaure.
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
def test1_0x5102_Clear_DTC(ManDiag_component):
    """ Enable Clear DTC information """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Clear DTC Feature
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET command to enable Clear DTC feature.          
                3. Post-condition: Send GET command to verify no DTCs are present.

                Expected Result:
                - The response should indicate that no DTCs are present.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Clear DTC feature Validation")
    SetCmd = "6D643E 51 02 01 00 00"
    GetCmd = "6D643E 51 03 00 00 00"

    # Step 1: Pre_Condition Test
    
    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Enable Clear DTC Feature ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get no. of DTCs present ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="00", retries=2)

    app_logger.info("Test case passed successfully!")


