# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x0A14_Phantom_USB_ON_OFF.py
# Author:       Srinivasa V
# Created:      Jan 02 2025
# Description:  This File contains Test case scenarios for validating 0x0A14 USB ON/OFF feature.
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
@pytest.mark.mib4_System_0x0A14
@pytest.mark.order(193)
def test1_0x0A14_Phantom_USB_Off_FeatureTest(ManDiag_component):
    """ IOC Power mode : Phantom_USB OFF test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Phantom_USB OFF
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to turn USB OFF.          
                3. Post-condition: Send GET command to verify the OFF status.

                Expected Result:
                - The response should indicate that Phantom voltage of USB is powered OFF.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Phantom USB OFF test feature Validation")
    SetCmd = "6D643E 0A 14 01 00 02 00 00"
    GetCmd = "6D643E 0A 14 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn OFF the USB ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get USB status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data="00 00", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0A14
@pytest.mark.order(194)
def test2_0x0A14_Phantom_USB_On_FeatureTest(ManDiag_component):
    """ IOC Power mode : Phantom_USB ON test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Phantom_USB ON
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command to turn Phantom_USB ON.          
                3. Post-condition: Send GET command to verify the OFF status.

                Expected Result:
                - The response should indicate that Phantom voltage of USB is powered ON.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Phantom_USB ON test feature Validation")
    SetCmd = "6D643E 0A 14 01 00 02 00 01"
    GetCmd = "6D643E 0A 14 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Turn ON the Phantom_USB ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get Phantom_USB status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="02", expected_data="00 01", retries=2)

    app_logger.info("Test case passed successfully!")
