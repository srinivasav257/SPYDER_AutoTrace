"""
=====================================================================================================================
File Name   : test_0x5015_TCU_IMU_Malfunction.py
Description : his File contains Test case scenarios for validating IMU malfunction status.
Author      : Srinivasa V
Created On  : 2025-01-01

Copyright© 2025 APTIV - This file is part of the MnM HPCC project - Manufacture Diagnostics Component.
Manufacture Diagnostics Project is free software: you can redistribute it and/or modify it under the terms of Diagnostics
Department ASUX.

======================================================================================================================
"""
import time

from infotest.core.trace_logger import app_logger
import pytest


# ************************************************************* TEST CASES #01 *************************************************
@pytest.mark.hpcc_tcu
@pytest.mark.order(19)
def test_0x5015_Read_IMU_Malfunction_Status(ManDiag_component):
    """ TCU : 50 15 IMU Malfunction """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Reading IMU malfunction status.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.
                Step 2. Actual Test
                - Send GET Read IMU malfunction status and Validate the Response Valid status Byte, Data_length and Data.
                Step 3: Post-condition

            Expected Result:
                - The read IMU malfunction status should match to expected data.
            """
    app_logger.debug(test_description)

    GetCommand = "6D643E 50 15 00 00 00"
    expected_data_length = "01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Read and verify IMU malfunction status")
    assert ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length, expected_data="00", retries=2)

    # Step 3: Post Condition

"""
Revision History:
|---------------------------------------------------------------------------------------|
| Version | Date       | Author            | Changes                                    |
|---------|------------|-------------------|--------------------------------------------|
| 1.0     | 2025-01-01 | Srinivas V        | Initial release                            |

"""
