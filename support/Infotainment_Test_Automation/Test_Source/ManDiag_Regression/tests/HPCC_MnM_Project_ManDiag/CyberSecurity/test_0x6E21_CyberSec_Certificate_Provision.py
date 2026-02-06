# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x6E21_CyberSec_Certificate_Provision.py
# Author:       Srinivasa V
# Created:      19 Nov 2024
# Description:  This File contains Test case scenarios for validating 6E 21 Cyber Security Certificate Provisioning .
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

@pytest.mark.order(6)
def test_0x6E21_CyberSecurity_SET_GET_validation_for_QNX_CertificateProvision(ManDiag_component):
    """ CyberSecurity : QNX Certificate Provision """
    test_description = """
        Test Case: Validate QNX Certificate Provisioning
        Steps:
        1. Pre-condition: Check initial Provision status (Get command).
        2. Execute Enable Provisioning (Set command).
        3. Validate post-condition: Verify Provision status updated (0x01, 0x02).

        Expected Result:
        - Provisioning process completes successfully.
        """
    app_logger.debug(test_description)
    app_logger.info("Starting test: QNX Certificate Provisioning Validation")
    GetCommand = "6D643E 6E 21 00 00 01 01"
    SetCommand = "6D643E 6E 21 01 00 01 01"

    # Step 1: Pre Condition
    app_logger.info("Checking pre-condition: GET Provision status for QNX")
    pre_condition_result = ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="02", expected_data="01 02", retries=2)
    if pre_condition_result:
        app_logger.warning("Skipping this test - QNX is already Provisioned")
        pytest.skip("QNX is already Provisioned")

    # Step 2: Actual Test
    app_logger.info("Executing actual test: SET Provisioning for QNX")
    assert ManDiag_component.send_request_command_and_validate_response(SetCommand, expected_data_length="02", expected_data="01 01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Validating post-condition: GET Provision status for QNX")
    assert ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="02", expected_data="01 02", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
@pytest.mark.order(7)
def test_0x6E21_CyberSecurity_SET_GET_validation_for_ANDROID_CertificateProvision(ManDiag_component):
    """ CyberSecurity : ANDROID Certificate Provision """
    test_description = """
        Test Case: Validate ANDROID Certificate Provisioning
        Steps:
        1. Pre-condition: Check initial Provision status (Get command).
        2. Execute Enable Provisioning (Set command).
        3. Validate post-condition: Verify Provision status updated (0x02, 0x02).

        Expected Result:
        - Provisioning process completes successfully.
        """
    app_logger.info(test_description)
    app_logger.info("Starting test: ANDROID Certificate Provisioning Validation")
    GetCommand = "6D643E 6E 21 00 00 01 02"
    SetCommand = "6D643E 6E 21 01 00 01 02"

    # Step 1: Pre Condition
    app_logger.info("Checking pre-condition: GET Provision status for ANDROID")
    pre_condition_result = ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="02", expected_data="02 02", retries=2)
    if pre_condition_result:
        app_logger.warning("Skipping this test - ANDROID is already Provisioned")
        pytest.skip("Android is already Provisioned")

    # Step 2: Actual Test
    app_logger.info("Executing actual test: SET Provisioning for ANDROID")
    assert ManDiag_component.send_request_command_and_validate_response(SetCommand, expected_data_length="02", expected_data="02 01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Validating post-condition: GET Provision status for ANDROID")
    assert ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="02", expected_data="02 02", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #03 *************************************************
@pytest.mark.order(8)
def test_0x6E21_CyberSecurity_SET_GET_validation_for_TCU_CertificateProvision(ManDiag_component):
    """ CyberSecurity : 5G Certificate Provision """
    test_description = """
        Test Case: Validate TCU Certificate Provisioning
        Steps:
        1. Pre-condition: Check initial Provision status (Get command).
        2. Execute Enable Provisioning (Set command).
        3. Validate post-condition: Verify Provision status updated (0x03, 0x02).

        Expected Result:
        - Provisioning process completes successfully.
        """
    app_logger.info(test_description)
    app_logger.info("Starting test: TCU Certificate Provisioning Validation")
    GetCommand = "6D643E 6E 21 00 00 01 03"
    SetCommand = "6D643E 6E 21 01 00 01 03"

    # Step 1: Pre Condition
    app_logger.info("Checking pre-condition: GET Provision status for TCU")
    pre_condition_result = ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="02", expected_data="03 02", retries=2)
    if pre_condition_result:
        app_logger.warning("Skipping this test - 5G is already Provisioned")
        pytest.skip("TCU is already Provisioned")
    # assert pre_condition_result, f"Pre-condition check failed for command '{GetCommand}' with expected data {[0x03, 0x00]}"

    # Step 2: Actual Test
    app_logger.info("Executing actual test: SET Provisioning for TCU")
    assert ManDiag_component.send_request_command_and_validate_response(SetCommand, expected_data_length="02", expected_data="03 01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Validating post-condition: GET Provision status for TCU")
    assert ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="02", expected_data="03 02", retries=2)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #04 *************************************************
@pytest.mark.order(9)
def test_0x6E21_CyberSecurity_SET_GET_validation_for_Cv2X_CertificateProvision(ManDiag_component):
    """ CyberSecurity : Cv2X Certificate Provision """
    test_description = """
        Test Case: Validate Cv2X Certificate Provisioning
        Steps:
        1. Pre-condition: Check initial Provision status (Get command).
        2. Execute Enable Provisioning (Set command).
        3. Validate post-condition: Verify Provision status updated (0x04, 0x02).

        Expected Result:
        - Provisioning process completes successfully.
        """
    app_logger.info(test_description)
    app_logger.info("Starting test: Cv2X Certificate Provisioning Validation")
    GetCommand = "6D643E 6E 21 00 00 01 04"
    SetCommand = "6D643E 6E 21 01 00 01 04"

    # Step 1: Pre Condition
    app_logger.info("Checking pre-condition: GET Provision status for Cv2X")
    pre_condition_result = ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="02", expected_data="04 02", retries=2)
    if pre_condition_result:
        app_logger.warning("Skipping this test - Cv2X is already Provisioned")
        pytest.skip("Cv2X is already Provisioned")

    # Step 2: Actual Test
    app_logger.info("Executing actual test: SET Provisioning for Cv2X")
    assert ManDiag_component.send_request_command_and_validate_response(SetCommand, expected_data_length="02", expected_data="04 01", retries=2)

    # Step 3: Post Condition
    app_logger.info("Validating post-condition: GET Provision status for Cv2X")
    assert ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="02", expected_data="04 02", retries=2)

    app_logger.info("Test case passed successfully!")
