# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x0A02_VehicleMode.py
# Author:       Srinivasa V
# Created:      Aug 25 2025
# Description:  This File contains Test case scenarios for validating 0x0A02 Vehicle Mode features.
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
#@pytest.mark.order(9)
def test1_0x0A02_VehicleMode(ManDiag_component):
    """ Vehicle Mode : Prerunning test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Vehicle Mode set to Prerunning
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET command to enable Prerunning as the Vehicle Mode.          
                3. Post-condition: Send GET command to verify Prerunning Vehicle Mode status.

                Expected Result:
                - The response should indicate that the Vehicle Mode is set to Prerunning.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Vehicle Mode - Prerunning test feature Validation")
    SetCmd = "6D643E 0A 02 01 00 01 04"
    GetCmd = "6D643E 0A 02 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Set the Vehicle Mode to Prerunning ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="04",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get Vehicle Mode status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="04", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(10)
def test2_0x0A02_VehicleMode(ManDiag_component):
    """ Vehicle Mode : Cranking test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Vehicle Mode set to Cranking
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET command to enable Cranking as the Vehicle Mode.          
                3. Post-condition: Send GET command to verify Cranking Vehicle Mode status.

                Expected Result:
                - The response should indicate that the Vehicle Mode is set to Cranking.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Vehicle Mode - Cranking test feature Validation")
    SetCmd = "6D643E 0A 02 01 00 01 05"
    GetCmd = "6D643E 0A 02 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Set the Vehicle Mode to Cranking")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="05",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get Vehicle Mode status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="05", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #03 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(11)
def test3_0x0A02_VehicleMode(ManDiag_component):
    """ Vehicle Mode : Running test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Vehicle Mode set to Running
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET command to enable Running as the Vehicle Mode.          
                3. Post-condition: Send GET command to verify Running Vehicle Mode status.

                Expected Result:
                - The response should indicate that the Vehicle Mode is set to Running.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Vehicle Mode - Running test feature Validation")
    SetCmd = "6D643E 0A 02 01 00 01 06"
    GetCmd = "6D643E 0A 02 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Set the Vehicle Mode to Running")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="06",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get Vehicle Mode status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="06", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #04 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(11)
def test4_0x0A02_VehicleMode(ManDiag_component):
    """ Vehicle Mode : Error test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Vehicle Mode set to Error
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET command to enable Error as the Vehicle Mode.          
                3. Post-condition: Send GET command to verify Error Vehicle Mode status.

                Expected Result:
                - The response should indicate that the Vehicle Mode is set to Error.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Vehicle Mode - Error test feature Validation")
    SetCmd = "6D643E 0A 02 01 00 01 0E"
    GetCmd = "6D643E 0A 02 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Set the Vehicle Mode to Error")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="0E",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get Vehicle Mode status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="0E", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #05 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(11)
def test5_0x0A02_VehicleMode(ManDiag_component):
    """ Vehicle Mode : Not Available test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Vehicle Mode set to Not Available
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET command to enable Not Available as the Vehicle Mode.          
                3. Post-condition: Send GET command to verify Not Available Vehicle Mode status.

                Expected Result:
                - The response should indicate that the Vehicle Mode is set to Not Available.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Vehicle Mode - Not Available test feature Validation")
    SetCmd = "6D643E 0A 02 01 00 01 0F"
    SetCmd1 = "6D643E 0A 02 01 00 01 06"
    GetCmd = "6D643E 0A 02 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Set the Vehicle Mode to Not Available")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="0F",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get Vehicle Mode status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="0F", retries=2)
    app_logger.info("Executing Post Condition test: Setting Vehicle Mode status to Running to validate other features. ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd1, expected_data_length="01", expected_data="06", retries=2)

    app_logger.info("Test case passed successfully!")
