# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x1406_AudioEqualizerSettings.py
# Author:       Srinivasa V
# Created:      2 Sept 2025
# Description:  This File contains Test case scenarios for validating 0x1406 Audio Equalizer Settings feature.
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
def test1_0x1406_AudioEqualizerSettings(ManDiag_component):
    """ SET EQ to Flat test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate SET EQ to Flat
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to SET EQ to Flat.          
                3. Post-condition: Send GET command to GET present EQ level.

                Expected Result:
                - The response should indicate that present EQ level is set to Flat.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: SET EQ to Flat test feature Validation")
    SetCmd = "6D643E 14 06 01 00 01 00"
    GetCmd = "6D643E 14 06 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET EQ to Flat ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="00", retries=5)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: GET present EQ level ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="00", retries=3)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
#@pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(8)
def test2_0x1406_AudioEqualizerSettings(ManDiag_component):
    """ SET EQ to Max test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate SET EQ to Max
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to SET EQ to Max.          
                3. Post-condition: Send GET command to GET present EQ level.

                Expected Result:
                - The response should indicate that present EQ level is set to Max.
                """
    app_logger.debug(test_description)
    SetCmd = "6D643E 14 06 01 00 01 01"
    GetCmd = "6D643E 14 06 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET EQ to Max ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01", retries=5)


    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: GET present EQ level ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="01", retries=3)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #03 *************************************************
#@pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(8)
def test3_0x1406_AudioEqualizerSettings(ManDiag_component):
    """ SET EQ to Min test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate SET EQ to Min
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to SET EQ to Min.          
                3. Post-condition: Send GET command to GET present EQ level.

                Expected Result:
                - The response should indicate that present EQ level is set to Min.
                """
    app_logger.debug(test_description)
    SetCmd = "6D643E 14 06 01 00 01 02"
    GetCmd = "6D643E 14 06 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET EQ to Min ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="02", retries=5)


    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: GET present EQ level ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="02", retries=3)

    app_logger.info("Test case passed successfully!")
