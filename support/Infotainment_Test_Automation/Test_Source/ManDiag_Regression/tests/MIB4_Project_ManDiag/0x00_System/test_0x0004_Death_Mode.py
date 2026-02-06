# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x0004_erminal_Filter_OnOff_Feature.py
# Author:       Srinivasa V
# Created:      JAN 1 2025
# Description:  This File contains Test case scenarios for performing Death mode ON/OFF Operation test.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2025 APTIV
#
# This file is part of the MIB4 project - Manufacture Diagnostics Component feature test Automation.
#
# Manufacture Diagnostics Project is free software: you can redistribute it and/or modify it under the terms
# of Diagnostics Department ASUX terms and conditions.
# ------------------------------------------------------------------------------------------------------------------

import time
import serial

from infotest.core.trace_logger import app_logger
import pytest

from infotest.devices.mandiag import Mandiag

# ************************************************************* TEST CASES #01 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0004
@pytest.mark.order(31)
def test1_0x0004_Death_Mode_On(ManDiag_component: Mandiag):
    """ IOC-ManDiag : Death Mode ON Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of Turning Death mode ON .
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send Turn Death mode ON  and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition: Send GET Death mode cmd and validate the response.

                Expected Result:
                - The response should indicate that Death mode has been turned ON.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: Death mode ON Operation feature Validation")
    SetCmd = "6D643E 00 04 01 00 01 01"
    GetCmd = "6D643E 00 04 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd,expected_data_length="00")

    # Step 3: Post Test Case
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd,expected_data_length="00",expected_data="01")
    app_logger.info("Test case passed successfully!")

# ************************************************************* TEST CASES #02 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0004
@pytest.mark.order(32)
def test2_0x0004_Death_Mode_Off(ManDiag_component: Mandiag):
    """ IOC-ManDiag : Death Mode OFF Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of Death mode OFF .
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send Turn Death mode OFF and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition: Send GET Death mode cmd and validate the response.

                Expected Result:
                - The response should indicate that Death mode has been turned OFF.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: Death mode OFF Operation feature Validation")
    SetCmd = "6D643E 00 04 01 00 01 00"
    GetCmd = "6D643E 00 04 00 00 00"
    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd,expected_data_length="00")

    # Step 3: Post Test Case
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd,expected_data_length="00",expected_data="00")
    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #03 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0004
@pytest.mark.order(33)
def test3_0x0004_Data_length_mismatch_for_Death_Mode(ManDiag_component: Mandiag):
    """ IOC-ManDiag : Death Mode ON Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of Turning Death mode ON .
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET mismatch data length Death mode cmd and Validate the Response.            
                3. Post-condition: The Response with status byte should be ERROR: "F6"

                Expected Result:
                - The response should indicate that given operation data length is invalid.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: Death mode ON Operation feature Validation")
    SetCmd = "6D643E 00 04 01 00 01 01 04"
    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd,expected_status="F6")

    # Step 3: Post Test Case
    app_logger.info("Test case passed successfully")

# ************************************************************* TEST CASES #04 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0004
@pytest.mark.order(34)
def test4_0x0004_Death_Mode_invalid_case(ManDiag_component: Mandiag):
    """ IOC-ManDiag : Death Mode ON Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of Turning Death mode ON .
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send Death mode cmd with operation '02' and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition: The Response with status byte should be ERROR: "F7"

                Expected Result:
                - The response should indicate that passed operation value in the command is invalid.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: Death mode ON Operation feature Validation")
    SetCmd = "6D643E 00 04 01 00 01 03"
    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd,expected_status="F7")

    # Step 3: Post Test Case
    app_logger.info("Test case passed successfully")
	
# ************************************************************* TEST CASES #05 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0004
@pytest.mark.order(35)
def test4_0x0004_Invalid_Data_length_for_Death_Mode(ManDiag_component: Mandiag):
    """ IOC-ManDiag : Death Mode ON Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of Turning Death mode ON .
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET invalid data length Death mode cmd and Validate the Response.            
                3. Post-condition: The Response with status byte should be ERROR: "F8"

                Expected Result:
                - The response should indicate that given operation data length is invalid.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: Death mode ON Operation feature Validation")
    SetCmd = "6D643E 00 04 01 00 02 03 00"
    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd,expected_status="F8")

    # Step 3: Post Test Case
    app_logger.info("Test case passed successfully")
	
