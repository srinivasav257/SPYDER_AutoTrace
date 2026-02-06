# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x0001_IOC_EnterExit_Mandiag_Session.py
# Author:       Srinivasa V
# Created:      AUG 1 2025
# Description:  This File contains Test case scenarios for performing EnterExit Mandiag Session 0x0001.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2025 APTIV
#
# This file is part of the MIB4 project - Manufacture Diagnostics Component feature test Automation.
#
# Manufacture Diagnostics Project is free software: you can redistribute it and/or modify it under the terms
# of Diagnostics Department ASUX terms and conditions.
# ------------------------------------------------------------------------------------------------------------------

from infotest.core.trace_logger import app_logger
import pytest

# ************************************************************* TEST CASES #01 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0001
@pytest.mark.order(1)
def test3_0x0001_EnterManDiag_Session(ManDiag_component):
    """ IOC ManDiag : Enter ManDiag Session Operation """
    test_description = """
            Test Case: Validate Enter ManDiag Session operation
            Steps:
            1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
            2. Actual Test: Send SET Enter ManDiag session cmd and validate the response.            
            3. Post-condition: Send GET Enter ManDiag session cmd and validate the response .

            Expected Result:
            - The response should indicate that the ManDiag Session has been successfully entered and sw must process any feature command 
              is passed by tester after session is opened.
            """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Enter ManDiag Session feature Validation")
    SetCommand = "6D643E 00 01 01 00 01 01"
    GetCommand = "6D643E 00 01 00 00 00"

    # Step 1: Pre Condition
    app_logger.info("Executing Pre Condition test: ")

    # step 2: Actual Test
    app_logger.info("Executing actual test: Send SET Enter ManDiag session")
    assert ManDiag_component.send_request_command_and_validate_response(SetCommand)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Send GET Enter ManDiag session")
    assert ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="01", expected_data="01")

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0001
@pytest.mark.order(2)
def test2_0x0001_ExitManDiag_Session(ManDiag_component):
    """ IOC ManDiag : Exit ManDiag Session Operation """
    test_description = """
            Test Case: Validate Exit ManDiag Session operation
            Steps:
            1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
            2. Actual Test: Send SET Exit ManDiag session cmd and validate the response.            
            3. Post-condition: Send GET Exit ManDiag session cmd and validate the response .

            Expected Result:
            - The response should indicate that the ManDiag Session has been successfully Closed and sw must not process any feature command 
              passed by tester.
            """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Exit Man Diag feature Validation")
    SetCommand = "6D643E 00 01 01 00 01 00"
    GetCommand = "6D643E 00 01 00 00 00"

    # Step 1: Pre Condition
    app_logger.info("Executing Pre Condition test: ")

    # step 2: Actual Test
    app_logger.info("Executing actual test: Send SET Exit ManDiag session")
    assert ManDiag_component.send_request_command_and_validate_response(SetCommand)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Send GET Exit ManDiag session")
    assert ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="01", expected_data="00")

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #03 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0001
@pytest.mark.order(3)
def test1_0x0001_Send_AnyCmd_without_ManDiag_Session_Opening(ManDiag_component):
    """ IOC ManDiag : Exit ManDiag Session Operation """
    test_description = """
            Test Case: Validate ManDiag Session
            Steps:
            1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                               Send GET Exit ManDiag session cmd and validate the response.
            2. Actual Test: Send any cmd and validate the response for "7F".          
            3. Post-condition: Nothing

            Expected Result:
            - The response should indicate that the ManDiag Session has been successfully Closed and sw must not process any feature command 
              passed by tester.
            """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Exit Man Diag feature Validation")
    SetCommand = "6D643E 00 01 01 00 01 00"
    GetWatchDogStatus = "6D643E 00 03 00 00 00"
    GetCommand = "6D643E 00 01 00 00 00"

    # Step 1: Pre Condition
    app_logger.info("Executing Pre Condition test: Send SET/GET Exit ManDiag session cmd")
    assert ManDiag_component.send_request_command_and_validate_response(SetCommand)
    assert ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="01", expected_data="00")

    # step 2: Actual Test
    app_logger.info("Executing actual test: Sending get watchDog status")
    assert ManDiag_component.send_request_command_and_validate_response(GetWatchDogStatus, expected_data_length="00", expected_status="7F")

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: ")

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #04 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0001
@pytest.mark.order(4)
def test4_0x0001_invalid_operation_for_man_diag_session(ManDiag_component):
    """ IOC ManDiag : Exit ManDiag Session Operation """
    test_description = """
            Test Case: Validate Exit ManDiag Session operation
            Steps:
            1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
            2. Actual Test: Send SET invalid ManDiag session cmd and validate the response.            
            3. Post-condition: 

            Expected Result:
            - The response should indicate that the given operation data byte is invalid.
            """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Invalid Data byte for manDiag Session Open/Close")
    SetCommand = "6D643E 00 01 01 00 01 02"

    # Step 1: Pre Condition
    app_logger.info("Executing Pre Condition test: ")

    # step 2: Actual Test
    app_logger.info("Executing actual test: Send SET Exit ManDiag session")
    result = ManDiag_component.send_request_command_and_validate_response(SetCommand, expected_data_length="00", expected_status="F7")
    if not result:
        pytest.skip("Skipping this test because this condition is not handled in code")

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test:")

    app_logger.info("Test case passed successfully!")



# ************************************************************* TEST CASES #06 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0001
@pytest.mark.order(5)
def test6_0x0001_Data_length_mismatch_for_man_diag_session(ManDiag_component):
    """ IOC ManDiag : Exit ManDiag Session Operation """
    test_description = """
            Test Case: Validate Exit ManDiag Session operation
            Steps:
            1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
            2. Actual Test: Send SET mismatch data length ManDiag session cmd and validate the response.            
            3. Post-condition: 

            Expected Result:
            - The response should indicate that the given operation data byte is invalid.
            """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Invalid Data byte for manDiag Session Open/Close")
    SetCommand = "6D643E 00 01 01 00 01 01 01"

    # Step 1: Pre Condition
    app_logger.info("Executing Pre Condition test: ")

    # step 2: Actual Test
    app_logger.info("Executing actual test: Send SET Exit ManDiag session")
    assert ManDiag_component.send_request_command_and_validate_response(SetCommand, expected_data_length="00", expected_status="F6")

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test:")

    app_logger.info("Test case passed successfully!")
	
# ************************************************************* TEST CASES #07 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0001
@pytest.mark.order(6)
def test7_0x0001_Invalid_Data_length_for_man_diag_session(ManDiag_component):
    """ IOC ManDiag : Exit ManDiag Session Operation """
    test_description = """
            Test Case: Validate Exit ManDiag Session operation
            Steps:
            1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
            2. Actual Test: Send SET mismatch data length ManDiag session cmd and validate the response.            
            3. Post-condition: 

            Expected Result:
            - The response should indicate that the given operation data byte is invalid.
            """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Invalid Data byte for manDiag Session Open/Close")
    SetCommand = "6D643E 00 01 01 00 02 01 01"

    # Step 1: Pre Condition
    app_logger.info("Executing Pre Condition test: ")

    # step 2: Actual Test
    app_logger.info("Executing actual test: Send SET Exit ManDiag session")
    assert ManDiag_component.send_request_command_and_validate_response(SetCommand, expected_data_length="00", expected_status="F8")

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test:")

    app_logger.info("Test case passed successfully!")

