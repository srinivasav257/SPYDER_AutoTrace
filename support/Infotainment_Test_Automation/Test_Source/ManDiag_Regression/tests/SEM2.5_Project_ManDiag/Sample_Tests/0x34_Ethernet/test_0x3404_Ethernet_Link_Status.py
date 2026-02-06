# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x3404_Ethernet_Link_Status.py
# Author:       Srinivasa V
# Created:      2 Sept 2025
# Description:  This File contains Test case scenarios for validating 0x3404 Ethernet Link Status.
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
def test1_0x3404_Ethernet_Link_Status(ManDiag_component):
    """ GET BRR1 Link Status Test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate GET BRR1 Link status
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send GET command to GET BRR1 Link status.          
                3. Post-condition:

                Expected Result:
                - The response should indicate that Link Status of BRR1 is returned successfully.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: GET BRR1 Link status test feature Validation")
    GetCmd = "6D643E 34 04 00 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: GET BRR1 Link status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data_prefix="01", retries=2)

    # Step 3: Post Condition

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
#@pytest.mark.skip(reason="Enable this line for Skipping this test case")
#@pytest.mark.order(7)
def test2_0x3404_Ethernet_Link_Status(ManDiag_component):
    """ GET BRR2 Link status Test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate GET BRR2 Link status
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send GET command to GET BRR2 Link status.          
                3. Post-condition:

                Expected Result:
                - The response should indicate that Link Status of BRR2 is returned successfully.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: GET BRR2 Link status test feature Validation")
    GetCmd = "6D643E 34 04 00 00 01 02"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: GET BRR2 Link status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data_prefix="02", retries=2)

    # Step 3: Post Condition

    app_logger.info("Test case passed successfully!")
