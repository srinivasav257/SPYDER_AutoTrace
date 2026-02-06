# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x1F02_Tuner_Measurement_Mode.py
# Author:       Srinivasa V
# Created:      Nov 07 2025
# Description:  This File contains Test case scenarios for validating Tuner feature.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2024 APTIV
#
# This file is part of the MIB4 project - Manufacture Diagnostics Component.
#
# Manufacture Diagnostics Project is free software: you can redistribute it
# and/or modify it under the terms of Diagnostics Department ASUX.
# ------------------------------------------------------------------------------------------------------------------

import time

from infotest.core.trace_logger import app_logger
import pytest

# ******************************************************************************************************************************

#   (Case_Id,   Test_Name,                Set_Primary_Source,         Get_Expected_Primary_Source,    Set_Tuner_Measurement_Mode,           Set_Expected_Data,                Get_Tuner_Measurement_Mode,    Get_Expected_Data)

TEST_CASES = [
    (1, "Set_PSFM_Tuner1_MM_ON_DM_FM1", "6D643E 14 08 01 00 01 01",  "6D643C 14 08 00 01 01 01", "6D643E 1F 02 01 00 04 01 00 01 01", "6D643C 1F 02 01 01 04 01 00 01 01", "6D643E 1F 02 00 00 01 01", "6D643C 1F 02 01 01 04 01 00 01 01"),
    (2, "Set_PSFM_Tuner1_MM_OFF_DM_FM1", "6D643E 14 08 01 00 01 01", "6D643C 14 08 00 01 01 01", "6D643E 1F 02 01 00 04 00 00 01 01", "6D643C 1F 02 01 01 04 00 00 01 01", "6D643E 1F 02 00 00 01 01", "6D643C 1F 02 01 01 04 00 00 01 01"),
    (3, "Set_PSFM_Tuner1_MM_ON_DM_FM2", "6D643E 14 08 01 00 01 01",  "6D643C 14 08 00 01 01 01", "6D643E 1F 02 01 00 04 01 00 01 02", "6D643C 1F 02 01 01 04 01 00 01 02", "6D643E 1F 02 00 00 01 01", "6D643C 1F 02 01 01 04 01 00 01 02"),
    (4, "Set_PSFM_Tuner1_MM_OFF_DM_FM2", "6D643E 14 08 01 00 01 01", "6D643C 14 08 00 01 01 01", "6D643E 1F 02 01 00 04 00 00 01 02", "6D643C 1F 02 01 01 04 00 00 01 02", "6D643E 1F 02 00 00 01 01", "6D643C 1F 02 01 01 04 00 00 01 02"),
    (5, "Set_PSFM_Tuner1_MM_ON_PD_FM1", "6D643E 14 08 01 00 01 01",  "6D643C 14 08 00 01 01 01", "6D643E 1F 02 01 00 04 01 01 01 01", "6D643C 1F 02 01 01 04 01 01 01 01", "6D643E 1F 02 00 01 01 01", "6D643C 1F 02 01 01 04 01 01 01 01"),
    (6, "Set_PSFM_Tuner1_MM_OFF_PD_FM1", "6D643E 14 08 01 00 01 01", "6D643C 14 08 00 01 01 01", "6D643E 1F 02 01 00 04 00 01 01 01", "6D643C 1F 02 01 01 04 00 01 01 01", "6D643E 1F 02 00 01 01 01", "6D643C 1F 02 01 01 04 00 01 01 01"),
    (7, "Set_PSFM_Tuner1_MM_ON_PD_FM2", "6D643E 14 08 01 00 01 01",  "6D643C 14 08 00 01 01 01", "6D643E 1F 02 01 00 04 01 01 01 02", "6D643C 1F 02 01 01 04 01 01 01 02", "6D643E 1F 02 00 01 01 01", "6D643C 1F 02 01 01 04 01 01 01 02"),
    (8, "Set_PSFM_Tuner1_MM_OFF_PD_FM2", "6D643E 14 08 01 00 01 01", "6D643C 14 08 00 01 01 01", "6D643E 1F 02 01 00 04 00 01 01 02", "6D643C 1F 02 01 01 04 00 01 01 02", "6D643E 1F 02 00 01 01 01", "6D643C 1F 02 01 01 04 00 01 01 02"),
    (9, "Set_PSAM_Tuner1_MM_ON_DM_FM1", "6D643E 14 08 01 00 01 02",  "6D643C 14 08 00 01 01 02", "6D643E 1F 02 01 00 04 01 00 01 01", "6D643C 1F 02 01 01 04 01 00 01 01", "6D643E 1F 02 00 00 01 01", "6D643C 1F 02 01 01 04 01 00 01 01"),
    (10, "Set_PSAM_Tuner1_MM_OFF_DM_FM1","6D643E 14 08 01 00 01 02", "6D643C 14 08 00 01 01 02", "6D643E 1F 02 01 00 04 00 00 01 01", "6D643C 1F 02 01 01 04 00 00 01 01", "6D643E 1F 02 00 00 01 01", "6D643C 1F 02 01 01 04 00 00 01 01"),
    (11, "Set_PSAM_Tuner1_MM_ON_DM_FM2", "6D643E 14 08 01 00 01 02",  "6D643C 14 08 00 01 01 02", "6D643E 1F 02 01 00 04 01 00 01 02", "6D643C 1F 02 01 01 04 01 00 01 02", "6D643E 1F 02 00 00 01 01", "6D643C 1F 02 01 01 04 01 00 01 02"),
    (12, "Set_PSAM_Tuner1_MM_OFF_DM_FM2", "6D643E 14 08 01 00 01 02", "6D643C 14 08 00 01 01 02", "6D643E 1F 02 01 00 04 00 00 01 02", "6D643C 1F 02 01 01 04 00 00 01 02", "6D643E 1F 02 00 00 01 01", "6D643C 1F 02 01 01 04 00 00 01 02"),
    (13, "Set_PSAM_Tuner1_MM_ON_PD_FM1", "6D643E 14 08 01 00 01 02",  "6D643C 14 08 00 01 01 02", "6D643E 1F 02 01 00 04 01 01 01 01", "6D643C 1F 02 01 01 04 01 01 01 01", "6D643E 1F 02 00 01 01 01", "6D643C 1F 02 01 01 04 01 01 01 01"),
    (14, "Set_PSAM_Tuner1_MM_OFF_PD_FM1", "6D643E 14 08 01 00 01 02", "6D643C 14 08 00 01 01 02", "6D643E 1F 02 01 00 04 00 01 01 01", "6D643C 1F 02 01 01 04 00 01 01 01", "6D643E 1F 02 00 01 01 01", "6D643C 1F 02 01 01 04 00 01 01 01"),
    (15, "Set_PSAM_Tuner1_MM_ON_PD_FM2", "6D643E 14 08 01 00 01 02",  "6D643C 14 08 00 01 01 02", "6D643E 1F 02 01 00 04 01 01 01 02", "6D643C 1F 02 01 01 04 01 01 01 02", "6D643E 1F 02 00 01 01 01", "6D643C 1F 02 01 01 04 01 01 01 02"),
    (16, "Set_PSAM_Tuner1_MM_OFF_PD_FM2", "6D643E 14 08 01 00 01 02", "6D643C 14 08 00 01 01 02", "6D643E 1F 02 01 00 04 00 01 01 02", "6D643C 1F 02 01 01 04 00 01 01 02", "6D643E 1F 02 00 01 01 01", "6D643C 1F 02 01 01 04 00 01 01 02")]

# ************************************************************* TEST CASES #01 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_Tuner_0x1F02
@pytest.mark.order(215)
@pytest.mark.parametrize(
    "case_id, test_name, set_primary_cmd, expected_primary_resp, set_mm_cmd, expected_set_resp, get_mm_cmd, expected_get_resp",
    TEST_CASES,
)
def test_0x1F02_Tuner_SET_GET_TunerMeasurementMode_FM(
    ManDiag_component,
    case_id,
    test_name,
    set_primary_cmd,
    expected_primary_resp,
    set_mm_cmd,
    expected_set_resp,
    get_mm_cmd,
    expected_get_resp,
):
    """ QNX_Tuner : Set Tuner Measurement Mode for FM """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of setting and Reading Tuner frequency for FM.

           Steps:
               Pre-condition
                - Do source switch to FM using 0x1408 Primary Sournce Switch Command.
               Actual Test
                - Send SET Tuner 1 to Measurement mode:ON, Operation mode:Dual Mode, Tuner:01, Antenna:01  and Validate the Response Valid status Byte, Data_length and Data.
               Post-condition
                - Send GET Read FM Tuner Measurement Mode and validate with expected data.

            Expected Result:
                - The user should be able SET Tuner Mode and read the present mode.                
            """
    app_logger.debug(test_description)
    app_logger.info(f"[Case {case_id}] Executing test: {test_name}")

    # Step 1: Pre_Condition Test
    app_logger.info("Executing Pre_Condition test: Switch source to FM/AM")
    assert ManDiag_component.send_request_command_and_validate_response(
        set_primary_cmd,
        expected_data_length="01",
        expected_data=expected_primary_resp,
    )

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Set Tuner Measurement Mode to FM/AM")
    assert ManDiag_component.send_request_command_and_validate_response(
        set_mm_cmd,
        expected_data_length="04",
        expected_data=expected_set_resp,
    )

    # Step 3: Post Condition
    app_logger.info("Executing Pre_Condition test: Read and verify Tuner Measurement Mode to FM/AM")
    assert ManDiag_component.send_request_command_and_validate_response(
        get_mm_cmd,
        expected_data_length="04",
        expected_data=expected_get_resp,
    )

    app_logger.info("Test case passed successfully!")

# ************************************************************* TEST CASES #02 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_Tuner_0x1F02
@pytest.mark.order(216)
def test_0x1F02_Tuner_SET_TunerMeasurementMode_FM_Command_Error_Format(ManDiag_component):
    """ QNX_Tuner : Set Tuner Measurement Mode for FM """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of setting and Reading Tuner frequency for FM.

           Steps:
               Pre-condition
                - Do source switch to FM using 0x1408 Primary Sournce Switch Command.
               Actual Test
                - Send SET Tuner with mismatched data length and and Validate the Response Valid status Byte, Data_length and Data.
               Post-condition
                - The Response with status byte should be ERROR: "F6"

            Expected Result:
                - The response should indicate that given operation data length is invalid                
            """
    app_logger.info(test_description)
    app_logger.info("Executing actual test: Set Tuner Measurement Mode to FM")
    SetCmd = "6D643E 1F 02 01 00 03 00 01 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_status="F6")

    # Step 3: Post Test Case

    app_logger.info("Test case cmd execution failed!")


# ************************************************************* TEST CASES #03 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_Tuner_0x1F02
@pytest.mark.order(217)
def test_0x1F02_Tuner_GET_TunerMeasurementMode_FM_Command_Error_Format(ManDiag_component):
    """ QNX_Tuner : Get Tuner Measurement Mode for FM """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of setting and Reading Tuner frequency for FM.

           Steps:
               Pre-condition
                - Do source switch to FM using 0x1408 Primary Sournce Switch Command.
               Actual Test
                - Send GET Tuner with mismatched data length and and Validate the Response Valid status Byte, Data_length and Data.
               Post-condition
                - The Response with status byte should be ERROR: "F6"

            Expected Result:
                - The response should indicate that given operation data length is invalid                
            """
    app_logger.info(test_description)
    app_logger.info("Executing actual test: Get Tuner Measurement Mode to FM")
    GetCmd = "6D643E 1F 02 00 00 01 01 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: GET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_status="F6")

    # Step 3: Post Test Case

    app_logger.info("Test case cmd execution failed!")
    
    
# ************************************************************* TEST CASES #04 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_Tuner_0x1F02
@pytest.mark.order(218)
def test_0x1F02_Tuner_SET_TunerMeasurementMode_FM_Mismatch_Data_Length(ManDiag_component):
    """ QNX_Tuner : Set Tuner Measurement Mode for FM """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of setting and Reading Tuner frequency for FM.

           Steps:
               Pre-condition
                - Do source switch to FM using 0x1408 Primary Sournce Switch Command.
               Actual Test
                - Send SET Tuner with wrong data length and and Validate the Response Valid status Byte, Data_length and Data.
               Post-condition
                - The Response with status byte should be ERROR: "F8"

            Expected Result:
                - The response should indicate that given operation data length is invalid                
            """
    app_logger.info(test_description)
    app_logger.info("Executing actual test: Set Tuner Measurement Mode to FM")
    SetCmd = "6D643E 1F 02 01 00 03 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_status="F8")

    # Step 3: Post Test Case

    app_logger.info("Test case cmd execution failed!")

    
# ************************************************************* TEST CASES #05 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_Tuner_0x1F02
@pytest.mark.order(219)
def test_0x1F02_Tuner_GET_TunerMeasurementMode_FM_Mismatch_Data_Length(ManDiag_component):
    """ QNX_Tuner : Get Tuner Measurement Mode for FM """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of setting and Reading Tuner frequency for FM.

           Steps:
               Pre-condition
                - Do source switch to FM using 0x1408 Primary Sournce Switch Command.
               Actual Test
                - Send GET Tuner with wrong data length and and Validate the Response Valid status Byte, Data_length and Data.
               Post-condition
                - The Response with status byte should be ERROR: "F8"

            Expected Result:
                - The response should indicate that given operation data length is invalid                
            """
    app_logger.info(test_description)
    app_logger.info("Starting test: Tuner measurement feature Validation")
    GetCmd = "6D643E 1F 02 00 00 02 01 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: GET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_status="F8")

    # Step 3: Post Test Case

    app_logger.info("Test case cmd execution failed!")


# ************************************************************* TEST CASES #06 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_Tuner_0x1F02
@pytest.mark.order(220)
def test_0x1F02_Tuner_SET_TunerMeasurementMode_FM_Function_Not_Supported(ManDiag_component):
    """ QNX_Tuner : Set Tuner Measurement Mode for FM """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of setting and Reading Tuner frequency for FM.

           Steps:
               Pre-condition
                - Do source switch to FM using 0x1408 Primary Sournce Switch Command.
               Actual Test
                - Send SET Tuner with not supported Operation byte 03 and and Validate the Response Valid status Byte, Data_length and Data.
               Post-condition
                - The Response with status byte should be ERROR: "F7"

            Expected Result:
                - The response should indicate that given operation data length is invalid                
            """
    app_logger.info(test_description)
    app_logger.info("Executing actual test: Set Tuner Measurement Mode to FM")
    SetCmd = "6D643E 1F 02 03 00 04 01 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_status="F7")

    # Step 3: Post Test Case

    app_logger.info("Test case cmd execution failed!")    

    
# ************************************************************* TEST CASES #07 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_Tuner_0x1F02
@pytest.mark.order(221)
def test_0x1F02_Tuner_GET_TunerMeasurementMode_FM_Function_Not_Supported(ManDiag_component):
    """ QNX_Tuner : Get Tuner Measurement Mode for FM """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of setting and Reading Tuner frequency for FM.

           Steps:
               Pre-condition
                - Do source switch to FM using 0x1408 Primary Sournce Switch Command.
               Actual Test
                - Send GET Tuner with not supported Operation byte 03 and and Validate the Response Valid status Byte, Data_length and Data.
               Post-condition
                - The Response with status byte should be ERROR: "F7"

            Expected Result:
                - The response should indicate that given operation data length is invalid                
            """
    app_logger.info(test_description)
    app_logger.info("Executing actual test: Get Tuner Measurement Mode to FM")
    GetCmd = "6D643E 1F 02 03 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: GET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_status="F7")

    # Step 3: Post Test Case

    app_logger.info("Test case cmd execution failed!")
    

# ************************************************************* TEST CASES #08 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_Tuner_0x1F02
@pytest.mark.order(222)
def test_0x1F02_Tuner_SET_TunerMeasurementMode_FM_Invalid_Operation_Or_Data_Byte(ManDiag_component):
    """ QNX_Tuner : Set Tuner Measurement Mode for FM """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of setting and Reading Tuner frequency for FM.

           Steps:
               Pre-condition
                - Do source switch to FM using 0x1408 Primary Sournce Switch Command.
               Actual Test
                - Send SET Tuner with not supported Tuner byte 05 and and Validate the Response Valid status Byte, Data_length and Data.
               Post-condition
                - The Response with status byte should be ERROR: "00"

            Expected Result:
                - The response should indicate that given operation data length is invalid                
            """
    app_logger.info(test_description)
    app_logger.info("Executing actual test: Set Tuner Measurement Mode to FM")
    SetCmd = "6D643E 1F 02 01 00 04 01 00 05 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: SET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(SetCmd, expected_status="00")

    # Step 3: Post Test Case

    app_logger.info("Test case cmd execution failed!")    

    
# ************************************************************* TEST CASES #09 *************************************************
@pytest.mark.mib4_regression_test
@pytest.mark.mib4_Tuner_0x1F02
@pytest.mark.order(223)
def test_0x1F02_Tuner_GET_TunerMeasurementMode_FM_Invalid_Operation_Or_Data_Byte(ManDiag_component):
    """ QNX_Tuner : Get Tuner Measurement Mode for FM """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of setting and Reading Tuner frequency for FM.

           Steps:
               Pre-condition
                - Do source switch to FM using 0x1408 Primary Sournce Switch Command.
               Actual Test
                - Send GET Tuner with not supported Tuner byte 05 and and Validate the Response Valid status Byte, Data_length and Data.
               Post-condition
                - The Response with status byte should be ERROR: "00"

            Expected Result:
                - The response should indicate that given operation data length is invalid                
            """
    app_logger.info(test_description)
    app_logger.info("Executing actual test: Get Tuner Measurement Mode to FM")
    GetCmd = "6D643E 1F 02 00 00 01 05"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: GET and verify ")
    assert ManDiag_component.send_request_command_and_validate_response(GetCmd, expected_status="00")

    # Step 3: Post Test Case

    app_logger.info("Test case cmd execution failed!")