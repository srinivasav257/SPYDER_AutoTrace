from __future__ import annotations

from datetime import datetime
import os
import time
from dataclasses import dataclass
from typing import Optional, Tuple, Union
from infotest.core.scpi_instruments import ScpiInstrument
from infotest.core.trace_logger import app_logger


class Keysight34465A(ScpiInstrument):
    """
    Minimal helpers for Keysight 34465A TrueVolt DMM.
    """

    # --- Basic configuration ---
    def conf_curr_dc(self, rng: Union[float, str] = 3, nplc: float = 0.1):
        """Configure DC current (fixed range or 'AUTO')."""
        self.write(f"CONF:CURR:DC {rng}")
        self.write(f"CURR:DC:NPLC {nplc}")

    def conf_volt_dc(self, rng: Union[float, str] = 10, nplc: float = 0.1):
        """Configure DC voltage (fixed range or 'AUTO')."""
        self.write(f"CONF:VOLT:DC {rng}")
        self.write(f"VOLT:DC:NPLC {nplc}")

    # --- One-shot reads ---
    def read_value(self) -> float:
        """Single reading using currently configured function/range."""
        return float(self.query("READ?"))

    def measure_curr_dc(self, rng: Union[float, str] = "AUTO", resolution: Union[float, str] = "DEF") -> float:
        return float(self.query(f"MEAS:CURR:DC? {rng},{resolution}"))

    def measure_volt_dc(self, rng: Union[float, str] = "AUTO", resolution: Union[float, str] = "DEF") -> float:
        return float(self.query(f"MEAS:VOLT:DC? {rng},{resolution}"))

    # --- Convenience logger ---
    def log_current(self, duration_s: float = 10.0, period_s: float = 0.5, nplc: float = 0.1, filepath: Optional[str] = None):
        """
        Log DC current for a fixed duration. Prints live values; optional CSV (if filename given).
        """
        self.conf_curr_dc(3, nplc=nplc)

        writer = None
        f = None
        if filepath:
            import csv
            # Ensure parent directory exists
            os.makedirs(os.path.dirname(filepath), exist_ok=True)
            # Create log file with timestamp
            log_filename = os.path.join(filepath, f"SleeCurrent_values_log_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv")
            f = open(log_filename, "w", newline="")
            writer = csv.writer(f)
            writer.writerow(["timestamp", "amps", "microamps", "range_A"])

        t0 = time.time()
        try:
            while time.time() - t0 < duration_s:
                v, rng = self.read_value(), 3.0
                ts = time.strftime("%Y-%m-%d %H:%M:%S")
                app_logger.info(f"{ts}  {v:.6f} A  ({v * 1e6:.1f} µA)  [range={rng}]")
                if writer:
                    writer.writerow([ts, f"{v:.9f}", f"{v * 1e6:.3f}", rng])
                time.sleep(period_s)
        finally:
            if f:
                f.close()
            # Leave panel running & local
            self.safe_release()


# Example usage (optional):
if __name__ == "__main__":
    VISA_ADDR = "USB0::0x2A8D::0x0301::MY57503803::0::INSTR"
    with Keysight34465A(VISA_ADDR) as dmm:
        print("IDN:", dmm.idn())
        dmm.conf_curr_dc(3, nplc=0.1)
        print("I =", dmm.read_value(), "A")
        dmm.log_current(duration_s=5)
