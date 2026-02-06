# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x1F01_Tuner_Band_Frequency.py
# Author:       Srinivasa V
# Created:      Jan 02 2025
# Description:  This File contains Test case scenarios for validating Tuner feature.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2024 APTIV
#
# This file is part of the MIB4 project - Manufacture Diagnostics Component.
#
# Manufacture Diagnostics Project is free software: you can redistribute it
# and/or modify it under the terms of Diagnostics Department ASUX.
# ------------------------------------------------------------------------------------------------------------------

import time

from diagnostics.logging_setup import logger
import pytest


# ************************************************************* TEST CASES #01 *************************************************
# @pytest.mark.skip(reason="disabling just for debugging purpose")
@pytest.mark.order(1)
def test_0x1F01_Tuner_SET_GET_TunerBandFrequency_AM(diag_test):
    """ QNX_Tuner : Set Tuner band frequency for AM """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of setting and Reading Tuner frequency for AM.

           Steps:
               Pre-condition
                - Do source switch to AM.
                - Read default AM frequency and validate - 522Khz
               Actual Test
                - Send SET Tuner Frequency to 1000Khz and Validate the Response Valid status Byte, Data_length and Data.
               Post-condition
                - Send GET Read AM frequency and validate with expected data.

            Expected Result:
                - The user should be able SET frequency and read the present frequency.                
            """
    logger.debug(test_description)
    time.sleep(1)
    SetAMSource = "6D643E 14 08 01 00 01 02"
    GetCommand = "6D643E 1F 01 00 00 01 01"
    SetCommand = " 6D643E 1F 01 01 00 04 01 01 03 E8"

    # Step 1: Pre_Condition Test

    logger.debug("Executing Pre_Condition test: Switch source to AM")
    actual_test_result = diag_test.send_and_validate_command(SetAMSource, expected_data_length="00", expected_data=None)
    assert actual_test_result, f"Actual test failed for command '{SetAMSource}' with expected data_length: 00 but observed: {diag_test.data_length}"

    time.sleep(2)
    logger.debug("Executing Pre_Condition test: Read and verify Tuner AM default frequency - 522Khz")
    actual_test_result = diag_test.send_and_validate_command(GetCommand, expected_data_length="04", expected_data=[0x01, 0x01, 0x02, 0x0A])
    assert actual_test_result, f"Actual test failed for command '{GetCommand}' with expected data_length: 04 but observed: {diag_test.data_length}"

    # Step 2: Actual Test Case
    logger.debug("Executing actual test: Set AM frequency to 1000Khz")
    actual_test_result = diag_test.send_and_validate_command(SetCommand, expected_data_length="00", expected_data=None)
    assert actual_test_result, f"Actual test failed for command '{SetCommand}' with expected data_length: 00 but observed: {diag_test.data_length}"

    # Step 3: Post Condition
    time.sleep(1)
    logger.debug("Executing Pre_Condition test: Read and verify Tuner AM  newly SET frequency")
    actual_test_result = diag_test.send_and_validate_command(GetCommand, expected_data_length="04", expected_data=[0x01, 0x01, 0x03, 0xE8])
    assert actual_test_result, f"Actual test failed for command '{GetCommand}' with expected data_length: 04 but observed: {diag_test.data_length}"

    logger.debug("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
@pytest.mark.order(2)
# @pytest.mark.skip(reason="disabling just for debugging purpose")
def test_0x1F01_Tuner_SET_GET_TunerBandFrequency_FM(diag_test):
    """ QNX_Tuner : Set Tuner band frequency for FM """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of setting and Reading Tuner frequency for FM.

            Steps:
               Pre-condition
                - Do source switch to FM.
                - Read default FM frequency and validate - 87.50Kh
               Actual Test
                - Send SET write Tuner Frequency to 98.10Khz and Validate the Response Valid status Byte, Data_length and Data.
               Post-condition
                - Send GET Read FM frequency and validate with expected data.

            Expected Result:
                - The user should be able SET frequency and read the present frequency.                
            """
    logger.debug(test_description)
    time.sleep(1)
    SetFMSource = "6D643E 14 08 01 00 01 01"
    GetCommand = "6D643E 1F 01 00 00 01 01"
    SetCommand = "6D643E 1F 01 01 00 04 01 00 26 52"

    # Step 1: Pre_Condition Test

    logger.debug("Executing Pre_Condition test: Switch source to FM")
    actual_test_result = diag_test.send_and_validate_command(SetFMSource, expected_data_length="00", expected_data=None)
    assert actual_test_result, f"Actual test failed for command '{SetFMSource}' with expected data_length: 00 but observed: {diag_test.data_length}"

    time.sleep(2)
    logger.debug("Executing Pre_Condition test: Read and verify Tuner AM default frequency 87.50Kh")
    actual_test_result = diag_test.send_and_validate_command(GetCommand, expected_data_length="04", expected_data=[0x01, 0x00, 0x22, 0x2E])
    assert actual_test_result, f"Actual test failed for command '{GetCommand}' with expected data_length: 04 but observed: {diag_test.data_length}"

    # Step 2: Actual Test Case
    logger.debug("Executing actual test: Set AM frequency to  98.10Khz")
    actual_test_result = diag_test.send_and_validate_command(SetCommand, expected_data_length="00", expected_data=None)
    assert actual_test_result, f"Actual test failed for command '{SetCommand}' with expected data_length: 00 but observed: {diag_test.data_length}"

    # Step 3: Post Condition
    logger.debug("Executing Pre_Condition test: Read and verify Tuner AM  newly SET frequency")
    time.sleep(1)
    actual_test_result = diag_test.send_and_validate_command(GetCommand, expected_data_length="04", expected_data=[0x01, 0x00, 0x26, 0x52])
    assert actual_test_result, f"Actual test failed for command '{GetCommand}' with expected data_length: 04 but observed: {diag_test.data_length}"

    logger.debug("Test case passed successfully!")
