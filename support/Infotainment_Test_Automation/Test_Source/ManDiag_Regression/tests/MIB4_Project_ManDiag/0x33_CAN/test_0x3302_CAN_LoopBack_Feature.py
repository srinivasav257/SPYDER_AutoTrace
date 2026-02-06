# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x3302_CAN_LoopBack_Feature.py
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
@pytest.mark.mib4_can_0x3302
@pytest.mark.order(311)
def test1_0x3302_CAN_LoopBack_Msg_MIBtoICAN(ManDiag_component):
    """ IOC-CAN: CAN loopback msg for MIBtoICAN Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of start sending loopback message for MIBtoICAN.
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send start sending loopback message for MIBtoICAN and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition:  

                Expected Result:
                - The response should indicate that loopback msg has been broad casted from MIB_CAN to ICAN and Same should be verified on CANoe terminal.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: loopback msg for MIBtoICAN Operation feature Validation")
    SetCmd = " 6D643E 33 02 00 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01")

    # Step 3: Post Test Case

    app_logger.info("Test case passed successfully!")

# ************************************************************* TEST CASES #02 *************************************************
@pytest.mark.mib4
@pytest.mark.mib4_can_0x3302
@pytest.mark.order(312)
def test2_0x3302_CAN_LoopBack_Msg_InternalCAN(ManDiag_component):
    """ IOC-CAN: CAN loopback msg for MIBtoICAN Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of start sending loopback message for Internal_CAN.
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send start sending loopback message for Internal_CAN and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition: 

                Expected Result:
                - The response should indicate that loopback msg has been broad casted from Internal_CAN to Internal_CAN and Same should be verified on CANoe terminal.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: loopback msg for Internal_CAN Operation feature Validation")
    SetCmd = " 6D643E 33 02 00 00 01 02"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01")

    # Step 3: Post Test Case

    app_logger.info("Test case passed successfully!")

