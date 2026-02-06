# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x6E62_TCU_FTM_Mode_4G_5G.py
# Author:       Srinivasa V
# Created:      Sept 02 2024
# Description:  This File contains Test case scenarios for validating functionality of entering and exiting FTM mode
#               for 4G and 5G..
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
@pytest.mark.order(17)
def test_0x6E62_TCU_FTM_Mode_4G_featureTest(ManDiag_component):
    """ TCU : 6E 62 FTM_Mode_4G """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of entering and exiting FTM mode for 4G.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.
                Step 2. Actual Test
                - Send Set Enter into 4G FTM mode and Validate the Response Valid status Byte, Data_length and Data.
                Step 3: Post-condition
                 - Send Set Exit the 4G FTM mode and Validate the Response Valid status Byte, Data_length and Data.

            Expected Result:
                - We should be able to verify enter and exit operation of FTM mode for 4G communication.
            """
    app_logger.debug(test_description)

    SetEnterFTM4G = "6D643E 6E 62 01 00 02 01 01"
    SetExitFTM4G = "6D643E 6E 62 01 00 02 00 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing Post Condition test: Enter FTM mode for 4G")
    assert ManDiag_component.send_request_command_and_validate_response(SetEnterFTM4G, expected_data_length="03", expected_data="01 01 01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Exit FTM mode for 4G")
    assert ManDiag_component.send_request_command_and_validate_response(SetExitFTM4G, expected_data_length="03", expected_data="00 01 01", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #03 *************************************************
@pytest.mark.hpcc_tcu
@pytest.mark.order(18)
def test_0x6E62_TCU_FTM_Mode_5G_featureTest(ManDiag_component):
    """ TCU : 6E 62 FTM_Mode_5G """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of entering and exiting FTM mode for 5G.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.
                Step 2. Actual Test
                - Send Set Enter into 5G FTM mode and Validate the Response Valid status Byte, Data_length and Data.
                Step 3: Post-condition
                 - Send Set Exit the 5G FTM mode and Validate the Response Valid status Byte, Data_length and Data.

            Expected Result:
                - We should be able to verify enter and exit operation of FTM mode for 4G communication.
            """
    app_logger.debug(test_description)
    time.sleep(1)
    SetEnterFTM5G = "6D643E 6E 62 01 00 02 01 02"
    SetExitFTM5G = "6D643E 6E 62 01 00 02 00 02"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing Post Condition test: Enter FTM mode for 5G")
    assert ManDiag_component.send_request_command_and_validate_response(SetEnterFTM5G, expected_data_length="03", expected_data="01 02 01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Exit FTM mode for 5G")
    assert ManDiag_component.send_request_command_and_validate_response(SetExitFTM5G, expected_data_length="03", expected_data="00 02 01", retries=2)

    app_logger.info("Test case passed successfully!")
