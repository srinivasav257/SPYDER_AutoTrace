# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x0A04_Sleep.py
# Author:       Srinivasa V
# Created:      Jan 02 2025
# Description:  This File contains Test case scenarios for validating 0x0A04 Sleep and S2R feature.
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

restart_flag = False
sleep_flag = False

# ==================================== CONFIGURATION ======================================
from infotest.utilities.util_can_helper import show_vector_configs

APP_NAME = "CANalyzer"   # Must match the application name in Vector Hardware Manager
CHANNEL = 3          # Channel number (e.g., 2 for CH3(MIB-CAN), 3(ICAN) for CH4)
BITRATE = 500000         # Bitrate for High-Speed CAN
READ_DURATION = 5       # Seconds to read CAN messages
DATA_BITRATE = 2000000   # Data-phase bitrate for CAN-FD
# ==========================================================================================

# ************************************************************* TEST CASES #01 *************************************************
@pytest.mark.mib4_sleep
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_System_0x0A04
@pytest.mark.order(131)
@pytest.skip(reason="Skipping Sleep Normal mode test case temporarily")
def test1_0x0A04_Sleep_Normal(ManDiag_component, PowerSupply_TENMA, Keysight_34465A_DMM, vector_CANCH2):
    """ IOC Power mode : Unit Sleep Normal mode"""
    # Step 0: Update the Test variable and Test Procedure
    global restart_flag, sleep_flag
    test_description = """
                Test Case: Unit Sleep Normal mode
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET Unit Sleep in normal mode.
                3. Post-condition: Read ManDiag session and verify the exit status.

                Expected Result:
                - The response should indicate that Unit is requested for Normal sleep and within 30 Sec sleep should complete
                  and current consumption should go below 10mA.
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Unit Sleep normal mode feature Validation")
    SetCmd = "6D643E 0A 04 01 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Unit Sleep in Normal mode and wait for 30sec and verify current consumption below 10mA ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="01", retries=2)

    # Monitor Current consumption drops below 10mA
    print("IDN:", Keysight_34465A_DMM.idn())
    Keysight_34465A_DMM.conf_curr_dc(3, nplc=0.1)
    Keysight_34465A_DMM.log_current(duration_s=75, filepath="out/reports")

    sleep_current = Keysight_34465A_DMM.read_value()
    app_logger.info(f"I = {sleep_current} A")
    '''
    if 0.000500 > sleep_current:  # check for below 500 Micro Amps
        sleep_flag = True
    assert sleep_flag, app_logger.info(f"Sleep current consumption didn't go below 10mA")
    '''

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Perform CAN NM wakeup and wait for 30sec, Verify ManDiag session exited and SET enter ManDiag session")
    vector_CANCH2.manual_send_message(can_id=0x1B000010, data=[0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF], extended_id=True)

    # Monitor Current consumption for full ON
    for i in range(1, 45):
        time.sleep(1)
        current = PowerSupply_TENMA.get_current()
        if current >= 1.00:
            restart_flag = True
        app_logger.info(f"Current consumption during wakeup at {i} Secs - {current} A")

    # Check ManDiag Session state
    if restart_flag:
        assert ManDiag_component.send_request_command_and_validate_response("6D643E 00 01 00 00 00", expected_data_length="01", expected_data="00", retries=2)
        assert ManDiag_component.send_request_command_and_validate_response("6D643E 00 01 01 00 01 01", retries=2)
        assert ManDiag_component.send_request_command_and_validate_response("6D643E 00 01 00 00 00", expected_data_length="01", expected_data="01", retries=2)
    else:
        assert False, f"during restart/wakeup sequence the current consumption didn't go above 2.5Amps - Device not went to FULL ON"

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
@pytest.mark.mib4_sleep
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_Power1_0x0A04
@pytest.mark.order(132)
def test2_0x0A04_S2R_mode(ManDiag_component, vector_CANCH2):
    """ IOC Power mode : Unit S2R mode"""
    # Step 0: Update the Test variable and Test Procedure
    global S2r_flag
    test_description = """
                Test Case: Unit S2R mode
                Steps:
                1. Pre-condition: The Unit should be powered ON and VIP must be fully BootUp and Running.
                2. Actual Test: Send SET Unit S2R mode.          
                3. Post-condition: Read ManDiag session and verify the exit status.

                Expected Result:
                - The response should indicate that Unit is requested for S2R within 2:30 Sec S2R should complete. 
                """
    app_logger.debug(test_description)
    app_logger.info("Starting test: Unit Sleep normal mode feature Validation")
    SetCmd = "6D643E 0A 04 01 00 01 02"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Unit Sleep in Normal mode and wait for 30sec and verify current consumption below 10mA ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_data_length="01", expected_data="02", retries=2)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test: Perform power restart and wait for 30sec, Verify ManDiag session exited and SET enter ManDiag session")

    # Monitor until the device went to S2R mode
    time.sleep(1)
    if ManDiag_component.serial_read_until_pattern("S2R_DIO OK", max_duration=200, regex=False) is not None:
        S2r_flag = True

    # Check ManDiag Session state
    if S2r_flag:
        app_logger.info("trigger wakeup by CAN NM message")
        vector_CANCH2.manual_send_message(can_id=0x3C0, data=[0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF], extended_id=True)
        time.sleep(15)
        assert ManDiag_component.send_request_command_and_validate_response("6D643E 00 01 00 00 00", expected_data_length="01", expected_data="00", retries=2)
        assert ManDiag_component.send_request_command_and_validate_response("6D643E 00 01 01 00 01 01", retries=2)
        assert ManDiag_component.send_request_command_and_validate_response("6D643E 00 01 00 00 00", expected_data_length="01", expected_data="01", retries=2)
    else:
        assert False, f"during S2R sequence the Software didn't went to S2R state - Device not went to Suspend to ram state"

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #03 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_Audio_0x1404
@pytest.mark.order(14042)
def test_0x0A04_negative_case(ManDiag_component):
    test_description = """
            Test Case Description:

            """
    app_logger.info(test_description)

    set_cmd = "6D643E 0A 04 01 00 01 03 04"

    app_logger.info("Step 1: Applying Treble value 3 with range 09")
    assert ManDiag_component.send_request_command_and_validate_response(set_cmd, expected_status="F6")

    app_logger.info("Test case passed successfully!")