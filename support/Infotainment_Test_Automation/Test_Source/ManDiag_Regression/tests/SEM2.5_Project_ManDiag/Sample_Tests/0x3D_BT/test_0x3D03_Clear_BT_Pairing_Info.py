# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x3D03_Clear_BT_Pairing_Info.py
# Author:       Srinivasa V
# Created:      Aug 28 2025
# Description:  This File contains Test case scenarios for validating 0x3D03 Clear Bluetooth Pairing Information feature.
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
@pytest.mark.skip(reason="Enable this line for Skipping this test case")
# @pytest.mark.order(2)
def test1_0x3D03_Clear_BT_Pairing_Info(ManDiag_component):
    """ Clear BT Pairing Information test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Clear BT Pairing Information
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send command to clear BT Pairing Information.          

                Expected Result:
                - The response should indicate that BT Pairing Information is cleared.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Clear BT Pairing Information test feature Validation")
    SetCmd = "6D643E 3D 03 01 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Clear BT Pairing Information ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01", retries=5)

    # Step 3: Post Condition

    app_logger.info("Test case passed successfully!")
