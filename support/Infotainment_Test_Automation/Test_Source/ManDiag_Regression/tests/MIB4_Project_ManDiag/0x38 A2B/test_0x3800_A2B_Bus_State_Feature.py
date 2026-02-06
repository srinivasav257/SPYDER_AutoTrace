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
def test1_0x3801_Phantom_A2B_MIC_Off_FeatureTest(ManDiag_component):
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
    GetCmd = "6D643E 38 01 00 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn OFF the A2B_MIC ")

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get A2B_MIC status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="04", expected_data="01 01 01 00", retries=2)

    app_logger.info("Test case passed successfully!")
	
# ************************************************************* TEST CASES #02 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
@pytest.mark.order(27)
def test2_0x3801_Phantom_A2B_MIC_Off_FeatureTest(ManDiag_component):
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
    GetCmd = "6D643E 38 01 00 00 01 02"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn OFF the A2B_MIC ")

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get A2B_MIC status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="04", expected_data="02 00 00 00", retries=2)

    app_logger.info("Test case passed successfully!")
	
# ************************************************************* TEST CASES #03 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
@pytest.mark.order(27)
def test3_0x3801_Phantom_A2B_MIC_Off_invalid_case(ManDiag_component):
    """ IOC Power mode : Phantom_A2B_MIC OFF test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the invalid operation value should be executed with negative response.
                Steps:
                1. Pre-condition: The Unit should be powered OFF and VIP must be fully BootUp and Running.
                2. Actual Test: Phantom_A2B_MIC OFF cmd with operation '03' and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition: The Response with status byte should be ERROR: "F7"

                Expected Result:
                - The response should indicate that passed operation value in the command is invalid.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Phantom A2B_MIC OFF test feature Validation")
    GetCmd = "6D643E 38 01 00 00 01 03"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn OFF the A2B_MIC ")

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get A2B_MIC status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_status="F7")

    app_logger.info("Test case passed successfully!")
	
	
# ************************************************************* TEST CASES #04 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
@pytest.mark.order(27)
def test4_0x3801_Data_length_mismatch_for_Phantom_A2B_MIC_Off(ManDiag_component):
    """ IOC Power mode : Phantom_A2B_MIC OFF test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of Phantom_A2B_MIC OFF .
                Steps:
                1. Pre-condition: The Unit should be powered OFF and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET mismatch data length Phantom_A2B_MIC OFF cmd and Validate the Response.            
                3. Post-condition: The Response with status byte should be ERROR: "F6"

                Expected Result:
                - The response should indicate that given operation data length is invalid.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Phantom A2B_MIC OFF test feature Validation")
    GetCmd = "6D643E 38 01 00 00 01 03 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn OFF the A2B_MIC ")

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get A2B_MIC status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_status="F6")

    app_logger.info("Test case passed successfully!")
	
	
# ************************************************************* TEST CASES #05 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
@pytest.mark.order(27)
def test5_0x3801_Invalid_Data_length_for_Phantom_A2B_MIC_Off(ManDiag_component):
    """ IOC Power mode : Phantom_A2B_MIC OFF test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of Turning Phantom_A2B_MIC OFF.
                Steps:
                1. Pre-condition: The Unit should be powered OFF and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET invalid data length Phantom_A2B_MIC OFFcmd and Validate the Response.            
                3. Post-condition: The Response with status byte should be ERROR: "F8"

                Expected Result:
                - The response should indicate that given operation data length is invalid.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Phantom A2B_MIC OFF test feature Validation")
    GetCmd = "6D643E 38 01 00 00 02 03 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn OFF the A2B_MIC ")

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get A2B_MIC status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_status="F8")

    app_logger.info("Test case passed successfully!")
	
	




