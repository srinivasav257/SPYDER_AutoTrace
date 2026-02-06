# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x5016_TCU_eSIM_Malfunction.py
# Author:       Srinivasa V
# Created:      Sept 02 2024
# Description:  This File contains Test case scenarios for validating eSIM malfunction status.
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


# ************************************************************* TEST CASES #01*************************************************
@pytest.mark.hpcc_tcu
@pytest.mark.order(20)
def test_0x5016_Read_eSIM_Malfunction_Status(ManDiag_component):
    """ TCU : 50 15 eSIM Malfunction """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Reading eSIM malfunction status.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.
                Step 2. Actual Test
                - Send GET Read eSIM malfunction status and Validate the Response Valid status Byte, Data_length and Data.
                Step 3: Post-condition

            Expected Result:
                - The read eSIM malfunction status should match to expected data.
            """
    app_logger.debug(test_description)
    GetCommand = "6D643E 50 16 00 00 00"
    expected_data_length = "01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Read and verify eSIM malfunction status")
    assert ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length, expected_data="00", retries=2)

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")
