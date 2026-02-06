# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x6E05_CyberSec_JTAG_Lock_Unlock.py
# Author:       Srinivasa V
# Created:      19 Nov 2024
# Description:  This File contains Test case scenarios for validating 6E05 JTAG Lock/Unlock.
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


# ************************************************************* TEST CASES #02 *************************************************

@pytest.mark.order(5)
def test_0x6E05_CyberSecurity_SET_GET_validation_for_JTAG_Lock_Operation(ManDiag_component):
    """ CyberSecurity : JTAG Lock Unlock Operation """
    test_description = """
        Test Case: Validate JTAG lock operation
        Steps:
        1. Pre-condition: Check initial JTAG lock status (Get command).
        2. Actual Test: Lock the JTAG with 16bytes unique key  (Set command).            
        3. Validate post-condition: Verify JTAG lock status updated (Get command) .

        Expected Result:
        - Uart should be Locked successfully.
        """
    app_logger.debug(test_description)
    app_logger.info("Starting test: JTAG lock feature Validation")
    GetJTAGStatus = "6D643E 6E 05 00 00 00"

    # Step 1: Pre Condition
    app_logger.info("Checking pre-condition: GET check JTAG Lock Status is UNLOCK-00")
    pre_condition_result = ManDiag_component.send_request_command_and_validate_response(GetJTAGStatus, expected_data_length="01", expected_data="01")

    if pre_condition_result:
        app_logger.warning("Skipping this test - JTAG is already Locked state")
        pytest.skip("JTAG is already Locked state")

    # Step 2: Actual Test
    app_logger.info("Executing actual test: SET LOCK the JTAG")

    JTAG_lockCommand = "6D643E 6E 05 11 00 11 01 7A 25 43 2A 46 2D 4A 61 4E 64 52 67 55 6B 58 70 C4 32"
    app_logger.info("SET lock the JTAG with 16bytes password ")
    assert ManDiag_component.send_request_command_and_validate_response(JTAG_lockCommand, expected_data_length="00", expected_data="91 02")

    # Restart the unit and Enter into mandiag after full ON with 45 sec gap
    assert ManDiag_component.send_request_command_and_validate_response("6D643E 0A 04 01 00 01 01"), f"Failed to restart the unit"
    for i in range(1, 45):
        time.sleep(1)
        app_logger.info(f"Waiting 45 Seconds for VIP Full ON - {i} Secs")
    assert ManDiag_component.send_request_command_and_validate_response("6D643E 00 01 01 00 01 01"), f"Failed to enter ManDiag Session for UART lock Unlock persistence test"

    # Step 3: Post Condition
    app_logger.info("Validating post-condition: GET Check JTAG Lock Status is LOCKED-01")
    assert ManDiag_component.send_request_command_and_validate_response(GetJTAGStatus, expected_data_length="01", expected_data="01")

    app_logger.info("Test case passed successfully!")
