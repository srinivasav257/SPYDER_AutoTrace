# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x5002_Apple_Authenticaiton.py
# Author:       Srinivasa V
# Created:      1 Sept 2025
# Description:  This File contains Test case scenarios for validating 0x5002 Apple Authenticaiton.
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
def test1_0x5002_Apple_Authenticaiton(ManDiag_component):
    """ Apple Authenticaiton Test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Apple Authenticaiton
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET command to check Apple Authenticaiton.          
                3. Post-condition:

                Expected Result:
                - The response should indicate that Apple chip self test is successful.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Apple Authenticaiton test feature Validation")
    GetCmd = "6D643E 50 02 01 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Apple Authenticaiton ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="01", retries=2)

    # Step 3: Post Condition

    app_logger.info("Test case passed successfully!")
