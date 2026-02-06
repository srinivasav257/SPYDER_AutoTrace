# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x0002_Terminal_Filter_OnOff_Feature.py
# Author:       Srinivasa V
# Created:      JAN 1 2025
# Description:  This File contains Test case scenarios for performing Terminal filter ON/OFF Operation test.
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

# ************************************************************* TEST CASES #01 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0002
@pytest.mark.order(11)
def test1_0x0002_Terminal_Filter_On(ManDiag_component):
    """ IOC-ManDiag : Terminal filter ON Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of Turning Terminal filter ON .
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send Turn Terminal filter ON  and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition: 

                Expected Result:
                - The response should indicate that Terminal filter has been turned ON and No traces apart from ManDIag request/response should appear on serial logger.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: Terminal filter ON Operation feature Validation")
    SetCmd = "6D643E 00 02 01 00 01 02"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="02")

    # Step 3: Post Test Case

    app_logger.info("Test case passed successfully!")

# ************************************************************* TEST CASES #02 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0002
@pytest.mark.order(12)
def test2_0x0002_Terminal_Filter_ON_exclude_ManDiag(ManDiag_component):
    """ IOC-ManDiag : Terminal filter ON Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of Turning Terminal filter ON .
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send Turn Terminal filter ON  and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition: 
                Expected Result:
                - The response should indicate that Terminal filter has been turned ON and no traces should appear on serial logger excluding ManDiag Component.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: Terminal filter ON excluding (ManDiag Component) Operation feature Validation")
    SetCmd = "6D643E 00 02 01 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01")

    # Step 3: Post Test Case

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #03 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0002
@pytest.mark.order(13)
def test3_0x0002_Terminal_Filter_Off(ManDiag_component):
    """ IOC-ManDiag : Terminal filter OFF Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of Turning Terminal filter OFF .
                Steps:
                1. Pre-condition: The Unit should be powered OFF and VIP must be fully BootUp and Running.
                2. Actual Test: Send Turn Terminal filter OFF  and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition: 

                Expected Result:
                - The response should indicate that Terminal filter has been turned OFF and all traces should appear on serial logger.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: Terminal filter OFF Operation feature Validation")
    SetCmd = "6D643E 00 02 01 00 01 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="00")

    # Step 3: Post Test Case

    app_logger.info("Test case passed successfully!")

# ************************************************************* TEST CASES #04 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0002
@pytest.mark.order(14)
def test4_0x0002_Terminal_Filter_invalid_case(ManDiag_component):
    """ IOC-ManDiag : Terminal filter operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the invalid operation value should be executed with negative response.
                Steps:
                1. Pre-condition: The Unit should be powered OFF and VIP must be fully BootUp and Running.
                2. Actual Test: Send Terminal filter cmd with operation '03' and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition: The Response with status byte should be ERROR: "F7"

                Expected Result:
                - The response should indicate that passed operation value in the command is invalid and all traces should appear on serial logger.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: Invalid Data byte for Terminal filter On/OFF Operation ")
    SetCmd = "6D643E 00 02 01 00 01 03"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_status="F7")

    # Step 3: Post Test Case

    app_logger.info("Test case passed successfully!")
	
# ************************************************************* TEST CASES #05 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0002
@pytest.mark.order(15)
def test5_0x0002_Data_length_mismatch_for_Terminal_Filter(ManDiag_component):
    """ IOC-ManDiag : Terminal filter operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of Turning Terminal filter OFF .
                Steps:
                1. Pre-condition: The Unit should be powered OFF and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET mismatch data length Terminal filter cmd and Validate the Response.            
                3. Post-condition: The Response with status byte should be ERROR: "F6"

                Expected Result:
                - The response should indicate that given operation data length is invalid and all traces should appear on serial logger.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test:Invalid Data length for Terminal filter On/Off")
    SetCmd = "6D643E 00 02 01 00 02 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_status="F6")

    # Step 3: Post Test Case

    app_logger.info("Test case passed successfully!")
	
# ************************************************************* TEST CASES #06 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0002
@pytest.mark.order(16)
def test6_0x0002_Invalid_Data_length_for_Terminal_Filter(ManDiag_component):
    """ IOC-ManDiag : Terminal filter operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of Turning Terminal filter OFF .
                Steps:
                1. Pre-condition: The Unit should be powered OFF and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET invalid data length Terminal filter cmd and Validate the Response.            
                3. Post-condition: The Response with status byte should be ERROR: "F8"

                Expected Result:
                - The response should indicate that given operation data length is invalid and all traces should appear on serial logger.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test:Invalid Data length for Terminal filter On/Off")
    SetCmd = "6D643E 00 02 01 00 02 01 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_status="F8")

    # Step 3: Post Test Case

    app_logger.info("Test case passed successfully!")