# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_0x6E13_Telematics_Data_SupplierFeed_Feature.py
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
@pytest.mark.order(5)
def test_0x6E13_Telematics_Data_SupplierFeed_TCUSoftwarePartNumber_0x00(ManDiag_component):
    """ TCU : 6E 13 TCUSoftwarePartNumber """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Reading TCU SoftwarePartNumber.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.
                Step 2. Actual Test
                - Send GET Read TCU SoftwarePartNumber and Validate the Response Valid status Byte, Data_length and Data.
                Step 3: Post-condition

            Expected Result:
                - The read TCU SoftwarePartNumber should match to expected data.
                - first 1byte is ManDiag ID
                    -> Second 6bytes AG215 SW Date - (Year|WeekDay|Date) 
                    -> Last 6bytes AG550 SW Date - (Year|WeekDay|Date) 
                    Ex: 6D643E 6E 13 00 00 01 00
                        6D643C 6E 13 00 01 0D 00 32 34 34 36 31 38 32 34 34 36 31 38 [ASCII] => "244618244618"
            """
    app_logger.debug(test_description)
    GetCommand = "6D643E 6E 13 00 00 01 00"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Read and verify TCU SoftwarePartNumber")
    ManDiag_response = ManDiag_component.send_request_command_and_get_response(GetCommand, retries=2)
    if ManDiag_response and ManDiag_response['Status'] == "01" and ManDiag_response['DataLength'] == "0D":
        assert True
    else:
        assert False, f"Actual test failed for TUC SoftwarePartNumber : Expected data length is 0x0D but observed 0x{ManDiag_response['Databytes']} not acceptable"

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #02 *************************************************
@pytest.mark.hpcc_tcu
@pytest.mark.order(6)
def test_0x6E13_Telematics_Data_SupplierFeed_CV2XSoftwarePartNumber_0x01(ManDiag_component):
    """ TCU : 6E 13 CV2XSoftwarePartNumber """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Reading Cv2X SoftwarePartNumber.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.
                Step 2. Actual Test
                - Send GET Read Cv2X SoftwarePartNumber and Validate the Response Valid status Byte, Data_length and Data.
                Step 3: Post-condition

            Expected Result:
                - The read Cv2X SoftwarePartNumber should match to expected data.
                - first 1byte is ManDiag ID
                    -> Second 6bytes AG215 SW Date - (Year|WeekDay|Date) 
                    -> Last 6bytes AG550 SW Date - (Year|WeekDay|Date) 
                    Ex: 6D643E 6E 13 00 00 01 01
                        6D643C 6E 13 00 01 0D 01 32 34 34 36 31 38 32 34 34 36 31 38 [ASCII] => "244618244618"
            """
    app_logger.debug(test_description)
    GetCommand = "6D643E 6E 13 00 00 01 01"

    # Step 1: Pre Condition

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Read and verify Cv2X SoftwarePartNumber")
    ManDiag_response = ManDiag_component.send_request_command_and_get_response(GetCommand, retries=2)
    if ManDiag_response and ManDiag_response['Status'] == "01" and ManDiag_response['DataLength'] == "0D":
        assert True
    else:
        assert False, f"Actual test failed Cv2X SoftwarePartNumber : Expected data length is 0x0D but observed 0x{ManDiag_response['Databytes']} not acceptable"

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #03 *************************************************
@pytest.mark.hpcc_tcu
@pytest.mark.order(7)
def test_0x6E13_Telematics_Data_SupplierFeed_IMEI_Number_0x02(ManDiag_component):
    """ TCU : 6E 13 IMEI_Number """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Reading IMEI Number.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.
                Step 2. Actual Test
                - Send GET Read IMEI Number and Validate the Response Valid status Byte, Data_length and Data.
                Step 3: Post-condition

            Expected Result:
                - The read IMEI Number should match to expected data.
                - first 1byte is ManDiag ID
                    -> First 1byte is ID
                    -> Next 16bytes are Data 
                    Ex: 6D643E 6E 13 00 00 01 02
                        6D643C 6E 13 00 01 10 02 38 36 30 34 38 38 30 35 30 30 37 35 33 34 36 [ASCII] => "860488050075346" 
            """
    app_logger.debug(test_description)
    GetCommand = "6D643E 6E 13 00 00 01 02"

    # Step 1: Pre Condition

    # Step 2: Actual Test
    app_logger.info("Executing actual test: Read and verify IMEI Number")
    ManDiag_response = ManDiag_component.send_request_command_and_get_response(GetCommand, retries=2)
    if ManDiag_response and ManDiag_response['Status'] == "01" and ManDiag_response['DataLength'] == "10":
        assert True
    else:
        assert False, f"Actual test failed IMEI Number : Expected data length is 0x10 but observed 0x{ManDiag_response['Databytes']} not acceptable"

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #04 *************************************************
@pytest.mark.hpcc_tcu
@pytest.mark.order(8)
def test_0x6E13_Telematics_Data_SupplierFeed_eUICCIDNumber_0x03(ManDiag_component):
    """ TCU : 6E 13 eUICCIDNumber """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Reading eUICCID Number.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.
                Step 2. Actual Test
                - Send GET Read eUICCID Number and Validate the Response Valid status Byte, Data_length and Data.
                Step 3: Post-condition

            Expected Result:
                - The read eUICCID Number should match to expected data.
                -> First 1byte is ID
                -> Next 32bytes are Data 
                Ex: 6D643E 6E 13 00 00 01 03
                    6D643C 6E 13 00 01 21 03 38 39 30 33 33 30 32 34 31 30 33 34 30 30 37 33 37 34 30 30 30 30 30 30 30 33 37 33 34 34 37 35 
                    [ASCII] => "89033024103400737400000003734475" 
            """
    app_logger.debug(test_description)
    GetCommand = "6D643E 6E 13 00 00 01 03"

    # Step 1: Pre Condition

    # Step 2: Actual Test
    app_logger.info("Executing actual test: Read and verify eUICCID Number")
    ManDiag_response = ManDiag_component.send_request_command_and_get_response(GetCommand, retries=2)
    if ManDiag_response and ManDiag_response['Status'] == "01" and ManDiag_response['DataLength'] == "21":
        assert True
    else:
        assert False, f"Actual test failed eUICCID Number : Expected data length is 0x21 but observed 0x{ManDiag_response['Databytes']} not acceptable"

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #05 *************************************************
@pytest.mark.hpcc_tcu
@pytest.mark.order(9)
def test_0x6E13_Telematics_Data_SupplierFeed_ICCID_Number_0x05(ManDiag_component):
    """ TCU : 6E 13 ICCID_Number """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Reading ICCID Number.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.
                Step 2. Actual Test
                - Send GET Read ICCID Number and Validate the Response Valid status Byte, Data_length and Data.
                Step 3: Post-condition

            Expected Result:
                - The read ICCID Number should match to expected data.
                -> First 1byte is ID
                -> Next 20bytes are Data 
                Ex: 6D643E 6E 13 00 00 01 05
                    6D643C 6E 13 00 01 15 05 38 39 39 31 38 36 39 30 35 30 37 30 36 31 31 34 31 31 38 36
                    [ASCII] => "89918690507061141186" 
            """
    app_logger.debug(test_description)
    GetCommand = "6D643E 6E 13 00 00 01 05"

    # Step 1: Pre Condition

    # Step 2: Actual Test
    app_logger.info("Executing actual test: Read and verify ICCID Number")
    ManDiag_response = ManDiag_component.send_request_command_and_get_response(GetCommand, retries=2)
    if ManDiag_response and ManDiag_response['Status'] == "01" and ManDiag_response['DataLength'] == "15":
        assert True
    else:
        assert False, f"Actual test failed ICCID Number : Expected data length is 0x15 but observed 0x{ManDiag_response['Databytes']} not acceptable"

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #06 *************************************************
@pytest.mark.hpcc_tcu
@pytest.mark.order(10)
def test_0x6E13_Telematics_Data_SupplierFeed_ESim_Status_0x06(ManDiag_component):
    """ TCU : 6E 13 ESim_Status """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Reading ESim Status.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.
                Step 2. Actual Test
                - Send GET Read ESim Status and Validate the Response Valid status Byte, Data_length and Data.
                Step 3: Post-condition

            Expected Result:
                - The read ESim Status should match to expected data.
                -> First byte is ID 
                -> Second Byte is Status          
                Ex: 6D643E 6E 13 00 00 01 06
                    6D643C 6E 13 00 01 02 06 00
            """
    app_logger.debug(test_description)
    GetCommand = "6D643E 6E 13 00 00 01 06"

    # Step 1: Pre Condition

    # Step 2: Actual Test
    app_logger.info("Executing actual test: Read and verify ESim Status")
    assert ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="02", expected_data="06 00", retries=2)

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #07 *************************************************
@pytest.mark.hpcc_tcu
@pytest.mark.order(11)
def test_0x6E13_Telematics_Data_SupplierFeed_GPS_Status_0x07(ManDiag_component):
    """ TCU : 6E 13 GPS_Status """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
           Test Case Description:
               - This test case verifies the functionality of Reading GPS Status.

           Test Sequence Steps:
               Step 1: Pre-condition
               - Check for telematics handshake after Full ON.
               Step 2. Actual Test
               - Send GET Read GPS Status and Validate the Response Valid status Byte, Data_length and Data.
               Step 3: Post-condition

           Expected Result:
               - The read GPS Status should match to expected data.
               -> First byte is ID 
               -> Second Byte is Status          
               Ex: 6D643E 6E 13 00 00 01 07
                   6D643C 6E 13 00 01 02 07 00
           """
    app_logger.debug(test_description)
    GetCommand = "6D643E 6E 13 00 00 01 07"
    expected_data_length = "02"

    # Step 1: Pre Condition

    # Step 2: Actual Test
    app_logger.info("Executing actual test: Read and verify GPS Status")
    assert ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="02", expected_data="07 00", retries=2)

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #8 *************************************************
@pytest.mark.hpcc_tcu
@pytest.mark.order(12)
def test_0x6E13_Telematics_Data_SupplierFeed_IMU_Status_0x08(ManDiag_component):
    """ TCU : 6E 13 IMU_Status """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
           Test Case Description:
               - This test case verifies the functionality of Reading IMU Status.

           Test Sequence Steps:
               Step 1: Pre-condition
               - Check for telematics handshake after Full ON.
               Step 2. Actual Test
               - Send GET Read IMU Status and Validate the Response Valid status Byte, Data_length and Data.
               Step 3: Post-condition

           Expected Result:
               - The read IMU Status should match to expected data.
               -> First byte is ID 
               -> Second Byte is Status          
               Ex: 6D643E 6E 13 00 00 01 08
                   6D643C 6E 13 00 01 02 08 00
           """
    app_logger.debug(test_description)
    GetCommand = "6D643E 6E 13 00 00 01 08"

    # Step 1: Pre Condition

    # Step 2: Actual Test
    app_logger.info("Executing actual test: Read and verify IMU Status")
    assert ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="02", expected_data="08 00", retries=2)

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #9 *************************************************
@pytest.mark.hpcc_tcu
@pytest.mark.order(13)
def test_0x6E13_Telematics_Data_SupplierFeed_BSP_5G_AG550_0x09(ManDiag_component):
    """ TCU : 6E 13 BSP_5G_AG550 """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Reading BSP version 5G_AG550.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.
                Step 2. Actual Test
                - Send GET Read BSP version 5G_AG550 and Validate the Response Valid status Byte, Data_length and Data.
                Step 3: Post-condition

            Expected Result:
                - The read BSP version 5G_AG550 should match to expected data.
                -> First 1byte is ManDiag ID 
                -> next 32Bytes is Status          
                Ex: 6D643E 6E 13 00 00 01 09
                    6D643C 6E 13 00 01 21 09 41 47 35 35 30 51 45 55 41 42 52 30 33 41 30 37 4D 38 47 5F 4F 43 50 55 5F 30 31 2E 30 30 31 00
                    [ASCII] - "AG550QEUABR03A07M8G_OCPU_01.001"
            """
    app_logger.debug(test_description)
    GetCommand = "6D643E 6E 13 00 00 01 09"

    # Step 1: Pre Condition

    # Step 2: Actual Test
    app_logger.info("Executing actual test: Read and verify BSP version 5G_AG550")
    ManDiag_response = ManDiag_component.send_request_command_and_get_response(GetCommand, retries=2)
    if ManDiag_response and ManDiag_response['Status'] == "01" and ManDiag_response['DataLength'] == "21":
        assert True
    else:
        assert False, f"Actual test failed for BSP version 5G_AG550 : Expected data length is 0x21 but observed 0x{ManDiag_response['Databytes']} not acceptable"

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #10 *************************************************
@pytest.mark.hpcc_tcu
@pytest.mark.order(14)
def test_0x6E13_Telematics_Data_SupplierFeed_BSP_Cv2x_AG215_0x0A(ManDiag_component):
    """ TCU : 6E 13 BSP_Cv2x_AG215 """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Reading BSP version Cv2x_AG215.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.
                Step 2. Actual Test
                - Send GET Read BSP version Cv2x_AG215 and Validate the Response Valid status Byte, Data_length and Data.
                Step 3: Post-condition

            Expected Result:
                - The read BSP version Cv2x_AG215 should match to expected data.
                -> First 1byte is ManDiag ID 
                -> next 32Bytes is Status          
                Ex: 6D643E 6E 13 00 00 01 0A
                    6D643C 6E 13 00 01 21 0A 41 47 32 31 35 53 47 4C 42 41 52 31 35 41 30 37 4D 34 47 5F 4F 43 50 55 5F 53 41 35 31 35 4D 00
                    [ASCII] - "AG215SGLBAR15A07M4G_OCPU_SA515M"
            """
    app_logger.debug(test_description)
    GetCommand = "6D643E 6E 13 00 00 01 0A"
    expected_data_length = "21"

    # Step 1: Pre Condition

    # Step 2: Actual Test
    app_logger.info("Executing actual test: Read and verify BSP version Cv2x_AG215")
    ManDiag_response = ManDiag_component.send_request_command_and_get_response(GetCommand, retries=2)
    if ManDiag_response and ManDiag_response['Status'] == "01" and ManDiag_response['DataLength'] == "21":
        assert True
    else:
        assert False, f"Actual test failed for BSP version Cv2x_AG215 : Expected data length is 0x21 but observed 0x{ManDiag_response['Databytes']} not acceptable"

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #11 *************************************************
@pytest.mark.hpcc_tcu
@pytest.mark.order(15)
def test_0x6E13_Telematics_Data_SupplierFeed_EthernetMAC_Cv2x_0x0B(ManDiag_component):
    """ TCU : 6E 13 EthernetMAC_Cv2x """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Reading EthernetMAC for Cv2x_AG215.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.
                Step 2. Actual Test
                - Send GET Read EthernetMAC for Cv2x_AG215 and Validate the Response Valid status Byte, Data_length and Data.
                Step 3: Post-condition

            Expected Result:
                - The read EthernetMAC for Cv2x_AG215 should match to expected data.
                -> First 1byte is ManDiag ID 
                -> next 32Bytes is Status          
                Ex: 6D643E 6E 13 00 00 01 0B
                    6D643C 6E 13 00 01 12 0B 30 32 3A 30 30 3A 64 38 3A 39 32 3A 62 64 3A 35 30
                    [ASCII] - "02:00:d8:92:bd:50"
            """
    app_logger.debug(test_description)
    GetCommand = "6D643E 6E 13 00 00 01 0B"

    # Step 1: Pre Condition

    # Step 2: Actual Test
    app_logger.info("Executing actual test: Read and verify EthernetMAC for Cv2x_AG215")
    ManDiag_response = ManDiag_component.send_request_command_and_get_response(GetCommand, retries=2)
    if ManDiag_response and ManDiag_response['Status'] == "01" and ManDiag_response['DataLength'] == "12":
        assert True
    else:
        assert False, f"Actual test failed for EthernetMAC - Cv2x_AG215 : Expected data length is 0x12 but observed 0x{ManDiag_response['Databytes']} not acceptable"

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #12 *************************************************
@pytest.mark.hpcc_tcu
@pytest.mark.order(16)
def test_0x6E13_Telematics_Data_SupplierFeed_EthernetMAC_5G_0x0C(ManDiag_component):
    """ TCU : 6E 13 EthernetMAC_5G """
    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Reading EthernetMAC for 5G.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Check for telematics handshake after Full ON.
                Step 2. Actual Test
                - Send GET Read EthernetMAC for 5G and Validate the Response Valid status Byte, Data_length and Data.
                Step 3: Post-condition

            Expected Result:
                - The read EthernetMAC for 5G should match to expected data.
                -> First 1byte is ManDiag ID 
                -> next 32Bytes is Status          
                Ex: 6D643E 6E 13 00 00 01 0C
                    6D643C 6E 13 00 01 12 0C 30 32 3A 30 30 3A 64 38 3A 39 32 3A 62 64 3A 34 66
                    [ASCII] - "02:00:d8:92:bd:4f"
            """
    app_logger.debug(test_description)
    GetCommand = "6D643E 6E 13 00 00 01 0C"

    # Step 1: Pre Condition

    # Step 2: Actual Test
    app_logger.info("Executing actual test: Read and verify EthernetMAC for 5G")
    ManDiag_response = ManDiag_component.send_request_command_and_get_response(GetCommand, retries=2)
    if ManDiag_response and ManDiag_response['Status'] == "01" and ManDiag_response['DataLength'] == "12":
        assert True
    else:
        assert False, f"Actual test failed for EthernetMAC - 5G : Expected data length is 0x12 but observed 0x{ManDiag_response['Databytes']} not acceptable"

    # Step 3: Post Condition
    app_logger.info("Test case passed successfully!")
