# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x5105_DTC_ExtendedData_Feature.py
# Author:       Srinivasa V
# Created:      Jan 02 2025
# Description:  This File contains Test case scenarios for validating BT feature.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2024 APTIV
#
# This file is part of the MnM HPCC project - Manufacture Diagnostics Component.
#
# Manufacture Diagnostics Project is free software: you can redistribute it
# and/or modify it under the terms of Diagnostics Department ASUX.
# ------------------------------------------------------------------------------------------------------------------

import time

from infotest.core.trace_logger import app_logger
import pytest


# ************************************************************* TEST CASES #01 *************************************************
# @pytest.mark.skip(reason="disabling just for debugging purpose")
@pytest.mark.order(95)
def test_0x5105_Read_DTCs_ExtendedData_InvalidControlData(ManDiag_component):
    """ IOC-Diag: read extended data dor Invalid control Data """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
         Test Case: This test case verifies the functionality of reading Active DTCs List.
         Steps:
         1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
         2. Actual Test: Send Get Extended DTC Data for given DTC number and Validate the Response Valid status Byte, Data_length and Data.            
         3. Post-condition:

         Expected Result:
         - The response should indicate the all the active DTCs being recorded.
         """
    app_logger.info(test_description)
    app_logger.info("Starting test: DTC extended data feature Validation")
    GetCmd = " 6D643E 51 05 00 00 03 00 06 89"

    # Step 1: Pre_Condition Test
    app_logger.info("Executing pre Condition test: SET and verify ")

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: GET and verify ")
    response_data = ManDiag_component.send_request_command_and_get_response(GetCmd)
    assert response_data["Status"] == "01" and response_data["DataLength"] == "28"

    # Step 3: Post Test Case
    app_logger.info("Executing Post Condition test: GET and verify ")

    app_logger.info("Test case passed successfully!")
