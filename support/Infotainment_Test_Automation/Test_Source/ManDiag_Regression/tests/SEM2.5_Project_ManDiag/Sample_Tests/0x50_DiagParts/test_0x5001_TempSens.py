# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x5001_TempSens.py
# Author:       Srinivasa V
# Created:      1 Sept 2025
# Description:  This File contains Test case scenarios for validating 0x5001 Temperature Control Component.
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
def test1_0x5001_TempSens(ManDiag_component):
    """ Get Temperature of Mainboard Sensor Test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Get Temperature of Mainboard Sensor
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send GET command to Get Temperature of Mainboard Sensor.          
                3. Post-condition:

                Expected Result:
                - The response should print the Temperature of Mainboard Sensor.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Get Temperature of Mainboard Sensor")
    GetCmd = "6D643E 50 01 00 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Get Temperature of Mainboard Sensor ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="03", expected_data_prefix="01", retries=2)

    # Step 3: Post Condition

    app_logger.info("Test case passed successfully!")
