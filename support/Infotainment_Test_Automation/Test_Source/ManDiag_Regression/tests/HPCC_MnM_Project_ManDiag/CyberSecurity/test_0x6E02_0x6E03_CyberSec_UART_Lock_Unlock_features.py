# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x6E02_0x6E03_CyberSec_UART_Lock_Unlock_features.py
# Author:       Srinivasa V
# Created:      19 Nov 2024
# Description:  This File contains Test case scenarios for validating 6E02 6E03 Cyber Security UART Lock/Unlock.
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
# @pytest.mark.skip(reason="no way of currently testing this")
@pytest.mark.order(1)
def test_0x6E02_0x6E03_CyberSecurity_SET_GET_validation_for_UART_Lock_Unlock_Operation(ManDiag_component):
    """ CyberSecurity : UART lock Operation """

    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case: Validate UART lock operation
            Steps:
            1. Pre-condition: Check initial UART lock status (Get command).
            2. Lock the UART with 16bytes unique key  (Set command).            
            3. Validate post-condition: Verify UART lock status updated (Get command) .

            Expected Result:
            - Uart should be Locked successfully.
            """
    app_logger.debug(test_description)
    app_logger.info("Starting test: UART lock feature Validation")

    Set_uart_lock = "6D643E 6E 02 11 00 11 01 EE EE EE EE FF FF FF FF EE EE EE EE FF FF FF FF 30 2D"
    Set_uart_unlock = "6D643E 6E 02 11 00 11 00 EE EE EE EE FF FF FF FF EE EE EE EE FF FF FF FF 20 CF"
    Get_uart_lockStatus = "6D643E 6E 03 00 00 00"

    # Step 1: Pre_Condition Test
    app_logger.info("Checking pre-condition: GET check UART Lock Status is UNLOCK-00")
    assert ManDiag_component.send_request_command_and_validate_response(Get_uart_lockStatus, expected_data_length="01", expected_data="00")

    # Step 2: Actual Test
    app_logger.info("Executing actual test: SET LOCK the UART")
    assert ManDiag_component.send_request_command_and_validate_response(Set_uart_lock, expected_data_length="00", expected_data="C0 2F")

    app_logger.info("Executing actual test: GET Check UART Lock Status is LOCKED-01")
    assert ManDiag_component.send_request_command_and_validate_response(Get_uart_lockStatus, expected_data_length="01", expected_data="01")

    # Step 3: Post Condition Test
    app_logger.info("Validating post-condition: SET UNLOCK the UART")
    assert ManDiag_component.send_request_command_and_validate_response(Set_uart_unlock, expected_data_length="00", expected_data="C0 2F")

    app_logger.info("Validating post-condition: GET Check UART Lock Status is UNLOCKED-00")
    assert ManDiag_component.send_request_command_and_validate_response(Get_uart_lockStatus, expected_data_length="01", expected_data="00")

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
@pytest.mark.skip(reason="no way of currently testing this with automation")
@pytest.mark.order(2)
def test_0x6E02_0x6E03_CyberSecurity_SET_GET_validation_for_UART_Lock_Operation_persistance(ManDiag_component):
    """ CyberSecurity : UART lock unlock Operation persistence test"""

    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case: Validate UART lock unlock operation persistence test
            Steps:
            1. Pre-condition: Check initial UART lock status (Get command).
            2. Lock the UART with 16bytes unique key  (Set command).            
            3. Validate post-condition: Verify UART lock status updated (Get command) .

            Expected Result:
            - Uart should be Locked successfully.
            """
    app_logger.debug(test_description)
    app_logger.info("Starting test: UART lock unlock feature persistance Validation")

    Set_uart_lock = "6D643E 6E 02 11 00 11 01 EE EE EE EE FF FF FF FF EE EE EE EE FF FF FF FF 30 2D"
    Set_uart_unlock = "6D643E 6E 02 11 00 11 00 EE EE EE EE FF FF FF FF EE EE EE EE FF FF FF FF 20 CF"
    Get_uart_lockStatus = "6D643E 6E 03 00 00 00"
    set_restart = "6D643E 0A 04 01 00 01 01"

    # Step 1: Pre_Condition Test
    app_logger.info("Checking pre-condition: GET check UART Lock Status is UNLOCK-00")
    assert ManDiag_component.send_request_command_and_validate_response(Get_uart_lockStatus, expected_data_length="01", expected_data="00")

    # Step 2: Actual Test
    app_logger.info("Executing actual test: SET LOCK the UART and validate persistence for lock status")
    assert ManDiag_component.send_request_command_and_validate_response(Set_uart_lock, expected_data_length="00", expected_data="C0 2F")

    app_logger.info("Validating actual-condition: GET Check UART Lock Status is LOCKED-01")
    assert ManDiag_component.send_request_command_and_validate_response(Get_uart_lockStatus, expected_data_length="01", expected_data="01")

    app_logger.info("Validating actual-condition: Mandiag restart")
    assert ManDiag_component.send_request_command_and_validate_response(set_restart)

    # Enter into mandiag after full ON
    # 30 sec gap full restart and Enter into mandiag after full ON
    for i in range(1, 30):
        time.sleep(1)
        app_logger.info(f"Waiting 30 Seconds for VIP Full ON - {i} Secs")
    assert ManDiag_component.send_request_command_and_validate_response("6D643E 00 01 01 00 01 01"), f"Failed to enter ManDiag Session for UART lock Unlock persistence test"

    app_logger.info("Validating actual-condition: GET Check UART Lock Status is LOCKED-01")
    assert ManDiag_component.send_request_command_and_validate_response(Get_uart_lockStatus, expected_data_length="01", expected_data="01")

    # Step 3: Post Condition Test
    app_logger.info("Executing post-condition: SET UNLOCK the UART and validate persistence for lock status")
    assert ManDiag_component.send_request_command_and_validate_response(Set_uart_unlock, expected_data_length="00", expected_data="C0 2F")

    app_logger.info("Validating Post-condition: GET Check UART Lock Status is UnLOCKED-00")
    assert ManDiag_component.send_request_command_and_validate_response(Get_uart_lockStatus, expected_data_length="01", expected_data="00")

    # Enter into mandiag after full ON
    # 30 sec gap full restart and Enter into mandiag after full ON
    for i in range(1, 30):
        time.sleep(1)
        app_logger.info(f"Waiting 30 Seconds for VIP Full ON - {i} Secs")
    assert ManDiag_component.send_request_command_and_validate_response("6D643E 00 01 01 00 01 01"), f"Failed to enter ManDiag Session for UART lock Unlock persistence test"

    app_logger.info("Validating Post-condition: GET Check UART Lock Status is UnLOCKED-00")
    assert ManDiag_component.send_request_command_and_validate_response(Get_uart_lockStatus, expected_data_length="01", expected_data="00")

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #03 *************************************************
# @pytest.mark.skip(reason="no way of currently testing this")
@pytest.mark.order(3)
def test_0x6E02_0x6E03_CyberSecurity_SET_GET_validation_for_continuous_UART_lock_Unlock_Operation(ManDiag_component):
    """ CyberSecurity : UART lock Unlock Operation more than 7times in single power cycle """

    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case: Validate UART lock Unlock operation for 7 times
            Steps:
            1. Pre-condition: Check initial UART lock status (Get command).
            2. UnLock the UART with 16bytes unique key  (Set command).            
            3. Validate post-condition: Verify UART lock status updated (Get command) .

            Expected Result:
            - Uart should be UnLocked successfully.
            """
    app_logger.debug(test_description)
    app_logger.info("Starting test: UART Unlock feature Validation")

    Set_uart_lock = "6D643E 6E 02 11 00 11 01 EE EE EE EE FF FF FF FF EE EE EE EE FF FF FF FF 30 2D"
    Set_uart_unlock = "6D643E 6E 02 11 00 11 00 EE EE EE EE FF FF FF FF EE EE EE EE FF FF FF FF 20 CF"
    Get_uart_lockStatus = "6D643E 6E 03 00 00 00"

    for i in range(1, 8):
        app_logger.info(f"Attempt: #0{i} Executing actual test Scenario")

        # Step 1: Pre_Condition Test
        app_logger.info("Checking pre-condition: GET check UART Lock Status is UNLOCK-00")
        assert ManDiag_component.send_request_command_and_validate_response(Get_uart_lockStatus, expected_data_length="01", expected_data="00")

        # Step 2: Actual Test
        app_logger.info("Executing actual test: SET LOCK the UART")
        assert ManDiag_component.send_request_command_and_validate_response(Set_uart_lock, expected_data_length="00", expected_data="C0 2F")

        app_logger.info("Validating actual-condition: GET Check UART Lock Status is LOCKED-01")
        assert ManDiag_component.send_request_command_and_validate_response(Get_uart_lockStatus, expected_data_length="01", expected_data="01")

        # Step 3: Post Condition Test
        app_logger.info("Executing Post test: SET UNLOCK the UART")
        assert ManDiag_component.send_request_command_and_validate_response(Set_uart_unlock, expected_data_length="00", expected_data="C0 2F")

        app_logger.info("Validating post-condition: GET Check UART Lock Status is UNLOCKED-00")
        assert ManDiag_component.send_request_command_and_validate_response(Get_uart_lockStatus, expected_data_length="01", expected_data="00")

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #04 *************************************************
@pytest.mark.skip(reason="skipping this because failure of this feature might affect other features ")
@pytest.mark.order(4)
def test_0x6E02_0x6E03_CyberSecurity_SET_GET_validation_for_continuous_UART_Unlock_Operation(ManDiag_component, PowerSupply_component):
    """ CyberSecurity : UART Unlock Operation with wrong Password more than 3times in single power cycle """

    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case: Validate UART Unlock operation with wrong password for 3 times
            Steps:
            1. Pre-condition: Check initial UART lock status (Get command).
            2. UnLock the UART with 16bytes unique key  (Set command).            
            3. Validate post-condition: Verify UART lock status updated (Get command) .

            Expected Result:
            - Uart should be UnLocked successfully.
            """
    app_logger.debug(test_description)
    app_logger.info("Starting test: UART Unlock feature Validation")

    Set_uart_lock = "6D643E 6E 02 11 00 11 01 EE EE EE EE FF FF FF FF EE EE EE EE FF FF FF FF 30 2D"
    Set_uart_unlock = "6D643E 6E 02 11 00 11 00 EE EE EE EE FF FF FF FF EE EE EE EE FF FF FF 55 34 6F"
    Get_uart_lockStatus = "6D643E 6E 03 00 00 00"

    # Step 1: Pre_Condition Test
    app_logger.info("Checking pre-condition: GET check UART Lock Status is UNLOCK-00")
    assert ManDiag_component.send_request_command_and_validate_response(Get_uart_lockStatus, expected_data_length="01", expected_data="00")

    # Step 2: Actual Test
    app_logger.info("Executing actual test: SET LOCK the UART")
    assert ManDiag_component.send_request_command_and_validate_response(Set_uart_lock, expected_data_length="00", expected_data="C0 2F")

    app_logger.info("Validating actual-condition: GET Check UART Lock Status is LOCKED-01")
    assert ManDiag_component.send_request_command_and_validate_response(Get_uart_lockStatus, expected_data_length="01", expected_data="01")

    for i in range(1, 4):
        app_logger.info(f"Attempt: #0{i} Executing actual test Scenario")

        # Step 3: Post Condition Test
        app_logger.info("Executing Post test: SET UNLOCK the UART")
        actual_test_result = ManDiag_component.send_request_command_and_validate_response(Set_uart_unlock, expected_status="00", expected_data_length="00", expected_data="F3 1E")
        if (actual_test_result is not True) and (i == 3):
            app_logger.info(f"Attempt: #0{i} No response is received, it validates 30sec blocking after 2 wrong attempts ")
            time.sleep(30)
            for j in range(3, 7):
                app_logger.info(f"Attempt: #0{j} Executing actual test Scenario")
                actual_test_result = ManDiag_component.send_request_command_and_validate_response(Set_uart_unlock, expected_status="00", expected_data_length="00", expected_data="F3 1E")
                if actual_test_result is not True and j == 6:
                    app_logger.info(f"Attempt: #0{j}  response is received, it will not validates blocking Unlock operations after 5 wrong attempts ")
                    assert False

    # power restart to rest UART block
    PowerSupply_component.restart_power()
    # Enter into mandiag after full ON
    # 30 sec gap full restart and Enter into mandiag after full ON
    for i in range(1, 45):
        time.sleep(1)
        app_logger.info(f"Waiting 30 Seconds for VIP Full ON - {i} Secs")
    assert ManDiag_component.send_request_command_and_validate_response("6D643E 00 01 01 00 01 01"), f"Failed to enter ManDiag Session for UART lock Unlock persistence test"

    app_logger.info("Test case passed successfully!")
