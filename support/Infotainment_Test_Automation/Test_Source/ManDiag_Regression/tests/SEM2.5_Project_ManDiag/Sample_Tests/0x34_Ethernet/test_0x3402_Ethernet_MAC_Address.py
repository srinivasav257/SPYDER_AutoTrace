# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x3402_Ethernet_MAC_Address.py
# Author:       Srinivasa V
# Created:      1 Sept 2025
# Description:  This File contains Test case scenarios for validating 0x3402 Ethernet MAC Address feature.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2024 APTIV
#
# This file is part of the Manufacture Diagnostics Component.
#
# Manufacture Diagnostics Project is free software: you can redistribute it
# and/or modify it under the terms of Diagnostics Department ASUX.
# ------------------------------------------------------------------------------------------------------------------

import time

from infotest.core.trace_logger import app_logger
import pytest


# ************************************************************* TEST CASES #01 *************************************************
#@pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(7)
def test1_0x3402_Ethernet_MAC_Address(ManDiag_component):
    """ Ethernet MAC Address test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Set Ethernet MAC Address
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to Set Ethernet MAC Address.          
                3. Post-condition: Send GET command to verify the Ethernet MAC Address.

                Expected Result:
                - The response should indicate that Ethernet MAC Address is set successfully.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Set Ethernet MAC Address test feature Validation")
    SetCmd = "6D643E 34 02 01 00 07 01 AA DD AA DD AA DD"
    GetCmd = "6D643E 34 02 00 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Set Ethernet MAC Address ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="07", expected_data="01 AA DD AA DD AA DD", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get Ethernet MAC Address ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="07", expected_data="01 AA DD AA DD AA DD", retries=2)

    app_logger.info("Test case passed successfully!")
