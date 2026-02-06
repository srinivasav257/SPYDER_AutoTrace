# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x3301_CAN_Dummy_Feature.py
# Author:       Srinivasa V
# Created:      JAN 1 2025
# Description:  This File contains Test case scenarios for performing CAN dummy message test.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2025 APTIV
#
# This file is part of the MnM HPCC project - Manufacture Diagnostics Component feature test Automation.
#
# Manufacture Diagnostics Project is free software: you can redistribute it and/or modify it under the terms
# of Diagnostics Department ASUX terms and conditions.
# ------------------------------------------------------------------------------------------------------------------

import time
import serial

from framework_modules.core.trace_logger import app_logger
import pytest

# ************************************************************* TEST CASES #01 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
@pytest.mark.order(27)
def test1_0x3800_Phantom_A2B_MIC_Off_FeatureTest(ManDiag_component):
    """ IOC Power mode : Phantom_A2B_MIC OFF test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Phantom_A2B_MIC OFF
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to turn A2B_MIC OFF.          
                3. Post-condition: Send GET command to verify the OFF status.

                Expected Result:
                - The response should indicate that Phantom voltage of A2B_MIC is powered OFF.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Phantom A2B_MIC OFF test feature Validation")
    SetCmd = "6D643E 38 00 01 00 02 01 02"
    GetCmd = "6D643E 38 00 00 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn OFF the A2B_MIC ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="00", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get A2B_MIC status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data="01 02", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
@pytest.mark.order(27)
def test2_0x3800_Phantom_A2B_MIC_Off_FeatureTest(ManDiag_component):
    """ IOC Power mode : Phantom_A2B_MIC OFF test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Phantom_A2B_MIC OFF
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to turn A2B_MIC OFF.          
                3. Post-condition: Send GET command to verify the OFF status.

                Expected Result:
                - The response should indicate that Phantom voltage of A2B_MIC is powered OFF.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Phantom A2B_MIC OFF test feature Validation")
    SetCmd = "6D643E 38 00 01 00 02 01 00"
    GetCmd = "6D643E 38 00 00 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn OFF the A2B_MIC ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="00", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get A2B_MIC status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data="01 00", retries=2)

    app_logger.info("Test case passed successfully!")
	
# ************************************************************* TEST CASES #03 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
@pytest.mark.order(27)
def test3_0x3800_Phantom_A2B_MIC_Off_FeatureTest(ManDiag_component):
    """ IOC Power mode : Phantom_A2B_MIC OFF test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Phantom_A2B_MIC OFF
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to turn A2B_MIC OFF.          
                3. Post-condition: Send GET command to verify the OFF status.

                Expected Result:
                - The response should indicate that Phantom voltage of A2B_MIC is powered OFF.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Phantom A2B_MIC OFF test feature Validation")
    SetCmd = "6D643E 38 00 01 00 02 01 01"
    GetCmd = "6D643E 38 00 00 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn OFF the A2B_MIC ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="00", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get A2B_MIC status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data="01 01", retries=2)

    app_logger.info("Test case passed successfully!")
	
# ************************************************************* TEST CASES #04 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
@pytest.mark.order(27)
def test4_0x3800_Phantom_A2B_MIC_Off_FeatureTest(ManDiag_component):
    """ IOC Power mode : Phantom_A2B_MIC OFF test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Phantom_A2B_MIC OFF
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to turn A2B_MIC OFF.          
                3. Post-condition: Send GET command to verify the OFF status.

                Expected Result:
                - The response should indicate that Phantom voltage of A2B_MIC is powered OFF.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Phantom A2B_MIC OFF test feature Validation")
    SetCmd = "6D643E 38 00 01 00 02 02 00"
    GetCmd = "6D643E 38 00 00 00 01 02"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn OFF the A2B_MIC ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="00", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get A2B_MIC status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data="02 00", retries=2)

    app_logger.info("Test case passed successfully!")
	
# ************************************************************* TEST CASES #05 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
@pytest.mark.order(27)
def test5_0x3800_Phantom_A2B_MIC_Off_FeatureTest(ManDiag_component):
    """ IOC Power mode : Phantom_A2B_MIC OFF test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Phantom_A2B_MIC OFF
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to turn A2B_MIC OFF.          
                3. Post-condition: Send GET command to verify the OFF status.

                Expected Result:
                - The response should indicate that Phantom voltage of A2B_MIC is powered OFF.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Phantom A2B_MIC OFF test feature Validation")
    SetCmd = "6D643E 38 00 01 00 02 02 01"
    GetCmd = "6D643E 38 00 00 00 01 02"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn OFF the A2B_MIC ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="00", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get A2B_MIC status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data="02 01", retries=2)

    app_logger.info("Test case passed successfully!")
	
# ************************************************************* TEST CASES #06 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
@pytest.mark.order(27)
def test6_0x3800_Phantom_A2B_MIC_Off_FeatureTest(ManDiag_component):
    """ IOC Power mode : Phantom_A2B_MIC OFF test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Phantom_A2B_MIC OFF
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to turn A2B_MIC OFF.          
                3. Post-condition: Send GET command to verify the OFF status.

                Expected Result:
                - The response should indicate that Phantom voltage of A2B_MIC is powered OFF.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Phantom A2B_MIC OFF test feature Validation")
    SetCmd = "6D643E 38 00 01 00 02 02 02"
    GetCmd = "6D643E 38 00 00 00 01 02"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn OFF the A2B_MIC ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="00", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get A2B_MIC status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data="02 02", retries=2)

    app_logger.info("Test case passed successfully!")