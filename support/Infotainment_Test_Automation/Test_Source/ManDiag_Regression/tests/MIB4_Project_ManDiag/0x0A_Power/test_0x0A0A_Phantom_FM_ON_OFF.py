# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x0A0A_Phantom_FM_ON_OFF.py
# Author:       Srinivasa V
# Created:      Jan 02 2025
# Description:  This File contains Test case scenarios for validating 0x0A0A FM ON/OFF feature.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2024 APTIV Diagnostics Department ASUX.
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
@pytest.mark.mib4_System_0x0A0A
@pytest.mark.order(181)
def test1_0x0A0A_Phantom_FM_Off_FeatureTest(ManDiag_component):
    """ IOC Power mode : Phantom_FM OFF test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Phantom_FM OFF
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to turn FM OFF.          
                3. Post-condition: Send GET command to verify the OFF status.

                Expected Result:
                - The response should indicate that Phantom voltage of FM is powered OFF.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Phantom FM OFF test feature Validation")
    SetCmd = "6D643E 0A 0A 01 00 01 00"
    GetCmd = "6D643E 0A 0A 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn OFF the FM ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="00",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get FM status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="00", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0A0A
@pytest.mark.order(181)
def test2_0x0A0A_Phantom_FM_On_FeatureTest(ManDiag_component):
    """ IOC Power mode : Phantom_FM ON test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Phantom_FM ON
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to turn Phantom_FM ON.          
                3. Post-condition: Send GET command to verify the OFF status.

                Expected Result:
                - The response should indicate that Phantom voltage of FM is powered ON.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Phantom_FM ON test feature Validation")
    SetCmd = "6D643E 0A 0A 01 00 01 01"
    GetCmd = "6D643E 0A 0A 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn ON the Phantom_FM ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get Phantom_FM status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="01", retries=2)

    app_logger.info("Test case passed successfully!")

'''
@pytest.mark.parametrize("state,expected", [("00", "OFF"), ("01", "ON")])
def test_0x0A0A_Phantom_FM_FeatureTest(ManDiag_component, state, expected):
    """ IOC Power mode : Phantom_FM ON/OFF test """
    
    app_logger.info(f"Starting Phantom_FM {expected} test")
    SetCmd = f"6D643E 0A 0A 01 00 01 {state}"
    GetCmd = "6D643E 0A 0A 00 00 00"
    
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data=state)
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data=state)

'''
