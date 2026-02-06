# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x3405_EthernetIP.py
# Author:       Srinivasa V
# Created:      2 Sept 2025
# Description:  This File contains Test case scenarios for validating 0x3405 Ethernet IP.
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
def test1_0x3405_EthernetIP(ManDiag_component):
    """ Set Ethernet IP Test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Set Ethernet IP
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET command to Set Ethernet IP.          
                3. Post-condition:

                Expected Result:
                - The response should indicate that the IP address of the interface is changed successfully.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Set Ethernet IP test feature Validation")
    SetCmd = "6D643E 34 05 01 00 09 01 C0 A8 01 22 FF FF FF 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Set Ethernet IP ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="00", retries=2)

    # Step 3: Post Condition

    app_logger.info("Test case passed successfully!")
