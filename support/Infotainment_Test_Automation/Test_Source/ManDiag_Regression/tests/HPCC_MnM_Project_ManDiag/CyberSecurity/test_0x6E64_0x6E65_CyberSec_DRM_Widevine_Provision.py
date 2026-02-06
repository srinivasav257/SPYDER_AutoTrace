# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x6E64_0x6E65_CyberSec_DRM_Widevine_Provision.py
# Author:       Srinivasa V
# Created:      19 Nov 2024
# Description:  This File contains Test case scenarios for validating 6E64 and 6E65 Cyber Security Widevine Provision.
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
@pytest.mark.skip(reason="disabling temperately for regression to avoid other feature failure")
@pytest.mark.order(18)
def test_0x6E64_0x6E65_CyberSecurity_SET_GET_validation_for_DRMWidevine_CertificateProvision(ManDiag_component):
    """ CyberSecurity : DRM widevine certificate provision"""
    test_description = """
        Test Case: Validate DRM Widevine Certificate Provisioning
        Steps:
        1. Pre-condition: Check initial Provision status (Get command).
        2. Execute Enable Provisioning (Set command).
            -> Write 128byte of data using multi-frame command 
            -> After successful write, push the data to SOC
        3. Validate post-condition: Verify Provision status updated (0x01, 0x02).

        Expected Result:
        - Provisioning process completes successfully.
        """
    app_logger.info(test_description)
    app_logger.info("Starting test: DRM Widevine Certificate Provisioning Validation")
    GetCommand = "6D643E 6E 65 00 00 00"
    SetCommand = "6D643E 6E 21 01 00 01 01"

    # Step 1: Pre Condition
    app_logger.info("Checking pre-condition: GET Provision status for DRM Widevine")
    pre_condition_result = ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="04", expected_data="00 00 00 01", retries=2)
    if pre_condition_result:
        app_logger.warning("Skipping this test - DRM widevine certificate provision already completed")
        pytest.skip("DRM widevine certificate provision already completed")

    # Step 2: Actual Test
    app_logger.info("Executing actual test: SET Provisioning for DRM Widevine")

    first_53bytesCommand = "6D643E 6E 64 01 00 39 00 01 00 99 61 70 74 69 76 5F 68 70 63 63 5F 38 32 39 35 5F 36 30 31 00 00 00 00 00 00 00 00 00 00 00 00 00 " \
                           "EA 41 C9 8C CF C8 10 6B D3 E8 CE 50 74 39 A2 61 00 00 00 02 00"
    next_53bytesCommand = "6D643E 6E 64 01 00 39 00 02 00 00 00 88 F7 28 8C 05 A3 CC 86 4D 9E 0F 38 F5 88 75 DA 7D BB FA 8D 7A 9C E5 DB 7D 73 35 A4 EF 6B 62 " \
                          "15 E4 19 C9 EC E2 E5 05 A4 6A EA 44 2D ED B4 C0 7D 87 E5 41 B4"
    last_23bytesCommand = "6D643E 6E 64 01 00 1A 00 FF 00 00 A7 CE E7 85 CC 78 7E EB CA 41 93 51 2C 99 6B 62 6F 78 75 91 53 DB"

    app_logger.info("SET Push first 53bytes of data ")
    assert ManDiag_component.send_request_command_and_validate_response(first_53bytesCommand, expected_data_length="05", expected_data="00 00 00 00 01")

    app_logger.info("SET Push Next 53bytes of data ")
    assert ManDiag_component.send_request_command_and_validate_response(next_53bytesCommand, expected_data_length="05", expected_data="00 00 00 00 01")

    app_logger.info("SET Push last 22bytes of data ")
    assert ManDiag_component.send_request_command_and_validate_response(last_23bytesCommand, expected_data_length="05", expected_data="00 00 00 00 01")

    app_logger.info("SET Write 128bytes of data to SOC ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCommand, expected_data_length="02", expected_data="01 01")

    # Restart the unit and Enter into mandiag after full ON with 45 sec gap
    assert ManDiag_component.send_request_command_and_validate_response("6D643E 0A 04 01 00 01 01"), f"Failed to restart the unit"
    for i in range(1, 60):
        time.sleep(1)
        app_logger.info(f"Waiting 60 Seconds for VIP Full ON - {i} Secs")
    assert ManDiag_component.send_request_command_and_validate_response("6D643E 00 01 01 00 01 01"), f"Failed to enter ManDiag Session for UART lock Unlock persistence test"

    # Step 3: Post Condition
    app_logger.info("Executing post condition test: Send SET Enter ManDiag cmd and Validate the Response")
    assert ManDiag_component.send_request_command_and_validate_response("6D643E 00 01 01 00 01 01"), f"Failed to enter ManDiag Session for DRM provision'"

    app_logger.info("Validating post-condition: GET Provision status for DRM Widevine")
    ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="04", expected_data="00 00 00 01", retries=2)

    app_logger.info("Test case passed successfully!")
