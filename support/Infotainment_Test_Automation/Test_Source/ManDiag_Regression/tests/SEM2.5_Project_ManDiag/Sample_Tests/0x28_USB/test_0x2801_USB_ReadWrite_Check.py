# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x2801_USB_ReadWrite_Check.py
# Author:       Srinivasa V
# Created:      Aug 29 2025
# Description:  This File contains Test case scenarios for validating 0x2801 USB Read/Write Check.
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
def test1_0x2801_USB_ReadWrite_Check(ManDiag_component):
    """ USB Read/Write Check Test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate USB Read/Write Check
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send GET command to check the Read/Write functionality of USB.          
                3. Post-condition:

                Expected Result:
                - The response should indicate that USB Read/Write Check has passed.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: USB Read/Write Check test feature Validation")
    GetCmd = "6D643E 28 01 00 00 01 02"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: USB Read/Write Check ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data_prefix="02", retries=2)

    # Step 3: Post Condition

    app_logger.info("Test case passed successfully!")
