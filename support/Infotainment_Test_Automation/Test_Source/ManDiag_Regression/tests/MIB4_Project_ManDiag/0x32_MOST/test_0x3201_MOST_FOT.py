# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x0A0A_Phantom_FM_ON_OFF.py
# Author:       Srinivasa V
# Created:      Jan 02 2025
# Description:  This File contains Test case scenarios for validating 0x0A0A FM ON/OFF feature.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2024 APTIV
#
# This file is part of the MIB4 project - Manufacture Diagnostics Component.
#
# Manufacture Diagnostics Project is free software: you can redistribute it
# and/or modify it under the terms of Diagnostics Department ASUX.
# ------------------------------------------------------------------------------------------------------------------

import time

from framework_modules.core.trace_logger import app_logger
import pytest


# ************************************************************* TEST CASES #01 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
@pytest.mark.order(40)
def test1_0x3201_MOST_FOT_enable_disable_FeatureTest(ManDiag_component):
    """ IOC Power mode : MOST FOT enable test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: MOST FOT enable
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET MOST FOT enable command         
                3. Post-condition: Send GET command to verify the status.

                Expected Result:
                - The response should indicate that MOST FOT is enabled
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: MOST FOT enable test feature Validation")
    SetCmd = "6D643E 32 01 01 00 01 01"
    GetCmd = "6D643E 32 01 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: MOST FOT enable")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="01", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
# @pytest.mark.skip(reason="Enable this line for Skipping this test case")
@pytest.mark.order(40)
def test2_0x3201_MOST_FOT_enable_disable_FeatureTest(ManDiag_component):
    """ IOC Power mode : MOST FOT disable test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: MOST FOT disable
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET MOST FOT disable command         
                3. Post-condition: Send GET command to verify the status.

                Expected Result:
                - The response should indicate that MOST FOT is disabled
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: MOST FOT disable test feature Validation")
    SetCmd = "6D643E 32 01 01 00 01 00"
    GetCmd = "6D643E 32 01 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: MOST FOT disable")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="00",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="00", retries=2)

    app_logger.info("Test case passed successfully!")
