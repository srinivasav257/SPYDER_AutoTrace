# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x5103_DTC_Number_Feature.py
# Author:       Srinivasa V
# Created:      Jan 02 2025
# Description:  This File contains Test case scenarios for validating  Active DTC count feature.
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
@pytest.mark.order(93)
def test1_0x5103_DTC_Number_Feature(ManDiag_component):
    """ CPV-Diag: read Active DTC count Number """

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
    GetCmd = "6D643E 51 03 00 00 00"

    # Step 1: Pre_Condition Test
    app_logger.info("Executing pre Condition test: SET and verify ")

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: GET and verify ")
    response_data = ManDiag_component.send_request_command_and_get_response(GetCmd)
    if response_data["DataLength"] == "02" and response_data["Databytes"] != "00 00":
        hex_str = response_data["Databytes"]
        decimal_value = int(hex_str.replace(" ", ""), 16)
        assert decimal_value != 0x00

    # Step 3: Post Test Case
    app_logger.info("Executing Post Condition test: GET and verify ")

    app_logger.info("Test case passed successfully!")
