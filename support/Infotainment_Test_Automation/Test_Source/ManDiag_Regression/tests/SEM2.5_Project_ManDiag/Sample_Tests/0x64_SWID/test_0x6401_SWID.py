# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x6401_SWID.py
# Author:       Srinivasa V
# Created:      Aug 29 2025
# Description:  This File contains Test case scenarios for validating 0x6401 Software ID.
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
def test1_0x6401_SWID(ManDiag_component):
    """ SWID Test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Software ID
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send GET command to Get Software ID.          
                3. Post-condition:

                Expected Result:
                - The response should indicate that Software ID of the component is returned successfully.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Get Software ID test feature Validation")
    GetCmd = "6D643E 64 01 00 00 01 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Get Software ID ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_prefix="00", retries=2)

    # Step 3: Post Condition

    app_logger.info("Test case passed successfully!")
