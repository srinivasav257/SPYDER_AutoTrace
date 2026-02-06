# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x1408_Audio_Primary_Source.py
# Author:       Srinivasa V
# Created:      Jan 02 2025
# Description:  This File contains Test case scenarios for validating 0x0A0A FM ON/OFF feature.
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
@pytest.mark.mib4_System_0x1408
@pytest.mark.order(211)
def test1_0x1408_Audio_Primary_Source_FeatureTest(ManDiag_component):
    """ IOC Power mode : Audio Primary Sorce switch test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Audio Primary Source switch
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET udio Primary Source command          
                3. Post-condition: Send GET command to verify the OFF status.

                Expected Result:
                - The response should indicate that Phantom voltage of FM is powered OFF.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test:  Audio Primary Source test feature Validation")
    SetCmd = "6D643E 14 08 01 00 01 02"
    GetCmd = "6D643E 14 08 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test:  ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="02",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get FM status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="02", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x1408
@pytest.mark.order(212)
def test2_0x1408_Audio_Primary_Source_FeatureTest(ManDiag_component):
    """ IOC Power mode : Audio Primary Source test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
                Test Case: Validate Audio Primary Source switch
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET power Enable/Disable command         
                3. Post-condition: Send GET command to verify the OFF status.

                Expected Result:
                - The response should indicate Primary Source switch
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Audio Primary Source test feature Validation")
    SetCmd = "6D643E 14 08 01 00 01 01"
    GetCmd = "6D643E 14 08 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01",  retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: get status ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_data_length="01", expected_data="01", retries=2)

    app_logger.info("Test case passed successfully!")
