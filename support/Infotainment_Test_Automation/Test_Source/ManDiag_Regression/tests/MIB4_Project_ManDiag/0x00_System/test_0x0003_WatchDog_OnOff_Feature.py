# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x0003_WatchDog_OnOff_Feature.py
# Author:       Srinivasa V
# Created:      JAN 1 2025
# Description:  This File contains Test case scenarios for performing WatchDog ON/OFF Operation.
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
@pytest.mark.mib4_System_0x0003
@pytest.mark.order(21)
def test1_0x0003_Watchdog_Monitoring_Off(ManDiag_component):
    """ IOC-PowerMode: Watchdog monitoring OFF Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of Turning OFF Watchdog monitoring.
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send Turn OFF watchdog monitoring and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition: Send GET watchdog monitoring status cmd and validate the response .

                Expected Result:
                - The response should indicate that Watchdog monitoring has been OFF and No reset trigger should happen.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: Watchdog monitoring OFF Operation feature Validation")
    SetCmd = " 6D643E 00 03 01 00 01 00"
    GetCmd = "6D643E 00 03 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="00")

    # Step 3: Post Test Case
    app_logger.info("Executing Post Condition test: GET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="00")

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0003
@pytest.mark.order(22)
def test2_0x0003_Watchdog_Monitoring_On(ManDiag_component):
    """ IOC-PowerMode: Watchdog monitoring ON Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of Turning OFF Watchdog monitoring.
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send Turn ON watchdog monitoring and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition: Send GET watchdog monitoring status cmd and validate the response .

                Expected Result:
                - The response should indicate that Watchdog monitoring has been ON and reset trigger should happen if SOH fails.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: Watchdog monitoring ON Operation feature Validation")
    SetCmd = " 6D643E 00 03 01 00 01 01"
    GetCmd = "6D643E 00 03 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01")

    # Step 3: Post Test Case
    app_logger.info("Executing Post Condition test: GET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="01")

    app_logger.info("Test case passed successfully!")
	
# ************************************************************* TEST CASES #03 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0003
@pytest.mark.order(23)
def test3_0x0003_Watchdog_Monitoring_Invalid_case(ManDiag_component):
    """ IOC-PowerMode: Watchdog monitoring Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the invalid operation value should be executed with negative response.
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send Watchdog monitoring cmd with operation '03' and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition: The Response with status byte should be ERROR: "F7"

                Expected Result:
                - The response should indicate that Watchdog monitoring passed operation value in the command is invalid
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: Watchdog monitoring ON Operation feature Validation")
    SetCmd = " 6D643E 00 03 01 00 01 03"


    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_status="F7")

    # Step 3: Post Test Case
    

    app_logger.info("Test case passed successfully!")
	

# ************************************************************* TEST CASES #04 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0003
@pytest.mark.order(24)
def test4_0x0003_Data_length_mismatch_for_Watchdog_Monitoring(ManDiag_component):
    """ IOC-PowerMode: Watchdog monitoring Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the invalid operation value should be executed with negative response.
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET mismatch data length Watchdog monitoring cmd and Validate the Response.             
                3. Post-condition: The Response with status byte should be ERROR: "F6"

                Expected Result:
                - The response should indicate that given operation data length is invalid
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: Watchdog monitoring ON Operation feature Validation")
    SetCmd = " 6D643E 00 03 01 00 02 03"


    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_status="F6")

    # Step 3: Post Test Case
    

    app_logger.info("Test case passed successfully!")
	
# ************************************************************* TEST CASES #05 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0003
@pytest.mark.order(25)
def test5_0x0003_Invalid_Data_length_for_Watchdog_Monitoring(ManDiag_component):
    """ IOC-PowerMode: Watchdog monitoring Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the invalid operation value should be executed with negative response.
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET invalid data length Watchdog monitoring cmd and Validate the Response.            
                3. Post-condition: The Response with status byte should be ERROR: "F8"

                Expected Result:
                - The response should indicate that given operation data length is invalid 
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: Watchdog monitoring ON Operation feature Validation")
    SetCmd = " 6D643E 00 03 01 00 02 03 01"


    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_status="F8")

    # Step 3: Post Test Case
    

    app_logger.info("Test case passed successfully!")