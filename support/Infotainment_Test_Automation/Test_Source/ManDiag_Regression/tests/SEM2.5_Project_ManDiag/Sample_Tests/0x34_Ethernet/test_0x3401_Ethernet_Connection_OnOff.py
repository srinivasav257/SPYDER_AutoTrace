# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x3401_Ethernet_Connection_OnOff.py
# Author:       Srinivasa V
# Created:      Aug 29 2025
# Description:  This File contains Test case scenarios for validating 0x3401 Ethernet Connection ON/OFF feature.
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
def test1_0x3401_Ethernet_Connection_OnOff(ManDiag_component):
    """ Set BRR1 ON test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Set BRR1 ON
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to Set BRR1 ON.          
                3. Post-condition: Send GET command to verify the ON status.

                Expected Result:
                - The response should indicate that BRR1 is powered ON.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Set BRR1 ON test feature Validation")
    SetCmd = "6D643E 34 01 01 00 02 01 01"
    GetCmd = "6D643E 34 01 00 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Set BRR1 ON ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="02", expected_data="01 01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get BRR1 status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data="01 01", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
#@pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(8)
def test2_0x3401_Ethernet_Connection_OnOff(ManDiag_component):
    """ Set BRR2 ON test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Set BRR2 ON 
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to Set BRR2 ON.          
                3. Post-condition: Send GET command to verify the ON status.

                Expected Result:
                - The response should indicate that BRR2 is powered ON.
                """
    app_logger.debug(test_description)
    SetCmd = "6D643E 34 01 01 00 02 02 01"
    GetCmd = "6D643E 34 01 00 00 01 02"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn ON BRR2 ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="02", expected_data="02 01",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get BRR2 status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data="02 01", retries=2)

    app_logger.info("Test case passed successfully!")
