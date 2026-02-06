# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x340A_Ethernet_Remote_Loopback.py
# Author:       Srinivasa V
# Created:      1 Sept 2025
# Description:  This File contains Test case scenarios for validating 0x340A Ethernet Remote Loopback feature.
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
def test1_0x340A_Ethernet_Remote_Loopback(ManDiag_component):
    """ Enable Remote loopback mode for BRR1 test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Enable Remote loopback mode for BRR1
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to enable Remote loopback mode for BRR1.          
                3. Post-condition: Send GET command to verify the status.

                Expected Result:
                - The response should indicate that Remote loopback mode for BRR1 is enabled.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Enable Remote loopback mode for BRR1 test feature Validation")
    SetCmd = "6D643E 34 0A 01 00 02 01 01"
    GetCmd = "6D643E 34 0A 00 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Enable Remote loopback mode for BRR1 ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="02", expected_data="01 01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get Remote Loopback status for BRR1 ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data="01 01", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
#@pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(7)
def test2_0x340A_Ethernet_Remote_Loopback(ManDiag_component):
    """ Enable Remote loopback mode for BRR2 test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Enable Remote loopback mode for BRR2
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to enable Remote loopback mode for BRR2.          
                3. Post-condition: Send GET command to verify the status.

                Expected Result:
                - The response should indicate that Remote loopback mode for BRR2 is enabled.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Enable Remote loopback mode for BRR2 test feature Validation")
    SetCmd = "6D643E 34 0A 01 00 02 02 01"
    GetCmd = "6D643E 34 0A 00 00 01 02"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Enable Remote loopback mode for BRR2 ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="02", expected_data="02 01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get Remote Loopback status for BRR2 ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data="02 01", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #03 *************************************************
#@pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(8)
def test3_0x340A_Ethernet_Remote_Loopback(ManDiag_component):
    """ Disable Remote loopback mode for BRR1 test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Disable Remote loopback mode for BRR1
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to disable Remote loopback mode for BRR1.          
                3. Post-condition: Send GET command to verify the status.

                Expected Result:
                - The response should indicate that Remote loopback mode for BRR1 is disabled.
                """
    app_logger.debug(test_description)
    SetCmd = "6D643E 34 0A 01 00 02 01 00"
    GetCmd = "6D643E 34 0A 00 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Disable Remote loopback mode for BRR1 ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="02", expected_data="01 00",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get Remote Loopback status for BRR1 ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data="01 00", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #04 *************************************************
#@pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(8)
def test4_0x340A_Ethernet_Remote_Loopback(ManDiag_component):
    """ Disable Remote loopback mode for BRR2 test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Disable Remote loopback mode for BRR2
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to disable Remote loopback mode for BRR2.          
                3. Post-condition: Send GET command to verify the status.

                Expected Result:
                - The response should indicate that Remote loopback mode for BRR2 is disabled.
                """
    app_logger.debug(test_description)
    SetCmd = "6D643E 34 0A 01 00 02 02 00"
    GetCmd = "6D643E 34 0A 00 00 01 02"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Disable Remote loopback mode for BRR2 ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="02", expected_data="02 00",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Get Remote Loopback status for BRR2 ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data="02 00", retries=2)

    app_logger.info("Test case passed successfully!")
