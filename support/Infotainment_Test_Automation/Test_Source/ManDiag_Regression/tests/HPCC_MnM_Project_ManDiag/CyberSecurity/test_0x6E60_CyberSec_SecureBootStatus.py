# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x6E60_CyberSec_SecureBootStatus.py
# Author:       Srinivasa V
# Created:      19 Nov 2024
# Description:  This File contains Test case scenarios for validating 6E 60 Cyber Security SecureBoot status.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2024 APTIV
#
# This file is part of the MnM HPCC project - Manufacture Diagnostics Component.
#
# Manufacture Diagnostics Project is free software: you can redistribute it
# and/or modify it under the terms of Diagnostics Department ASUX.
# ------------------------------------------------------------------------------------------------------------------


import time
import pytest
from infotest.core.trace_logger import app_logger


# ************************************************************* TEST CASES #01 *************************************************
@pytest.mark.order(12)
def test_0x6E60_CyberSecurity_GET_validation_for_SecureBootStatus_of_5G(ManDiag_component):
    """ CyberSecurity : read Secure boot status for 5G"""
    test_description = """
        Test Case: Validate 5G Secure Boot status
        Steps:
        1. Pre-condition: 
        2. Read Secure Boot status and validate with expectation (Get command).
        3. Validate post-condition:

        Expected Result:
        - De-Provisioning process completes successfully.
        """
    app_logger.debug(test_description)
    app_logger.info("Starting test: 5G Secure Boot status Validation")
    GetCommand = "6D643E 6E 60 00 00 01 00"

    # Step 1: Pre Condition

    # Step 2: Actual Test
    app_logger.info("Executing actual test: GET Read Secure Boot status for 5G and validate with expectation")
    assert ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="02", expected_data="00 01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
# @pytest.mark.skip(reason="skipping this because depend on fusing status of SW ")
@pytest.mark.order(13)
def test_0x6E60_CyberSecurity_GET_validation_for_SecureBootStatus_of_SOC(ManDiag_component):
    """ CyberSecurity : read Secure boot status for SOC"""
    test_description = """
        Test Case: Validate SOC Secure Boot status
        Steps:
        1. Pre-condition: 
        2. Read Secure Boot status and validate with expectation (Get command).
        3. Validate post-condition:

        Expected Result:
        - De-Provisioning process completes successfully.
        """
    app_logger.debug(test_description)
    app_logger.info("Starting test: SOC Secure Boot status Validation")
    GetCommand = "6D643E 6E 60 00 00 01 01"

    # Step 1: Pre Condition

    # Step 2: Actual Test
    app_logger.info("Executing actual test: GET Read Secure Boot status for SOC and validate with expectation")
    assert ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="02", expected_data="01 01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #03 *************************************************
@pytest.mark.order(14)
def test_0x6E60_CyberSecurity_GET_validation_for_SecureBootStatus_of_Cv2X(ManDiag_component):
    """ CyberSecurity : read Secure boot status for Cv2X"""
    test_description = """
        Test Case: Validate Cv2X Secure Boot status
        Steps:
        1. Pre-condition: 
        2. Read Secure Boot status and validate with expectation (Get command).
        3. Validate post-condition:

        Expected Result:
        - De-Provisioning process completes successfully.
        """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Cv2X Secure Boot status Validation")
    GetCommand = "6D643E 6E 60 00 00 01 02"

    # Step 1: Pre Condition

    # Step 2: Actual Test
    app_logger.info("Executing actual test: GET Read Secure Boot status for Cv2X and validate with expectation for Fused HW")
    assert ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="02", expected_data="02 01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")
