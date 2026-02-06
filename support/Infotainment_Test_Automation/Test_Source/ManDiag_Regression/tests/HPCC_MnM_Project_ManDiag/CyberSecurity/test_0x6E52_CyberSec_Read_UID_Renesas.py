# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x6E52_CyberSec_Read_UID_Renesas.py
# Author:       Srinivasa V
# Created:      19 Nov 2024
# Description:  This File contains Test case scenarios for validating 6E 52 Cyber Security JTAG lock UID for
#               Renesas.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2024 APTIV
#
# This file is part of the MnM HPCC project - Manufacture Diagnostics Component.
#
# Manufacture Diagnostics Project is free software: you can redistribute it
# and/or modify it under the terms of Diagnostics Department ASUX.
# ------------------------------------------------------------------------------------------------------------------


import time
import pytest
from infotest.core.trace_logger import app_logger


# ************************************************************* TEST CASES #01 *************************************************
@pytest.mark.order(11)
def test_0x6E52_CyberSecurity_GET_validation_for_JTAG_LockUID(ManDiag_component):
    """ CyberSecurity : read JTAG Lock UID"""
    test_description = """
        Test Case: Validate 5G JTAG status
        Steps:
        1. Pre-condition: 
        2. Read JTAG lock UID for Renesas and validate with expectation (Get command).
        3. Validate post-condition:

        Expected Result:
        - De-Provisioning process completes successfully.
        """
    app_logger.debug(test_description)
    app_logger.info("Starting test: JTAG lock UID for Renesas key validation")
    GetCommand = "6D643E 6E 52 00 00 00"

    # Step 1: Pre Condition

    # Step 2: Actual Test
    app_logger.info("Executing actual test: GET Read TAG lock UID for Renesas and validate with expectation")
    assert ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="0F", expected_data="4E 42 4D 48 35 31 2D 35 31 20 09 FF 58 00 8A", retries=2)

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")
