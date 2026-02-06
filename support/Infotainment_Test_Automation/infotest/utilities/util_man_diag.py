"""
=====================================================================================================================
File Name   : util_man_diag.py
Description : This File contains ManDiag test device class - This module sends diagnostic commands to the infotainment
              unit and processes responses.
Author      : Srinivasa V
Created On  : 2025-01-01

Copyright (C) 2025 APTIV - This file is part of the MnM HPCC project - Manufacture Diagnostics Component.
Manufacture Diagnostics Project is free software: you can redistribute it and/or modify it under the terms of Diagnostics
Department ASUX.

======================================================================================================================
"""

from infotest.core.trace_logger import app_logger

def parse_response(response):
    data_list = response.split()
    data_dict = {
        "Prefix": data_list[0],
        "GroupId": data_list[1],
        "TestId": data_list[2],
        "Operation": data_list[3],
        "Status": data_list[4],
        "DataLength": data_list[5],
    }

    # If Data length is not "00", extract Data bytes
    if data_list[5] != "00" or data_list[3] == "10" or data_list[3] == "11":
        data_dict["Databytes"] = " ".join(data_list[6:])  # Join remaining bytes as space-separated
    else:
        data_dict["Databytes"] = ""  # Keep empty string instead of None

    # Print dictionary
    app_logger.debug(f"Response Dictionary: {data_dict}")
    return data_dict

def hex_to_ascii_from_n(hex_string, n):
    if hex_string and len(hex_string) >= n:
        hex_string = hex_string.replace(' ', '')
        sliced_hex_string = hex_string[n * 2:]
        bytes_object = bytes.fromhex(sliced_hex_string)
        return bytes_object.decode('ascii')
    else:
        return " "

def get_hex_array(data):
    data_parts = data.split()
    integer_array = [int(part, 16) for part in data_parts]
    return integer_array

"""
Revision History:
|---------------------------------------------------------------------------------------|
| Version | Date       | Author            | Changes                                    |
|---------|------------|-------------------|--------------------------------------------|
| 1.0     | 2025-01-01 | Srinivas V        | Initial release                            |

"""
