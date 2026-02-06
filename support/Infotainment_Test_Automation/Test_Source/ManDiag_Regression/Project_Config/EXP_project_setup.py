"""
project_setup.py – Project-specific setup for MIB4 Porsche ManDiag
-----------------------------------------------------------------
Contains:
✔ Device fixtures
✔ Project metadata variables
✔ PreConditional_check for SW/HW versions
✔ Injection into pytest-html metadata
"""

import time
import pytest
from configparser import ConfigParser
from infotest.core.trace_logger import app_logger, print_owl_symbol
from infotest.devices.mandiag import Mandiag
from infotest.devices.power_supply import TenmaPowerSupply
from infotest.devices.io_controller import IOController
from infotest.devices.Keysight_34465A_DMM import Keysight34465A
from infotest.utilities.util_man_diag import hex_to_ascii_from_n
from pytest_metadata.plugin import metadata_key

# Global metadata placeholders
SilverBox_HW_Version = "Not able to Read"
SW_Build_Version = "Not able to Read"
IOC_SoftwareVersion = "Not able to Read"
CPA_SoftwareVersion = "Not able to Read"
CPV_SoftwareVersion = "Not able to Read"
CPR_SoftwareVersion = "Not able to Read"


@pytest.fixture(scope="session")
def config():
    """Load configuration from config.ini"""
    cfg = ConfigParser()
    cfg.read("config.ini")
    return cfg


@pytest.fixture(scope="session")
def ManDiag_component(config):
    """Diagnostics (Mandiag) device"""
    port = config.get("Ports", "vip", fallback="/dev/ttyUSB0")
    device = Mandiag(port)
    PreConditional_check(device)
    yield device
    device.close()


@pytest.fixture(scope="session")
def PowerSupply_TENMA(config):
    """TENMA Power Supply"""
    port = config.get("Ports", "tenma", fallback="/dev/ttyUSB1")
    device = TenmaPowerSupply(port)
    yield device
    device.close()


@pytest.fixture(scope="session")
def IO_controller(config):
    """Arduino IO Controller"""
    port = config.get("Ports", "io_controller", fallback="/dev/ttyUSB2")
    device = IOController(port)
    yield device
    device.close()


@pytest.fixture(scope="session")
def Keysight_34465A_DMM(config):
    """Keysight 34465A DMM"""
    port = config.get("Ports", "VISA_ADDR")
    device = Keysight34465A(port)
    yield device
    device.close()


def PreConditional_check(diag):
    """Read SW & HW version details into metadata & logs"""
    app_logger.info("------------ PreConditional_check Start ------------")

    commands = {"SW_Build_Version": "version"}

    # Dictionary to store all version results
    version_info = {key: "Not able to Read" for key in commands if key != "Enter_man_diag_session_cmd_SET"}

    for name, command in commands.items():
        time.sleep(0.5)

        # Special handling for SW Build Version
        if name == "SW_Build_Version":
            response_string = diag.serial_write_data_and_await_for_response(command, pattern="Build ID/UBN")
            if response_string:
                version_info[name] = response_string.rsplit(":", 1)[1].strip()
                app_logger.info(f"{name}: {version_info[name]}")
            else:
                app_logger.error(f"Failed to get {name}")
            continue

    app_logger.info("------------ PreConditional_check Completed ---------")

    # Inject into pytest-html metadata
    diag_meta = getattr(diag, "serial_conn", None)
    diag_port = diag_meta.port if diag_meta else "Unknown Port"

    pytest.config.stash[metadata_key].update({
        **version_info,  # Unpack dictionary into metadata
        "Diag Port": diag_port,
    })
