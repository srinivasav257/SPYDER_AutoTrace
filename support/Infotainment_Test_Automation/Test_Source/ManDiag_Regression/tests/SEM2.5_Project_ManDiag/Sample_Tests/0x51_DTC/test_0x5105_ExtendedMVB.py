# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x5105_ExtendedMVB.py
# Author:       Srinivasa V
# Created:      1 Sept 2025
# Description:  This File contains Test case scenarios for validating 0x5105 Extended MVB Information by DTC Number.
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
@pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(7)
def test1_0x5105_ExtendedMVB(ManDiag_component):
    """ Extended MVB Information by DTC Number Test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Extended MVB Information by DTC Number
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send GET command to Get Extended MVB Information by DTC Number.          
                3. Post-condition:

                Expected Result:
                - The response should print Extended MVB Information for the DTC Number.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Extended MVB Information by DTC Number test feature Validation")
    GetCmd = "6D643E 51 05 00 00 03 90 02 19"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Get Extended MVB Information ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_prefix="90 02 19", retries=2)

    # Step 3: Post Condition

    app_logger.info("Test case passed successfully!")
