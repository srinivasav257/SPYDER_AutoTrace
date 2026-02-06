# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x5B02_TCU_Gyro_Accelerometer.py
# Author:       Srinivasa V
# Created:      Sept 02 2024
# Description:  This File contains Test case scenarios for validating Gyro_Accelerometer values.
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
@pytest.mark.hpcc_tcu
@pytest.mark.order(1)
def test_0x5B02_TCU_Gyro_Accelerometer_Values(ManDiag_component):
    """ TCU : Read Gyro Accelerometer data """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Reading Gyro_Accelerometer_Value.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.
                Step 2. Actual Test
                - Send GET Read Gyro_Accelerometer Values and Validate the Response Valid status Byte, Data_length and Data.
                Step 3: Post-condition

            Expected Result:
                - The read Gyro_Accelerometer Values should match to expected data.
            """
    app_logger.debug(test_description)

    GetCommandGyro = "6D643E 5B 02 00 00 00"
    expected_data_length = "0D"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.debug("Executing actual test: Read and verify Gyro Accelerometer Values")
    ManDiag_response = ManDiag_component.send_request_command_and_get_response(GetCommandGyro, retries=2)
    if ManDiag_response and ManDiag_response['Status'] == "01" and ManDiag_response['DataLength'] == "0D":
        if ManDiag_response['Databytes'] == "00 00 00 00 00 00 00 00 00 00 00 00 00":
            assert False, f"Actual test failed for GetCommandGyro : received signal gain is {ManDiag_response['Databytes']} not acceptable"
    else:
        assert False, f"Actual test failed for GetCommandGyro: Invalid Response"

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")
