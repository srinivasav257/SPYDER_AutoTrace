"""
=====================================================================================================================
File Name   : trace_logger.py
Description : Centralized logger class ensuring consistent logging across the entire framework.
Author      : Srinivasa V (Seenu)
Created On  : 2025-01-01

Copyright (C) 2025 APTIV - MnM HPCC Project
======================================================================================================================
"""

import logging
import os
from datetime import datetime
from logging.handlers import RotatingFileHandler
from configparser import ConfigParser
from colorama import Fore, Style, init as colorama_init

# Initialize colorama (cross-platform color support)
colorama_init(autoreset=True)

# -------------------------------------------------------------------------------------------------
# 🔧 Configuration
# -------------------------------------------------------------------------------------------------
config = ConfigParser()
config_path = "config.ini" if os.path.exists("config.ini") else "config/config.ini"
config.read(config_path)

# Default fallback values
LOG_DIR = config.get("Logging", "log_directory", fallback="out/logs")
LOG_LEVEL = config.get("Logging", "log_level", fallback="DEBUG").upper()
CONSOLE_LEVEL = config.get("Logging", "console_level", fallback="INFO").upper()
LOG_FILE_MAX_MB = int(config.get("Logging", "max_file_size_mb", fallback="5"))
LOG_FILE_BACKUP = int(config.get("Logging", "backup_count", fallback="3"))


# -------------------------------------------------------------------------------------------------
# 🎨 Colored Console Formatter
# -------------------------------------------------------------------------------------------------
class ColoredFormatter(logging.Formatter):
    """Custom formatter to add colors to console logs."""

    COLORS = {
        logging.DEBUG: Fore.WHITE,
        logging.INFO: Fore.GREEN,
        logging.WARNING: Fore.YELLOW,
        logging.ERROR: Fore.RED,
        logging.CRITICAL: Fore.MAGENTA + Style.BRIGHT,
    }

    def format(self, record):
        color = self.COLORS.get(record.levelno, "")
        base_msg = super().format(record)
        return f"{color}{base_msg}{Style.RESET_ALL}"


# -------------------------------------------------------------------------------------------------
# 🧱 Logger Class
# -------------------------------------------------------------------------------------------------
class Logger:
    """Singleton Logger for consistent logging across all modules."""
    _instance = None

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super(Logger, cls).__new__(cls)
            cls._instance._setup_logger()
        return cls._instance

    def _setup_logger(self):
        os.makedirs(LOG_DIR, exist_ok=True)

        # File name with timestamp
        log_filename = os.path.join(LOG_DIR, f"log_{datetime.now():%Y%m%d_%H%M%S}.log")

        # --- Handlers ---
        file_handler = RotatingFileHandler(
            log_filename, maxBytes=LOG_FILE_MAX_MB * 1024 * 1024, backupCount=LOG_FILE_BACKUP
        )
        console_handler = logging.StreamHandler()

        # --- Formatters ---
        file_format = logging.Formatter(
            "[%(asctime)s] [%(filename)25s:%(lineno)3d] %(levelname)8s >> %(message)s",
            datefmt="%m-%d-%y %H:%M:%S"
        )
        console_format = ColoredFormatter(
            "\r\n%(asctime)s | %(levelname)-5s | %(message)s",
            datefmt="%H:%M:%S"
        )

        file_handler.setFormatter(file_format)
        console_handler.setFormatter(console_format)

        # --- Set Levels ---
        file_handler.setLevel(getattr(logging, LOG_LEVEL, logging.DEBUG))
        console_handler.setLevel(getattr(logging, CONSOLE_LEVEL, logging.INFO))

        # --- Main Logger ---
        self.logger = logging.getLogger("InfotainmentLogger")
        self.logger.setLevel(logging.DEBUG)

        # 🧹 Remove old handlers to prevent duplicates when reloaded
        if self.logger.hasHandlers():
            self.logger.handlers.clear()

        self.logger.addHandler(file_handler)
        self.logger.addHandler(console_handler)


# -------------------------------------------------------------------------------------------------
# 🧩 Utility Functions
# -------------------------------------------------------------------------------------------------
def console_log(level, message):
    """
    Print colored message only to console (useful for quick feedback without logging to file).
    Example: console_log(logging.INFO, "Running EXP test suite")
    """
    temp_logger = logging.getLogger("ConsoleLogger")
    temp_logger.propagate = False
    temp_logger.setLevel(logging.DEBUG)

    handler = logging.StreamHandler()
    handler.setFormatter(ColoredFormatter("%(asctime)s | %(levelname)-5s | %(message)s", datefmt="%H:%M:%S"))
    temp_logger.addHandler(handler)

    temp_logger.log(level, message)
    temp_logger.removeHandler(handler)


def print_owl_symbol():
    """Fun Easter egg 🦉"""
    owl_symbol = r"""
         ___
        (o,o)
        { " }
        --"--
    """
    app_logger.critical(owl_symbol)


# -------------------------------------------------------------------------------------------------
# 🌍 Global Logger Instance
# -------------------------------------------------------------------------------------------------
app_logger = Logger().logger


# -------------------------------------------------------------------------------------------------
# 🧪 Self-test
# -------------------------------------------------------------------------------------------------
if __name__ == "__main__":
    app_logger.info("Framework initialized successfully")
    app_logger.debug("Debug message for internal trace")
    app_logger.warning("Low voltage detected (simulation)")
    app_logger.error("Command failed - device timeout")
    app_logger.critical("Critical: System halted!")
    print_owl_symbol()

"""
Revision History:
|---------------------------------------------------------------------------------------|
| Version | Date       | Author            | Changes                                    |
|---------|------------|-------------------|--------------------------------------------|
| 1.0     | 2025-01-01 | Srinivas V        | Initial release                            |

"""
