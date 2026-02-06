# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x6E13_TCU_4G_Tx_Rx_Feature.py
# Author:       Srinivasa V
# Created:      Sept 02 2024
# Description:  This File contains Test case scenarios for validating Telematics Data Supplier feed.
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
@pytest.mark.hpcc_tcu
@pytest.mark.order(2)
def test_0x6E15_0x6E16_Telematics_4G_TxRx_featureTest(ManDiag_component):
    """ TCU : 4G Rx Tx test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of 4G communication signal strength.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.
                - Enter into 4G FTM mode
                Step 2. Actual Test
                - Send Set 4G Tx and Validate the Response Valid status Byte, Data_length and Data.
                - Send Set 4G Rx and Validate the Response Valid status Byte, Data_length and Data.
                - Send Get 4G Rx and Validate the Response Valid status Byte, Data_length and Data.
                Step 3: Post-condition
                - Exit the 4G FTM mode

            Expected Result:
                - We should be able to verify 4G communication with good signal Gain.
            """
    app_logger.debug(test_description)

    SetEnterFTM4G = "6D643E 6E 62 01 00 02 01 01"
    Set4GTxCommand = "6D643E 6E 15 01 00 0B 01 00 03 03 00 00 4C 77 01 05 00"
    Set4GRxCommand = "6D643E 6E 16 01 00 0B 01 00 28 03 00 00 96 FA 00 FE 0C"
    Get4GRxCommand = "6D643E 6E 16 00 00 00"
    SetExitFTM4G = "6D643E 6E 62 01 00 02 00 01"

    # Step 1: Pre_Condition Test
    app_logger.info("Executing Post Condition test: Enter FTM mode for 4G")
    assert ManDiag_component.send_request_command_and_validate_response(SetEnterFTM4G, expected_data_length="03", expected_data="01 01 01", retries=2)

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Read and verify Set4GTxCommand")
    assert ManDiag_component.send_request_command_and_validate_response(Set4GTxCommand, expected_data_length="01", expected_data="01", retries=2)

    app_logger.info("Executing actual test: Read and verify Set4GRxCommand")
    assert ManDiag_component.send_request_command_and_validate_response(Set4GRxCommand, expected_data_length="01", expected_data="01", retries=2)

    app_logger.info("Executing actual test: Read and verify Get4GRxCommand")
    ManDiag_response = ManDiag_component.send_request_command_and_get_response(Get4GRxCommand, retries=2)
    if ManDiag_response and ManDiag_response['Status'] == "01" and ManDiag_response['DataLength'] == "08":
        if ManDiag_response['Databytes'] == "00 00 00 00 00 00 00 00":
            assert False, f"Actual test failed for Get4GRxCommand : received signal gain is {ManDiag_response['Databytes']} not acceptable"
    else:
        assert False, f"Actual test failed for Get4GRxCommand: Invalid Response"

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Exit FTM mode for 4G")
    assert ManDiag_component.send_request_command_and_validate_response(SetExitFTM4G, expected_data_length="03", expected_data="00 01 01", retries=2)

    app_logger.info("Test case passed successfully!")
