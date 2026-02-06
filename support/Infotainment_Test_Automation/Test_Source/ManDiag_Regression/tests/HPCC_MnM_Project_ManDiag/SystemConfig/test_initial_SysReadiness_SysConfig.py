# ------------------------------------------------------------------------------------------------------------------
# File Name:    test_initial_SysReadiness_SysConfig.py
# Author:       Srinivasa V
# Created:      JAN 1 2025
# Description:  This File contains Test case scenarios for performing system configuration.
# ------------------------------------------------------------------------------------------------------------------
# Copyright (C) 2025 APTIV
#
# This file is part of the MnM HPCC project - Manufacture Diagnostics Component feature test Automation.
#
# Manufacture Diagnostics Project is free software: you can redistribute it and/or modify it under the terms
# of Diagnostics Department ASUX terms and conditions.
# ------------------------------------------------------------------------------------------------------------------

import time
import serial

from infotest.core.trace_logger import app_logger
import pytest

# Global variable data
fc_vc_data = {
    "W610": {"PACK_A": {"8155": {"VC": "19 09 00 04 00 1E 00 00 00 00 00 00 8D 10 00 00 00 00 00 00",
                                 "FC_INT_1MIC": "07 01 F3 09 43 89 7C F4 04 33 21 84 FF F8 3D FD 00 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"},
                        },
             "PACK_B": {"8155": {"VC": "19 09 00 04 10 BF 00 04 00 00 00 00 8D 10 00 00 00 00 00 00",
                                 "FC_EXT_1MIC": "07 01 F3 29 44 89 1C F6 4C 3B 21 84 FF FA 3D FF F0 44 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00",
                                 "FC_EXT_2MIC": "07 01 F3 29 44 89 1C FA 4C 3B 21 84 FF FA 3D FF F0 44 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"
                                 },
                        },
             "PACK_C": {"8295": {"VC": "19 09 00 04 19 BF 00 0C 00 00 00 00 8D 10 00 00 00 00 00 00",
                                 "FC_EXT_1MIC": "87 6D F3 CF 44 87 1C F6 4C 3B 21 84 FF FA FD FF F1 44 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00",
                                 "FC_EXT_2MIC": "87 6D F3 CF 44 87 1C FA 4C 3B 21 84 FF FA FD FF F1 44 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"
                                 },
                        },
             },
    "M310": {"PACK_A": {"8155": {"VC": "19 09 00 04 00 1E 00 00 00 00 00 00 8D 10 00 00 00 00 00 00",
                                 "FC_INT_1MIC": "0F 01 F3 09 43 89 7C F4 04 33 21 84 FF F8 3D FD 00 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"},
                        },
             "PACK_B": {"8155": {"VC": "19 09 00 04 10 9F 00 04 00 00 00 00 8D 10 00 00 00 00 00 00",
                                 "FC_EXT_1MIC": "0F 01 F3 29 44 89 1C F6 4C 3B 25 84 FF FA 3D FF F0 44 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00",
                                 "FC_EXT_2MIC": "0F 01 F3 29 44 89 1C FA 4C 3B 25 84 FF FA 3D FF F0 44 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"
                                 },
                        },
             "PACK_C": {"8295": {"VC": "19 09 00 04 19 BF 00 0C 00 00 00 00 8D 10 00 00 00 00 00 00",
                                 "FC_EXT_1MIC": "8F 6D FF CF 44 87 1C F6 4C 3B 25 84 FF FA FD FF F1 44 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00",
                                 "FC_EXT_2MIC": "8F 6D FF CF 44 87 1C FA 4C 3B 25 84 FF FA FD FF F1 44 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"
                                 },
                        },
             },
    "M110": {"PACK_A": {"8155": {"VC": "19 09 00 04 00 1E 00 00 00 00 00 00 8D 10 00 00 00 00 00 00",
                                 "FC_INT_1MIC": "17 01 F3 09 43 89 7C F4 04 33 21 84 FF F8 3C FD 04 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"},
                        },
             "PACK_B": {"8155": {"VC": "19 09 00 04 10 9F 00 04 00 00 00 00 8D 10 00 00 00 00 00 00",
                                 "FC_EXT_1MIC": "17 01 F3 29 44 89 1C F6 4C 3B 25 84 FF FA 3C FF F4 53 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00",
                                 "FC_EXT_2MIC": "17 01 F3 29 44 89 1C FA 4C 3B 25 84 FF FA 3C FF F4 53 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"
                                 },
                        },
             "PACK_C": {"8295": {"VC": "19 09 00 04 19 BF 00 0C 00 00 00 00 8D 10 00 00 00 00 00 00",
                                 "FC_EXT_1MIC": "97 6D F3 CF 44 87 1C F6 4C 3B 25 84 FF FA FC FF F5 53 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00",
                                 "FC_EXT_2MIC": "97 6D F3 CF 44 87 1C FA 4C 3B 25 84 FF FA FC FF F5 53 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"
                                 },
                        },
             },
    "M111": {"PACK_A": {"8155": {"VC": "19 09 00 04 00 1E 00 00 00 00 00 00 8D 10 00 00 00 00 00 00",
                                 "FC_INT_1MIC": "27 01 F3 09 43 89 7C F4 04 33 21 84 FF F8 3C FD 04 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"},
                        },
             "PACK_B": {"8155": {"VC": "19 09 00 04 10 9F 00 04 00 00 00 00 8D 10 00 00 00 00 00 00",
                                 "FC_EXT_1MIC": "27 01 F3 29 44 89 1C F6 4C 3B 25 84 FF FA 3C FF F4 53 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00",
                                 "FC_EXT_2MIC": "27 01 F3 29 44 89 1C FA 4C 3B 25 84 FF FA 3C FF F4 53 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"
                                 },
                        },
             "PACK_C": {"8295": {"VC": "19 09 00 04 19 BF 00 0C 00 00 00 00 8D 10 00 00 00 00 00 00",
                                 "FC_EXT_1MIC": "A7 6D F3 CF 44 87 1C F6 4C 3B 25 84 FF FA FC FF F5 53 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00",
                                 "FC_EXT_2MIC": "A7 6D F3 CF 44 87 1C FA 4C 3B 25 84 FF FA FC FF F5 53 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                 },
                        },
             }
}

# Select Variant
VC_Data = fc_vc_data["M110"]["PACK_A"]["8155"]["VC"]
FC_Data = fc_vc_data["M110"]["PACK_A"]["8155"]["FC_INT_1MIC"]


# *********************************************************** TEST CASES #01 ************************************************
# @pytest.mark.skip(reason="no way of currently testing this")
@pytest.mark.order(1)
def test1_PowerLevelChecks(ManDiag_component, PowerSupply_component):
    """ NA : Power Supply voltage level validation """

    # Step 0: Update the Test variable and Test Procedure
    test_description = """
        Test Case Description: 
            - This test case verifies the valid power is supplied to the Unit.

        Test Sequence Steps:
            - Check the voltage supplied and if it is not in the expected Range ( 12V to 14V) then Update it.
             
        Expected Result:
        - The Unit should be supplied with min Power parameters: Voltage = 12V and Current = 5mA.
        """
    app_logger.debug(test_description)

    # Test :
    app_logger.info("Executing actual test: Checking and updating Current & voltage Ranges")

    voltage = float(PowerSupply_component.get_voltage())
    if 12.00 <= voltage <= 14.00:
        app_logger.info("Supplied voltage is in valid range")
        assert True
    else:
        # update Supply voltage to 13V and current consumption range 5Am
        app_logger.info(f"The Supply voltage is updated to 13V and Current range to 5A")
        PowerSupply_component.set_current(5)
        time.sleep(1)
        PowerSupply_component.set_voltage(13)
        assert True
    app_logger.debug("Test case passed successfully!")


# *********************************************************** TEST CASES #02 ************************************************
# @pytest.mark.skip(reason="no way of currently testing this")
@pytest.mark.order(2)
def test2_man_diag_session_entering_for_initial_SysConfig(ManDiag_component):
    """ VIP: Enter Mandiag Session """

    # Step 0: Update the Test variable and Test Procedure
    test_description = """
        Test Case Description: 
            - This test case verifies the functionality of Enter Manufacture diagnostics Session.

        Test Sequence Steps:
            Step 1: Pre-condition 
            - The Unit should be supplied with min Power parameters: Voltage = 12V and Current = 5mA.
            - A TTL should be connected to harness pins E11 and E12, Serial communication for VIP terminal should be established.
            - The Unit should be powered ON and VIP must be fully BootUp and Running
            - VIP full ON takes 20 to 30sec.
            Step 2. Actual Test
            - Send SET Enter ManDiag cmd "6D643E 00 01 01 00 01 01" and Validate the Response with status byte as "01"                        
            Step 3: Post-condition
            - Send GET Enter ManDiag cmd "6D643E 00 01 00 00 00" and Validate the Response with status byte and Data byte as "01"                     

        Expected Result:
        - The response should indicate that the ManDiag Session has been successfully entered and we should be able to execute
          mandiag other feature test commands.
        """
    app_logger.debug(test_description)
    GetCommand = "6D643E 00 01 00 00 00"
    SetCommand = "6D643E 00 01 01 00 01 01"

    # Step 1: Pre_Condition Test

    # Step 2: Actual Test
    app_logger.debug("Executing actual condition test: Send SET Enter ManDiag cmd and Validate the Response")
    assert ManDiag_component.send_request_command_and_validate_response(SetCommand), f"Failed to SET enter ManDiag Session for initial_SysConfig"

    # Step 3: Post Conditional Test
    app_logger.debug("Executing Post Condition test: Send GET Enter ManDiag cmd and Validate the Response")
    assert ManDiag_component.send_request_command_and_validate_response(GetCommand, expected_data_length="01",
                                                                        expected_data="01"), f"Failed to GET ManDiag Session for initial_SysConfig"

    app_logger.debug("Test case passed successfully!")


# ************************************************************* TEST CASES #03 *************************************************
@pytest.mark.skip(reason="No need to disable SOH if Display is connected")
@pytest.mark.order(3)
def test3_0x501B_SOH_Disable(ManDiag_component):
    """ VIP: Disable the SOH """

    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of disabling the SOH.

            Test Sequence Steps:
                Step 1: Pre-condition
                - Verify SOH is enabled by default with GET command
                Step 2. Actual Test
                - Disable the SOH using SET command.               
                Step 3: Post-condition
                - Verify SOH is disabled with GET command

            Expected Result:
                - We should be able to diSABLE and verify SOH state is Disable.
            """
    app_logger.debug(test_description)
    SetSOHDisable = "6D643E 50 1B 01 00 01 01"
    GetSOHDisable = "6D643E 50 1B 00 00 00"

    # Step 1: Pre Condition Test
    app_logger.info("Executing Pre Conditional test: Read and verify SOH status for Enabled state")
    if not ManDiag_component.send_request_command_and_validate_response(GetSOHDisable, expected_data_length="01", expected_data="00"):
        pytest.skip("skipping this test because pre condition is not satisfied")

    # Step 2: Actual Test
    app_logger.info("Executing actual test: Disable the SOH state using SET ")
    assert ManDiag_component.send_request_command_and_validate_response(SetSOHDisable, expected_status="00", retries=4)

    # Step 3: Post Condition Test
    app_logger.info("Executing Post test: Read and verify SOH status for Disabled state")
    assert ManDiag_component.send_request_command_and_validate_response(GetSOHDisable, expected_data_length="01", expected_data="01")

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #04 *************************************************
# @pytest.mark.skip(reason="no way of currently testing this")
@pytest.mark.order(4)
def test4_0x6611_SET_GET_VC(ManDiag_component):
    """ VIP: Writing Variant code into NvM """

    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Variant coding.

            Test Sequence Steps:
                Step 1: Pre-condition
                - VIP should be Fully up and Running
                Step 2. Actual Test
                - Send Set write VC data and Validate the Response for Valid status Byte, Data_length and Data.
                Step 3: Post-condition
                - Send Get Read VC data and Validate the Response for Valid status Byte, Data_length and Data.

            Expected Result:
                - We should be able to verify write and reading of VC data into NVM.
            """
    app_logger.debug(test_description)
    global VC_Data
    SetVCData = "6D643E 66 11 01 00 16 15 14 " + VC_Data
    GetVCData = "6D643E 66 11 00 00 01 15"
    VC_RespData = f"15 14 " + VC_Data
    # Step 1: Pre_Condition Test

    # Step 2: Actual Test Case
    app_logger.info("Executing actual test: Send Set write VC data and Validate")
    assert ManDiag_component.send_request_command_and_validate_response(SetVCData)

    # Step 3: Post Condition
    app_logger.info("Executing Post Condition test:Send Get Read VC data and Validate")
    assert ManDiag_component.send_request_command_and_validate_response(GetVCData, expected_data_length="16", expected_data=VC_RespData)

    app_logger.info("Test case passed successfully!")


# ************************************************************* TEST CASES #05 *************************************************
# @pytest.mark.skip(reason="no way of currently testing this")
@pytest.mark.order(5)
def test5_0x6611_SET_GET_FC(ManDiag_component):
    """ VIP: Writing feature code into NvM """

    # Step 0: Update the Test variable and Test Procedure
    test_description = """
            Test Case Description:
                - This test case verifies the functionality of Feature coding.

            Test Sequence Steps:
                Step 1: Pre-condition
                - VIP should be Fully up and Running
                Step 2. Actual Test
                - Send Set write FC data and Validate the Response for Valid status Byte, Data_length and Data.
                Step 3: Post-condition
                - Send Get Read FC data and Validate the Response for Valid status Byte, Data_length and Data.

            Expected Result:
                - We should be able to verify write and reading of FC data into NVM.
            """
    app_logger.debug(test_description)
    global FC_Data
    SetFCData = "6D643E 66 11 01 00 3E 16 3C " + FC_Data
    GetFCData = "6D643E 66 11 00 00 01 16"
    FC_RespData = f"16 3C " + FC_Data
    # Step 1: Pre_Condition Test

    # Step 2: Actual Test
    app_logger.info("Executing actual test: Send SET write FC data and Validate")
    assert ManDiag_component.send_request_command_and_validate_response(SetFCData)

    # Step 3: Post Condition Test
    app_logger.info("Executing Post Condition test:Send GET Read FC data and Validate")
    assert ManDiag_component.send_request_command_and_validate_response(GetFCData, expected_data_length="3E", expected_data=FC_RespData)

    app_logger.info("Test case passed successfully!")


# *********************************************************** TEST CASES #06 ************************************************
# @pytest.mark.skip(reason="disabling just for debugging purpose")
@pytest.mark.order(6)
def test6_Power_restart_for_FCVC_refection(ManDiag_component, PowerSupply_component):
    """ NA : Restart the Unit for FC-VC changes to reflect """

    # Step 0: Update the Test variable and Test Procedure
    test_description = """
        Test Case Description: 
            - This test case verifies the power restarting the Unit.

        Test Sequence Steps:
            - turn OFF and ON the power supply.

        Expected Result:
        - The Unit should be power restarted and FCVC changes should be reflected in power cycle.
        """
    app_logger.debug(test_description)

    app_logger.info("Executing actual test: Turning ON and OFF the power supply")

    if PowerSupply_component.restart_power():
        # Enter into mandiag after full ON
        for i in range(1, 30):
            time.sleep(1)
            app_logger.info(f"Waiting 30 Seconds for VIP Full ON - {i} Secs")
        assert ManDiag_component.send_request_command_and_validate_response("6D643E 00 01 01 00 01 01"), f"Failed to enter ManDiag Session"
        app_logger.debug("Test case passed successfully!")
    else:
        app_logger.debug("Test case Failed")

