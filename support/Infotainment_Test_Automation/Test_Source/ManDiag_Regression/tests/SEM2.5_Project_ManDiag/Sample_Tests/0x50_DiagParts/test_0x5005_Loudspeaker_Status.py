# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x5005_Loudspeaker_Status.py
# Author:       Srinivasa V
# Created:      1 Sept 2025
# Description:  This File contains Test case scenarios for validating 0x5005 Loudspeaker Status feature.
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
def test1_0x5005_Loudspeaker_Status(ManDiag_component):
    """ Loudspeaker Status test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Loudspeaker Status
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to trigger Loudspeaker Status processing.          
                3. Post-condition: Send GET command to verify the status.

                Expected Result:
                - The response should indicate the status of the Loudspeaker.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Loudspeaker Status test feature Validation")
    SetCmd = "6D643E 50 05 01 00 00"
    GetCmd = "6D643E 50 05 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Trigger Loudspeaker Status processing ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_status="AA", expected_data_length="00", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get Loudspeaker status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="04", retries=2)

    app_logger.info("Test case passed successfully!")
