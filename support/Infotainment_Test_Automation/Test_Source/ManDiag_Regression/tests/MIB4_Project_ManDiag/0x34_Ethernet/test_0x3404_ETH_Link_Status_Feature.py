# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x3404_ETH_Link_Status_Feature.py
# Author:       Srinivasa V
# Created:      JAN 1 2025
# Description:  This File contains Test case scenarios for performing Ethernet link status read Operation test.
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
@pytest.mark.mib4_can_0x3304
@pytest.mark.order(411)
def test1_0x3304_ETH_link_status_read_100MB(ManDiag_component):
    """ IOC-Ethernet: ETH link status Operation test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: This test case verifies the functionality of ETH link status.
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send read Ethernet link status and Validate the Response Valid status Byte, Data_length and Data.            
                3. Post-condition: 

                Expected Result:
                - The response should indicate that Ethernet configuration status.
                """
    app_logger.info(test_description)
    app_logger.info("Starting test: ETH link status Operation feature Validation")
    GetCmd = "6D643E 34 04 00 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data="01 01")

    # Step 3: Post Test Case

    app_logger.info("Test case passed successfully!")

