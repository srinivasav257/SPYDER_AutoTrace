# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x3D08_BT_Internal_Loopback_Disable.py
# Author:       Srinivasa V
# Created:      Aug 28 2025
# Description:  This File contains Test case scenarios for validating 0x3D08 BT Internal Loopback Disable feature.
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
def test1_0x3D08_BT_Internal_Loopback_Disable(ManDiag_component):
    """ Disable BT Internal Loopback test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate BT Internal Loopback Disable.
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET command to disable BT Internal Loopback.          
                3. Post-condition: Send GET command to verify the status.

                Expected Result:
                - The response should indicate that BT Internal Loopback is disabled.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Disable BT Internal Loopback test feature Validation")
    SetCmd = "6D643E 3D 08 01 00 01 00"
    GetCmd = "6D643E 3D 08 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Disable BT Internal Loopback ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_status="AA", retries=3)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get BT Internal Loopback status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="00", retries=3)

    app_logger.info("Test case passed successfully!")
