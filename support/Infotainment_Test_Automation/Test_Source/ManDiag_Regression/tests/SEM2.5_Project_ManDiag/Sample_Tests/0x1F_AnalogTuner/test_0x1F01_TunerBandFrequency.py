# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x1F01_TunerBandFrequency.py
# Author:       Srinivasa V
# Created:      Aug 29 2025
# Description:  This File contains Test case scenarios for validating 0x1F01 Tuner Band Frequency.
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
# @pytest.mark.order(7)
def test1_0x1F01_TunerBandFrequency(ManDiag_component):
    """ Set Tuner frequency of Dirana3 Tuner 1 for band AM Test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Set Tuner frequency of Dirana3 Tuner 1 for band AM
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET command to Set Tuner frequency of Dirana3 Tuner 1 for band AM.          
                3. Post-condition: Send GET command to Get Tuner frequency of Dirana3 Tuner 1 for band AM. 

                Expected Result:
                - The response should indicate that Tuner frequency of Dirana3 Tuner 1 for band AM is set.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Set Tuner frequency of Dirana3 Tuner 1 for band AM Test")
    SetCmd = "6D643E 1F 01 01 00 04 01 01 03 DE"
    GetCmd = "6D643E 1F 01 00 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Set Tuner frequency of Dirana3 Tuner 1 for band AM ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="04", expected_data="01 01 03 DE", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing actual test: Get Tuner frequency of Dirana3 Tuner 1 for band AM ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="04", expected_data="01 01 03 DE", retries=2)

    app_logger.info("Test case passed successfully!")
