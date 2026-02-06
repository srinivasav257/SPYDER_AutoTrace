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
# @pytest.mark.skip(reason="disabling just for debugging purpose")
@pytest.mark.order(58)
def test1_0x7801_Components_Hardware_ID(ManDiag_component):
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
    GetCmd = "6D643E 78 01 00 00 01 02"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd,expected_status="01")

    # Step 3: Post Test Case

    

    app_logger.info("Test case passed successfully!")

# ************************************************************* TEST CASES #02 *************************************************
# @pytest.mark.skip(reason="disabling just for debugging purpose")
@pytest.mark.order(58)
def test2_0x7801_Components_Hardware_ID(ManDiag_component):
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
    GetCmd = "6D643E 78 01 00 00 01 02 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd,expected_status="F6")

    # Step 3: Post Test Case

    

    app_logger.info("Test case failed")
	
# ************************************************************* TEST CASES #03 *************************************************
# @pytest.mark.skip(reason="disabling just for debugging purpose")
@pytest.mark.order(58)
def test3_0x7801_Components_Hardware_ID(ManDiag_component):
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
    GetCmd = "6D643E 78 01 00 00 02 02 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd,expected_status="F8")

    

    app_logger.info("Test case failed")
	
# ************************************************************* TEST CASES #04 *************************************************
# @pytest.mark.skip(reason="disabling just for debugging purpose")
@pytest.mark.order(58)
def test4_0x7801_Components_Hardware_ID(ManDiag_component):
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
    GetCmd = "6D643E 78 01 01 00 01 02"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd,expected_status="F7")

  
    

    app_logger.info("Test case failed")