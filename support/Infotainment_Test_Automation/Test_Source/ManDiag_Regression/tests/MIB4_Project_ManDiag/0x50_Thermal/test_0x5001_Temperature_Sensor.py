# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x5001_Temperature_Sensor.py
# Author:       Srinivasa V
# Created:      JAN 1 2025
# Description:  This File contains Test case scenarios for performing USB_Voltage enable/disable.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2025 APTIV
#
# This file is part of the MIB4 project - Manufacture Diagnostics Component feature test Automation.
#
# Manufacture Diagnostics Project is free software: you can redistribute it and/or modify it under the terms
# of Diagnostics Department ASUX terms and conditions.
# ------------------------------------------------------------------------------------------------------------------

import time
import serial

from diagnostics.logging_setup import logger, print_owl_symbol
import pytest
from diagnostics.TENMASerial import TenmaSerial

# Global variable data

# Fixture functions


# ************************************************************* TEST CASES #01 *************************************************
# CustomId
customID = ("Main Board (IOC)",
            "MOST FOT",
            "RSC (J6)",
            "MMX PMIC",
            "MMX MEM",
            "MMX SOC",
           
            )

# TestParameters           TestName              GET Command
testParametersFor_IOPort = [("Main Board (IOC)", "6D643E 50 01 00 00 01 01"),
                            ("MOST FOT", "6D643E 50 01 00 00 01 02"),
                            ("RSC (J6)", "6D643E 50 01 00 00 01 05"),
                            ("MMX PMIC", "6D643E 50 01 00 00 01 06"),
                            ("MMX MEM", "6D643E 50 01 00 00 01 07"),
                            ("MMX SOC", "6D643E 50 01 00 00 01 08"),
                           
                            ]


# @pytest.mark.skip(reason="No need to disable SOH if Display is connected")
@pytest.mark.order(33)
@pytest.mark.parametrize("TestName, GetCmd", testParametersFor_IOPort, ids=customID)
def test_0x5001_Temperature_Sensor(diag_test, TestName, GetCmd):
    """ VIP-Diagnostics: Read temperature sensor data """

    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of temperature sensors data reading.

            Test Sequence Steps:
                Step 1: Pre-condition
                - 
                Step 2. Actual Test
                -        
                Step 3: Post-condition
                - 

            Expected Result:
                - 
            """
    logger.debug(test_description)

    # Step 1: Pre Condition Test
    logger.info("Executing Pre Conditional test: Read and verify ")

    # Step 2: Actual Test
    logger.info("Executing actual test: Read and verify ")
    actual_test_result = diag_test.send_and_validate_command(GetCmd,expected_data_length="03")
    if actual_test_result and diag_test.data[3:] == "00 00":
        assert False, f"Actual test failed for {GetCmd} : received Data is {diag_test.data} not valid"
    else:
        assert actual_test_result, f"Actual test failed for command '{GetCmd}' \nExpected - statusByte: 01, Datalength: 03, Data: xx xx xx \n" \
                               f"Observed - statusByte: {diag_test.status}, Datalength: {diag_test.data_length}, Data: {diag_test.data}"

    # Step 3: Post Condition Test
    logger.info("Executing Post test: Read and verify ")

    logger.info("Test case passed successfully!")
