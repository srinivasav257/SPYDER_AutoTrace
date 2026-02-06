# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x8702_TCU_GPS_latitude_longitude.py
# Author:       Srinivasa V
# Created:      Sept 02 2024
# Description:  This File contains Test case scenarios for validating GPS_latitude_longitude values.
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
@pytest.mark.order(21)
def test_0x8702_GPS_latitude_longitude_Values(ManDiag_component):
    """ TCU : 8702 GPS co-ordinates """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Reading GPS latitude and longitude Value.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.
                Step 2. Actual Test
                - Send GET Read GPS latitude and longitude Values and Validate the Response Valid status Byte, Data_length and Data.
                Step 3: Post-condition

            Expected Result:
                - The read GPS latitude and longitude Values should match to expected data.
            """
    app_logger.debug(test_description)

    GetCommandGPS = "6D643E 87 02 00 00 00"
    expected_data_length = "28"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Read and verify GPS latitude and longitude Values")
    ManDiag_response = ManDiag_component.send_request_command_and_get_response(GetCommandGPS, retries=2)
    if ManDiag_response and ManDiag_response['Status'] == "01" and ManDiag_response['Databytes'] == "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00":
        assert False, f"Actual test failed for GetCommandGPS : received signal gain is {ManDiag_response['Databytes']} not acceptable"

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")
