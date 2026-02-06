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

from infotest.core.trace_logger import app_logger
import pytest

# ************************************************************* TEST CASES #01 *************************************************
@pytest.mark.mib4
@pytest.mark.mib4_can_0x33
@pytest.mark.order(301)
def test1_0x3301_CAN_Dummy_Msg_MIBCAN(ManDiag_component):
    """ IOC-CAN: CAN Dummy msg for MIB_CAN Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of start sending Dummy message for MIB_CAN.
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send start sending Dummy message for MIB_CAN and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition: Send stop sending Dummy message for MIB_CAN and Validate the Response Valid status Byte, Data_length and Data. 

                Expected Result:
                - The response should indicate that dummy msg has been broad casted in MIB_CAN and Same should be verified on CANoe terminal.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: CAN monitoring OFF Operation feature Validation")
    SetCmdstart = "6D643E 33 01 01 00 02 01 01"
    SetCmdstop  = "6D643E 33 01 01 00 02 01 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmdstart, expected_data_length="02", expected_data="01 01")

    # Step 3: Post Test Case
    app_logger.info("Executing Post Condition test: GET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmdstop, expected_data_length="02", expected_data="01 00")

    app_logger.info("Test case passed successfully!")

# ************************************************************* TEST CASES #02 *************************************************
@pytest.mark.mib4
@pytest.mark.mib4_can_0x3301
@pytest.mark.order(302)
def test2_0x3301_CAN_Dummy_Msg_ICAN(ManDiag_component):
    """ IOC-CAN: CAN Dummy msg for ICAN Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of start sending Dummy message for I_CAN.
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send start sending Dummy message for I_CAN and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition: Send stop sending Dummy message for I_CAN and Validate the Response Valid status Byte, Data_length and Data. 

                Expected Result:
                - The response should indicate that dummy msg has been broad casted in I_CAN and Same should be verified on CANoe terminal.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: CAN monitoring OFF Operation feature Validation")
    SetCmdstart = "6D643E 33 01 01 00 02 02 01"
    SetCmdstop  = "6D643E 33 01 01 00 02 02 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmdstart, expected_data_length="02", expected_data="02 01")

    # Step 3: Post Test Case
    app_logger.info("Executing Post Condition test: GET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmdstop, expected_data_length="02", expected_data="02 00")

    app_logger.info("Test case passed successfully!")
