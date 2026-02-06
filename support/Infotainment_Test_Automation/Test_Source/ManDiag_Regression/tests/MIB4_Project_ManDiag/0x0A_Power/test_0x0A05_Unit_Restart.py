# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x0A05_Unit_Restart.py
# Author:       Srinivasa V
# Created:      Jan 02 2025
# Description:  This File contains Test case scenarios for validating 0x0A05 Unit restart feature.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2024 APTIV
#
# This file is part of the MnM HPCC project - Manufacture Diagnostics Component.
#
# Manufacture Diagnostics Project is free software: you can redistribute it
# and/or modify it under the terms of Diagnostics Department ASUX.
# ------------------------------------------------------------------------------------------------------------------

import time

from infotest.core.trace_logger import app_logger
import pytest
restart_flag = False

# ************************************************************* TEST CASES #01 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0A05
@pytest.mark.order(141)
def test1_0x0A05_UnitRestart(ManDiag_component, PowerSupply_TENMA):
    """ IOC Power mode : Unit Restart Normal mode"""
    # Step 0: Update the Test variable and Test Procedure
    global restart_flag
    test_description = """
                Test Case: Unit Restart Normal mode
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET Unit restart in normal mode.          
                3. Post-condition: Read ManDiag session and verify the exit status.

                Expected Result:
                - The response should indicate that Unit is requested for Normal restart and within 30 Sec restart should complete
                  and unit should come to full ON state.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Unit restart normal mode feature Validation")
    SetCmd = "6D643E 0A 05 01 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Unit restart in Normal mode ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: wait for 30sec and verify ManDiag session exited and SET enter ManDiag session")

    # Monitor Current consumption drops below 1Amps
    for i in range(1, 45):
        time.sleep(1)
        current = PowerSupply_TENMA.get_current()
        if current < 1.00:
            restart_flag = True
        app_logger.info(f"Current consumption at {i} Secs - {current} mA")

    # Check ManDiag Session state
    if restart_flag:
        assert ManDiag_component.send_request_command_and_validate_response("6D643E 00 01 00 00 00", expected_data_length="01", expected_data="00", retries=2)
        assert ManDiag_component.send_request_command_and_validate_response("6D643E 00 01 01 00 01 01", retries=2)
        assert ManDiag_component.send_request_command_and_validate_response("6D643E 00 01 00 00 00", expected_data_length="01", expected_data="01", retries=2)
    else:
        assert False, f"during restart the current consumption didn't go below 1Amps"
    app_logger.info("Test case passed successfully!")
