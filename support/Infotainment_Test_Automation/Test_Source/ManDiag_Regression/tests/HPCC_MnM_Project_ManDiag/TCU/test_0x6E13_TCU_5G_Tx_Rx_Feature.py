# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_Telematics_5G_Tx_Rx_Feature.py
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
@pytest.mark.order(3)
def test_0x6E17_0x6E18_Telematics_5G_TxRx_featureTest(ManDiag_component):
    """ TCU : 5G Rx Tx test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of 5G communication signal strength.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.
                - Enter into 5G FTM mode
                Step 2. Actual Test
                - Send Set 5G Tx and Validate the Response Valid status Byte, Data_length and Data.
                - Send Set 5G Rx and Validate the Response Valid status Byte, Data_length and Data.
                - Send Get 5G Rx and Validate the Response Valid status Byte, Data_length and Data.
                Step 3: Post-condition
                - Exit the 5G FTM mode

            Expected Result:
                - We should be able to verify 5G communication with good signal Gain.
            """
    app_logger.debug(test_description)

    SetEnterFTM5G = "6D643E 6E 62 01 00 02 01 02"
    Set5GTxCommand = "6D643E 6E 17 01 00 0F 02 00 4E 03 00 09 D1 05 00 01 41 0F A0 0B B8"
    Set5GRxCommand = "6D643E 6E 18 01 00 0C 02 00 4E 03 00 09 D1 05 00 FE 0C 00"
    Get5GRxCommand = "6D643E 6E 18 00 00 00"
    SetExitFTM5G = "6D643E 6E 62 01 00 02 00 02"

    # Step 1: Pre_Condition Test
    app_logger.info("Executing Post Condition test: Enter FTM mode for 4G")
    assert ManDiag_component.send_request_command_and_validate_response(SetEnterFTM5G, expected_data_length="03", expected_data="01 02 01", retries=2)

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Send Set 5G Tx and Validate")
    assert ManDiag_component.send_request_command_and_validate_response(Set5GTxCommand, expected_data_length="01", expected_data="01", retries=2)

    app_logger.info("Executing actual test: Set 5G Rx and Validate")
    assert ManDiag_component.send_request_command_and_validate_response(Set5GRxCommand, expected_data_length="01", expected_data="01", retries=2)

    app_logger.info("Executing actual test: Get 5G Rx and Validate")
    ManDiag_response = ManDiag_component.send_request_command_and_get_response(Get5GRxCommand, retries=2)
    if ManDiag_response and ManDiag_response['Status'] == "01" and ManDiag_response['Databytes'] == "00 00 00 00" and ManDiag_response['DataLength'] == "04":
        assert False, f"Actual test failed for Get5GRxCommand : received signal gain is {ManDiag_response['Databytes']} not acceptable"

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Exit FTM mode for 5G")
    assert ManDiag_component.send_request_command_and_validate_response(SetExitFTM5G, expected_data_length="03", expected_data="00 02 01", retries=2)

    app_logger.info("Test case passed successfully!")
