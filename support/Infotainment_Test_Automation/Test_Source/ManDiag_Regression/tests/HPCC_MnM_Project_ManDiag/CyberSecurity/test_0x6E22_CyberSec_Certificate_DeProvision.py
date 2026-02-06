# -----------------------------------------------------------------------------------------------------
# File Name:    test_0x6E22_CyberSec_Certificate_DeProvision.py
# Author:       Srinivasa V
# Created:      19 Nov 2024
# Description:  This File contains Test case scenarios for validating 6E 22 Cyber Security Certificate
#               De-Provisioning.
# -----------------------------------------------------------------------------------------------------
# Copyright (C) 2024 APTIV
#
# This file is part of the MnM HPCC project - Manufacture Diagnostics Component.
#
# Manufacture Diagnostics Project is free software: you can redistribute it
# and/or modify it under the terms of Diagnostics Department ASUX.
# ------------------------------------------------------------------------------------------------------

import time
import pytest
from infotest.core.trace_logger import app_logger


# ************************************************************* TEST CASES #01 *************************************************
@pytest.mark.order(10)
def test_0x6E22_CyberSecurity_SET_GET_validation_for_5G_Certificate_DeProvision(ManDiag_component):
    """ CyberSecurity : 5G Certificate De-Provision """
    test_description = """
        Test Case: Validate 5G Certificate De-Provisioning
        Steps:
        1. Pre-condition: Check initial De-Provision status (Get command).
        2. Execute De-Provisioning (Set command).
        3. Validate post-condition: Verify De-Provision status updated (0x03, 0x02).

        Expected Result:
        - De-Provisioning process completes successfully.
        """
    app_logger.debug(test_description)
    app_logger.info("Starting test: 5G Certificate De-Provisioning Validation")
    GetCommand = "6D643E 6E 22 00 00 01 03"
    SetCommand = "6D643E 6E 22 01 00 01 03"

    # Step 1: Pre Condition
    app_logger.info("Checking pre-condition: GET DeProvision status for 5G chip")
    pre_condition_result = ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="02", expected_data="03 02", retries=2)
    if pre_condition_result:
        app_logger.warning("Skipping this test - 5G is already De-provisioned")
        pytest.skip("5G is Not Provisioned")

    # Step 2: Actual Test
    app_logger.info("Executing actual test: SET De-Provisioning for 5G chip")
    assert ManDiag_component.send_request_command_and_validate_response(SetCommand, expected_data_length="02", expected_data="03 01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Validating post-condition: GET De-provision status")
    assert ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="02", expected_data="03 02", retries=2)

    app_logger.info("Test case passed successfully!")
