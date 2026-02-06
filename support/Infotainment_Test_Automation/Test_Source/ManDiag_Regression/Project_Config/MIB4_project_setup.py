"""
MIB4_project_setup.py – Project-specific setup for MIB4 Porsche ManDiag
-----------------------------------------------------------------
Contains:
✔ Device fixtures (ManDiag, TENMA, IO, Keysight)
✔ Project metadata variables
✔ Exception-safe setup for hardware devices
✔ PreConditional_check for SW/HW versions
✔ Injection into pytest-html metadata
"""

import time
import pytest
from configparser import ConfigParser

from infotest.core.can_base import CANBase
from infotest.core.trace_logger import app_logger, print_owl_symbol
from infotest.devices.mandiag import Mandiag
from infotest.devices.power_supply import TenmaPowerSupply
from infotest.devices.io_controller import IOController
from infotest.devices.Keysight_34465A_DMM import Keysight34465A
from infotest.utilities.util_man_diag import hex_to_ascii_from_n
from infotest.core.exception_handler import SerialConnectionError
from infotest.devices.mandiag_MOL import Vector_CAN_Interface

# ------------------------------------------------------------------------------------------------
# Global metadata placeholders
# ------------------------------------------------------------------------------------------------
version_info = {"SilverBox_HW_Version": "Not able to Read",
                "SW_Build_Version": "Not able to Read",
                "IOC_SoftwareVersion": "Not able to Read",
                "CPA_SoftwareVersion": "Not able to Read",
                "CPV_SoftwareVersion": "Not able to Read",
                "CPR_SoftwareVersion": "Not able to Read"}

# ------------------------------------------------------------------------------------------------
# Fixtures
# ------------------------------------------------------------------------------------------------
@pytest.fixture(scope="session")
def config():
    """Load configuration from config.ini"""
    cfg = ConfigParser()
    cfg.read("config.ini")
    return cfg


@pytest.fixture(scope="session")
def ManDiag_component(config):
    """
    Diagnostics (Mandiag) device.
    If this fails, stop the entire session immediately — it's the foundation for all tests.
    """
    device = ""
    port = config.get("Ports", "vip", fallback="/dev/ttyUSB0")
    try:
        app_logger.info(f"Initializing ManDiag connection on {port}")
        device = Mandiag(port)
        # Step 1: System connectivity check
        System_Check(device)

        # Step 2: Read system info
        Read_System_Info(device)

        yield device
    except SerialConnectionError as e:
        app_logger.critical(f"FATAL: Unable to connect to Mandiag on {port} — {e}")
        pytest.exit(f"[FATAL] Cannot proceed: Mandiag not detected on {port}\nReason: {e}")
    finally:
        try:
            device.close()
        except Exception:
            pass

@pytest.fixture(scope="session")
def CAN_Interface(config):
    """
    Sets up and provides the MIB4 CAN Interface (VN1630A/VN1640A) for tests.
    Uses Vector hardware interface via python-can backend.
    """
    channel = config.getint("CAN", "channel", fallback=0)
    bitrate = config.getint("CAN", "bitrate", fallback=500000)
    interface = config.get("CAN", "interface", fallback="vector")

    app_logger.info(f"Initializing CAN Interface (interface={interface}, channel={channel}, bitrate={bitrate})")
    can_device = CANBase(channel=channel, bitrate=bitrate, interface=interface)

    yield can_device

    can_device.close()
    app_logger.info("Closed CAN Interface after test session.")

@pytest.fixture(scope="session")
def PowerSupply_TENMA(config):
    """TENMA Power Supply — skipped if unavailable"""
    port = config.get("Ports", "tenma", fallback="/dev/ttyUSB1")
    try:
        app_logger.info(f"Initializing TENMA Power Supply on {port}")
        device = TenmaPowerSupply(port)
        yield device
    except SerialConnectionError as e:
        pytest.skip(f"[SKIPPED] PowerSupply_TENMA not available at {port} — {e}")
    finally:
        try:
            device.close()
        except Exception:
            pass


@pytest.fixture(scope="session")
def IO_controller(config):
    """Arduino IO Controller — skipped if unavailable"""
    port = config.get("Ports", "io_controller", fallback="/dev/ttyUSB2")
    try:
        app_logger.info(f"Initializing IO Controller on {port}")
        device = IOController(port)
        yield device
    except SerialConnectionError as e:
        pytest.skip(f"[SKIPPED] IO Controller not available at {port} — {e}")
    finally:
        try:
            device.close()
        except Exception:
            pass


@pytest.fixture(scope="session")
def Keysight_34465A_DMM(config):
    """Keysight 34465A DMM — skipped if unavailable"""
    port = config.get("Ports", "VISA_ADDR", fallback="")
    try:
        app_logger.info(f"Initializing Keysight 34465A DMM on {port or 'AUTO-DISCOVERY'}")
        device = Keysight34465A(port)
        yield device
    except SerialConnectionError as e:
        pytest.skip(f"[SKIPPED] Keysight DMM not available at {port or 'Auto-Detect'} — {e}")
    finally:
        try:
            device.close()
        except Exception:
            pass

@pytest.fixture(scope="session")
def vector_CANCH1(config):
    """vector VN1640A — skipped if unavailable"""
    interface = config.get("Ports", "interface", fallback="vector")
    channel = config.get("Ports", "channel1", fallback="0")
    bitrate = config.get("Ports", "bitrate", fallback="500000")

    try:
        app_logger.info(f"Initializing vector CAN on channel:{channel}")
        device = Vector_CAN_Interface(channel, bitrate=bitrate, interface=interface)
        yield device
    except SerialConnectionError as e:
        pytest.skip(f"[SKIPPED] vector CAN channel not available at channel:{channel} — {e}")
    finally:
        try:
            device.close()
        except Exception:
            pass

@pytest.fixture(scope="session")
def vector_CANCH2(config):
    """vector VN1640A — skipped if unavailable"""
    interface = config.get("Ports", "interface", fallback="vector")
    channel = config.get("Ports", "channel2", fallback="1")
    bitrate = config.get("Ports", "bitrate", fallback="500000")

    try:
        app_logger.info(f"Initializing vector CAN on channel:{channel}")
        device = Vector_CAN_Interface(channel, bitrate=bitrate, interface=interface)
        yield device
    except SerialConnectionError as e:
        pytest.skip(f"[SKIPPED] vector CAN channel not available at channel:{channel} — {e}")
    finally:
        try:
            device.close()
        except Exception:
            pass

# ---------------------------------------------------------------------------------------------------------------
# System Checks
# ----------------------------------------------------------------------------------------------------------------
def System_Check(diag):
    """
    Perform a basic system connectivity check before running tests.
    Sends Enter ManDiag command and verifies response.
    If it fails, stop all test execution early.
    """
    app_logger.info("-------------------------------------- System_Check -----------------------------------------")

    enter_cmd = "6D643E 00 01 01 00 01 01"
    response = diag.send_request_command_and_get_response(enter_cmd)

    if not response or response.get("Status") != "01":
        app_logger.critical("System Check FAILED — Device not responding or not powered ON!")
        print_owl_symbol()
        pytest.exit(" Device not Running or No ManDiag response — stopping test execution.", returncode=1)
    else:
        app_logger.info(" System check passed — Device is ON and responding.")

    app_logger.info("-------------------------------------- System_Check Completed -------------------------------")


# -----------------------------------------------------------------------------------------------------------------
# Read System info
# -----------------------------------------------------------------------------------------------------------------
def Read_System_Info(diag):
    """Read SW & HW version details into metadata & logs"""
    app_logger.warning("----------------------------- PreConditional_check Start --------------------------------")

    commands = {
        "SW_Build_Version": "version",
        "SilverBox_HW_Version": "6D643E 78 01 00 00 01 02",
        "IOC_SoftwareVersion": "6D643E 64 01 00 00 01 02",
        "CPA_SoftwareVersion": "6D643E 64 01 00 00 01 01",
        "CPV_SoftwareVersion": "6D643E 64 01 00 00 01 05",
        "CPR_SoftwareVersion": "6D643E 64 01 00 00 01 06",
    }

    global version_info
    for name, command in commands.items():
        time.sleep(0.5)
        try:
            if name == "SW_Build_Version":
                response_string = diag.serial_write_data_and_await_for_response(command, pattern="Build ID/UBN")
                if not response_string:
                    app_logger.warning(f"Failed to get {name}")
                    continue

                version_info[name] = response_string.rsplit(":", 1)[1].strip()
                app_logger.info(f"{name}: {version_info[name]}")
            else:
                response = diag.send_request_command_and_get_response(command)
                if not response or response.get("Status") != "01":
                    app_logger.warning(f"Failed to read {name}")
                    continue

                version_data = hex_to_ascii_from_n(response.get("Databytes", ""), 1)
                version_info[name] = version_data
                app_logger.info(f"{name}: {version_data}")

        except Exception as e:
            app_logger.error(f"Exception while processing {name}: {e}")

    app_logger.warning("--------------------------- PreConditional_check Completed ----------------------------")
