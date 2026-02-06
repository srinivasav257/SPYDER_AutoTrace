# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x1401_Audio_Volume.py
# Author:       Srinivasa V
# Created:      Jan 02 2025
# Description:  This File contains Test case scenarios for validating Audio feature.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2024 APTIV
#
# This file is part of the MIB4 project - Manufacture Diagnostics Component.
#
# Manufacture Diagnostics Project is free software: you can redistribute it
# and/or modify it under the terms of Diagnostics Department ASUX.
# ------------------------------------------------------------------------------------------------------------------

import time

from infotest.core.trace_logger import app_logger
import pytest

# ************************************************************* TEST CASES #01 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x1401
@pytest.mark.order(201)
def test1_0x1401_Audio_Volume_Set_Min_Value_0x00(ManDiag_component):
    """ QNX-Audio: Set_Min_Value_0x00 """

    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of ---.

            Test Sequence Steps:
                Step 1: Pre-condition
                - 
                Step 2. Actual Test
                -        
                Step 3: Post-condition
                -

            Expected Result:
                - 
            """
    app_logger.debug(test_description)
    SetCmd = "6D643E 14 01 01 00 01 00"
    GetCmd = "6D643E 14 01 00 00 00"

    # Step 1: Pre Condition Test
    app_logger.info("Executing Pre Conditional test: Read and verify ")

    # Step 2: Actual Test
    app_logger.info("Executing actual test: Read and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="00",  retries=2)

    # Step 3: Post Condition Test
    app_logger.info("Executing Post test: Read and verify ")
    assert ManDiag_component.send_and_validate_command(GetCmd, expected_data_length="01", expected_data="00", retries=2)

    app_logger.info("Test case passed successfully!")
