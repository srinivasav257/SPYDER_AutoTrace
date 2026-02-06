# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x1F07_Tuner_Signal_Strength.py
# Author:       Srinivasa V
# Created:      Jan 02 2025
# Description:  This File contains Test case scenarios for validating Tuner feature.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2024 APTIV
#
# This file is part of the MnM HPCC project - Manufacture Diagnostics Component.
#
# Manufacture Diagnostics Project is free software: you can redistribute it
# and/or modify it under the terms of Diagnostics Department ASUX.
# ------------------------------------------------------------------------------------------------------------------

import time

from diagnostics.logging_setup import logger
import pytest


# ************************************************************* TEST CASES #01 *************************************************
# @pytest.mark.skip(reason="disabling just for debugging purpose")
@pytest.mark.order(4)
def test_0x1F07_Tuner_Signal_Strength_AM(diag_test):
    """ QNX_Tuner : Get Tuner Signal strength AM """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Getting Tuner Signal strength.

            Test Sequence Steps:
                Step 1: Pre-condition
                - 
                Step 2. Actual Test
                - 
                Step 3: Post-condition
                - 

            Expected Result:
                - The user should be able to read Tuner Signal strength.                
            """
    logger.debug(test_description)
    time.sleep(1)
    SetAMSource = "6D643E 14 07 01 00 01 01"
    GetAMSignalStrength = "6D643E 1F 07 00 00 01 01"

    # Step 1: Pre_Condition Test
    logger.debug("Executing Pre_Condition test: Switch source to AM")
    actual_test_result = diag_test.send_and_validate_command(SetAMSource, expected_data_length="00", expected_data=None)
    assert actual_test_result, f"Actual test failed for command '{SetAMSource}' with expected data_length: 00 but observed: {diag_test.data_length}"

    # Step 2: Actual Test Case
    logger.debug("Executing actual test: read AM tuner strength ")
    actual_test_result = diag_test.send_and_validate_command(GetAMSignalStrength, expected_data_length="02")
    if actual_test_result and diag_test.data[3:] == "00":
        assert False, f"Actual test failed for command '{GetAMSignalStrength}', Received Signal strength {diag_test.diag_test.data[3:]} is not valid"
    else:
        assert actual_test_result, f"Actual test failed for command '{GetAMSignalStrength}' with expected data_length: 04 but observed: {diag_test.data_length}"

    # Step 3: Post Condition
    logger.debug("Executing Pre_Condition test: Read and verify")

    logger.debug("Test case passed successfully!")

# ************************************************************* TEST CASES #02 *************************************************
@pytest.mark.skip(reason="disabling dur to HW dependency")
@pytest.mark.order(5)
def test_0x1F07_Tuner_Signal_Strength_FM(diag_test):
    """ QNX_Tuner : Get Tuner Signal strength FM """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Getting Tuner Signal strength.

            Test Sequence Steps:
                Step 1: Pre-condition
                - 
                Step 2. Actual Test
                - 
                Step 3: Post-condition
                - 

            Expected Result:
                - The user should be able to read Tuner Signal strength.                
            """
    logger.debug(test_description)
    time.sleep(1)
    SetFMSource = "6D643E 14 07 01 00 01 02"
    GetFMSignalStrength = "6D643E 1F 07 00 00 01 01"

    # Step 1: Pre_Condition Test
    logger.debug("Executing Pre_Condition test: Switch source to FM")
    actual_test_result = diag_test.send_and_validate_command(SetFMSource, expected_data_length="00", expected_data=None)
    assert actual_test_result, f"Actual test failed for command '{SetFMSource}' with expected data_length: 00 but observed: {diag_test.data_length}"

    # Step 2: Actual Test Case
    logger.debug("Executing actual test: read AM tuner strength ")
    actual_test_result = diag_test.send_and_validate_command(GetFMSignalStrength, expected_data_length="02")
    if actual_test_result and diag_test.data[3:] == "00":
        assert False, f"Actual test failed for command '{GetFMSignalStrength}', Received Signal strength {diag_test.data[3:]} is not valid"
    else:
        assert actual_test_result, f"Actual test failed for command '{GetFMSignalStrength}' with expected data_length: 04 but observed: {diag_test.data_length}"

    # Step 3: Post Condition
    logger.debug("Executing Pre_Condition test: Read and verify")

    logger.debug("Test case passed successfully!")