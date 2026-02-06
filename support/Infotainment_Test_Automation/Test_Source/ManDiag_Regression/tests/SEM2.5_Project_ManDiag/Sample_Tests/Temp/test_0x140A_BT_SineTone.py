# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x140A_BT_SineTone.py
# Author:       Srinivasa V
# Created:      2 Sept 2025
# Description:  This File contains Test case scenarios for validating 0x140A BT Sine Tone feature.
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
def test1_0x140A_BT_SineTone(ManDiag_component):
    """ BT Sine Tone test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate BT Sine Tone Test
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to Enable BT Sine Tone Test.          
                3. Post-condition: 

                Expected Result:
                - The response should indicate that BT Sine Tone Test is enabled.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: BT Sine Tone test feature Validation")
    SetCmd = "6D643E 14 0A 01 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: BT Sine Tone Test ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_status="AA", retries=5)

    # Step 3: Post Condition
    
    app_logger.info("Test case passed successfully!")
