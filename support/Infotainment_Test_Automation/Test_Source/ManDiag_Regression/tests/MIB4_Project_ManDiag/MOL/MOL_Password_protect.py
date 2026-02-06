# ------------------------------------------------------------------------------------------------------------------
# File Name:    MOL_Password_protect.py
# Author:       Srinivasa V
# Created:      Jan 02 2025
# Description:  This File contains Test case scenarios for validating MOL password protection.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2024 APTIV
#
# This file is part of the MnM MIB4 project - Manufacture Diagnostics Component.
#
# Manufacture Diagnostics Project is free software: you can redistribute it
# and/or modify it under the terms of Diagnostics Department ASUX.
# ------------------------------------------------------------------------------------------------------------------

import time
from infotest.core.trace_logger import app_logger
import pytest


# ************************************************************* TEST CASES #01 *************************************************
@pytest.mark.mib4_MOL_regression_test
@pytest.mark.order(801)
def test1_MOL_CAN_unlock_password_protection(vector_CANCH2):
    """ MOL CAN : unlock CAN password protection for MOL"""
    # Step 0: Update the Test variable and Test Procedure

    test_description = """
                Test Case: CAN MOL password unlock feature Validation
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET unlock CAN MOL password protection.          
                3. Post-condition: Read and verify response.

                Expected Result:
                - 
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: CAN MOL password unlock feature Validation")

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Send unlock CAN MOL password protection and verify the response")
    vector_CANCH2.send_and_validate_response(request_data =[0x29, 0x32, 0x36, 0x31, 0x38, 0x1F, 0x08, 0x00],
                                             expected_data=[0x29, 0x32, 0x36, 0x31, 0x38, 0x1F, 0x08, 0x00],
                                             extended_id=True)
    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")

# ************************************************************* TEST CASES #01 *************************************************
@pytest.mark.mib4_MOL_regression_test
@pytest.mark.order(801)
def test2_MOL_CAN_Enter_ManDiag_Session(vector_CANCH2):
    """ MOL CAN : Enter mandiag Session for MOL"""
    # Step 0: Update the Test variable and Test Procedure

    test_description = """
                Test Case: 
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET enter mandiag session.          
                3. Post-condition: Read and verify response.

                Expected Result:
                - 
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: CAN MOL Enter ManDiag feature Validation")

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Send Enter ManDiag and verify the response")
    vector_CANCH2.send_and_validate_response(request_data =[0xFF, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00],
                                             expected_data=[0xFF, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00],
                                             extended_id=True)
    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")