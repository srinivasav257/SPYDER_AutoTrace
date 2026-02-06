# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x0004_erminal_Filter_OnOff_Feature.py
# Author:       Srinivasa V
# Created:      JAN 1 2025
# Description:  This File contains Test case scenarios for performing Terminal filter ON/OFF Operation test.
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
# @pytest.mark.skip(reason="disabling just for debugging purpose")
@pytest.mark.order(6)
def test1_0x6601_EEPROM_Read_Write(ManDiag_component):
    """ IOC-ManDiag : EEPROM Read Write Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of Turning Terminal filter ON .
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send Turn Terminal filter ON  and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition: Send GET Terminal filter status cmd and validate the response.

                Expected Result:
                - The response should indicate that Terminal filter has been turned ON and No traces apart from ManDIag request/response should appear on serial logger.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: Terminal filter ON Operation feature Validation")
    SetCmd = "6D643E 66 01 01 00 0B 00 02 20 30 06 AA BB CC DD EE FF"
	GetCmd = "6D643E 66 01 00 00 05 00 02 20 30 06"
    # Step 1: Pre_Condition Test
    #Sleep trigger
    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd,expected_status="01")

    # Step 3: Post Test Case
	assert ManDiag_component.send_request_command_and_validate_response(GetCmd,expected_status="01")
	 
    app_logger.info("Test case passed successfully!")

# ************************************************************* TEST CASES #02 *************************************************
# @pytest.mark.skip(reason="disabling just for debugging purpose")
@pytest.mark.order(6)
def test2_0x6601_EEPROM_Read_Write(ManDiag_component):
    """ IOC-ManDiag : EEPROM Read Write Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of Turning Terminal filter ON .
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send Turn Terminal filter ON  and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition: Send GET Terminal filter status cmd and validate the response.

                Expected Result:
                - The response should indicate that Terminal filter has been turned ON and No traces apart from ManDIag request/response should appear on serial logger.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: Terminal filter ON Operation feature Validation")
    SetCmd = "6D643E 66 01 01 00 0A 00 02 20 30 06 AA BB CC DD EE FF"
    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd,expected_status="F6")

    # Step 3: Post Test Case
	 
    app_logger.info("Test case Failed")

