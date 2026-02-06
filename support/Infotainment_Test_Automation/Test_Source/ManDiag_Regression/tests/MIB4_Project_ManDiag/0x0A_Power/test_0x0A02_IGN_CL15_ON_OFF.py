# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x0A02_IGN_CL15_ON_OFF.py
# Author:       Srinivasa V
# Created:      Jan 02 2025
# Description:  This File contains Test case scenarios for validating 0x0A02 IGN CL15 ON/OFF feature.
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


# ************************************************************* TEST CASES #01 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0A02
@pytest.mark.order(111)
def test1_0x0A02_IGN_CL15_Off_FeatureTest(ManDiag_component):
    """ IOC Power mode : IGN_CL15 OFF test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate IGN_CL15 OFF
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to turn IGN_CL15 OFF.          
                3. Post-condition: Send GET command to verify the OFF status.

                Expected Result:
                - The response should indicate that Phantom voltage of IGN_CL15 is powered OFF.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: IGN_CL15 OFF test feature Validation")
    SetCmd = "6D643E 0A 02 01 00 01 00"
    GetCmd = "6D643E 0A 02 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn OFF the IGN_CL15 ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="00",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get IGN_CL15 status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="00", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0A02
@pytest.mark.order(112)
def test2_0x0A02_IGN_CL15_On_FeatureTest(ManDiag_component):
    """ IOC Power mode : IGN_CL15 ON test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate IGN_CL15 ON
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to turn IGN_CL15 ON.          
                3. Post-condition: Send GET command to verify the OFF status.

                Expected Result:
                - The response should indicate that Phantom voltage of Radio is powered ON.
                """
    app_logger.debug(test_description)
    SetCmd = "6D643E 0A 02 01 00 01 01"
    GetCmd = "6D643E 0A 02 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn ON the IGN_CL15 ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get IGN_CL15 status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="01", retries=2)

    app_logger.info("Test case passed successfully!")
