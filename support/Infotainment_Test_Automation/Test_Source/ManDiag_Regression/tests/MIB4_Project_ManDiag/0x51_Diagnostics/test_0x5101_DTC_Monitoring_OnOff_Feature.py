# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x5101_DTC_Monitoring_OnOff_Feature.py
# Author:       Srinivasa V
# Created:      JAN 1 2025
# Description:  This File contains Test case scenarios for performing DTC monitoring ON/OFF Operation.
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

from infotest.core.trace_logger import app_logger
import pytest

# ************************************************************* TEST CASES #01 *************************************************
# @pytest.mark.skip(reason="disabling just for debugging purpose")
@pytest.mark.order(91)
def test1_0x5101_DTC_Monitoring_Off(ManDiag_component):
    """ CPV-Diag: DTC monitoring OFF Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of Turning OFF DTC monitoring.
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send Turn OFF DTC monitoring and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition: Send GET DTC monitoring status cmd and validate the response .

                Expected Result:
                - The response should indicate that DTC monitoring has been OFF and No new DTCs should be recorded.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: DTC monitoring OFF Operation feature Validation")
    SetCmd = "6D643E 51 01 01 00 01 00"
    GetCmd = "6D643E 51 01 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="00")

    # Step 3: Post Test Case
    app_logger.info("Executing Post Condition test: GET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="00")

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
# @pytest.mark.skip(reason="disabling just for debugging purpose")
@pytest.mark.order(92)
def test2_0x5101_DTC_Monitoring_On(ManDiag_component):
    """ CPV-Diag: DTC monitoring ON Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
         Test Case: This test case verifies the functionality of Turning OFF DTC monitoring.
         Steps:
         1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
         2. Actual Test: Send Turn ON DTC monitoring and Validate the Response Valid status Byte, Data_length and Data.            
         3. Post-condition: Send GET DTC monitoring status cmd and validate the response .

         Expected Result:
         - The response should indicate that DTC monitoring has been ON and new DTCs should be recorded.
         """
    app_logger.info(test_description)
    app_logger.info("Starting test: DTC monitoring OFF Operation feature Validation")
    SetCmd = "6D643E 51 01 01 00 01 01"
    GetCmd = "6D643E 51 01 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01")

    # Step 3: Post Test Case
    app_logger.info("Executing Post Condition test: GET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="01")

    app_logger.info("Test case passed successfully!")
