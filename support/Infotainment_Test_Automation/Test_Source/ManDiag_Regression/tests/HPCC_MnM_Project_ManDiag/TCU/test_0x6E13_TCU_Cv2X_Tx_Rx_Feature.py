# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_Telematics_Cv2X_Tx_Rx_Feature.py
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
@pytest.mark.order(4)
def test_0x6E19_0x6E20_Telematics_Cv2X_TxRx_featureTest(ManDiag_component):
    """ TCU : Cv2X Rx Tx test """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Cv2X communication signal strength.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.                
                Step 2. Actual Test
                - Send Set Cv2X Tx and Validate the Response Valid status Byte, Data_length and Data.
                - Send Set Cv2X Rx and Validate the Response Valid status Byte, Data_length and Data.                
                Step 3: Post-condition
                - - Send Get Cv2X Rx and Validate the Response Valid status Byte, Data_length and Data.

            Expected Result:
                - We should be able to verify Cv2X communication with good signal Gain.
            """
    app_logger.debug(test_description)

    SetCv2XTxCommand = "6D643E 6E 20 01 00 03 FF 00 0A"
    SetCv2XRxCommand = "6D643E 6E 19 01 00 01 0A"
    GetCv2XRxCommand = "6D643E 6E 19 00 00 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Send Set Cv2X Tx and Validate")
    assert ManDiag_component.send_request_command_and_validate_response(SetCv2XTxCommand, expected_data_length="01", expected_data="01", retries=2)

    app_logger.info("Executing actual test: Set Cv2X Rx and Validate")
    assert ManDiag_component.send_request_command_and_validate_response(SetCv2XRxCommand, expected_data_length="01", expected_data="01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing actual test: Get Cv2X Rx and Validate")
    ManDiag_response = ManDiag_component.send_request_command_and_get_response(GetCv2XRxCommand, retries=2)
    if ManDiag_response and ManDiag_response['Status'] == "01" and ManDiag_response['Databytes'] == "00 00 00 00":
        assert False, f"Actual test failed for GetCv2XRxCommand : received signal gain is {ManDiag_response['Databytes']} not acceptable"

    app_logger.info("Test case passed successfully!")
