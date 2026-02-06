# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x2806_USBHostMode.py
# Author:       Srinivasa V
# Created:      Aug 22 2025
# Description:  This File contains Test case scenarios for validating 0x2806 USB Host Mode.
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
def test1_0x2806_USBHostMode(ManDiag_component):
    """ USB Host Mode Enable """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate USB Host Mode
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                                  Send GET command to verify USB Host Mode is disabled.
                2. Actual Test: Send SET command to enable USB Host Mode.          
                3. Post-condition: Send GET command to verify USB Host Mode is enabled.

                Expected Result:
                - The response should indicate that USB Host Mode is enabled.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: USB Host Mode test feature Validation")
    SetCmd = "6D643E 28 06 01 00 02 02 01"
    GetCmd = "6D643E 28 06 00 00 01 02"

    # Step 1: Pre_Condition Test
    app_logger.info("Executing Pre Condition test: Get USB Host Mode ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data="02 00", retries=2)
    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Enable USB Host Mode ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="02", expected_data="02 01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get USB Host Mode ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data="02 01", retries=2)

    app_logger.info("Test case passed successfully!")


