"""
scpi_instruments.py
-------------------
Derived SCPI convenience classes built on top of visa_base.VisaSession.

- ScpiInstrument: adds SCPI utilities (IDN/reset/errors, display/local/remote,
  trigger control, beeper, autozero) and a safe_release() to avoid leaving
  the front panel in a 'remote/hold' state.

- Keysight34465A: example subclass for TrueVolt 34465A DMM with helpers
  for DC current/voltage config, one-shot reads, and optional software
  autorange + logger.
"""

from __future__ import annotations
import time
from dataclasses import dataclass
from typing import Optional, Tuple, Union

from infotest.core.visa_base import VisaSession


class ScpiInstrument(VisaSession):
    """SCPI utility layer on top of VisaSession."""

    # --- Identification / Reset / Errors ---
    def idn(self) -> str:
        return self.query("*IDN?").strip()

    def reset(self):
        self.write("*RST")

    def clear_status(self):
        self.write("*CLS")

    def next_error(self) -> str:
        return self.query("SYST:ERR?").strip()

    def clear_errors(self, max_reads: int = 20) -> str:
        last = ""
        for _ in range(max_reads):
            last = self.next_error()
            if last.startswith("0"):
                break
        return last

    # --- Local/Remote & Display ---
    def to_local(self):
        self.write("SYST:LOC")

    def to_remote(self):
        self.write("SYST:REM")

    def display_on(self, on: bool = True):
        self.write(f"DISP:STAT {'ON' if on else 'OFF'}")

    def display_text(self, text: str = ""):
        if text:
            self.write(f'DISP:TEXT "{text}"')
        else:
            self.write("DISP:TEXT:CLE")

    # --- Trigger / Acquisition ---
    def init_cont(self, on: bool = True):
        self.write(f"INIT:CONT {'ON' if on else 'OFF'}")

    def abort(self):
        self.write("ABORt")

    def trig_source(self, source: str = "IMM"):
        self.write(f"TRIG:SOUR {source}")

    def sample_count(self, count: int = 1):
        self.write(f"SAMP:COUN {int(count)}")

    # --- Beeper / Autozero ---
    def beep(self):
        self.write("SYST:BEEP")

    def beep_state(self, on: bool = True):
        self.write(f"SYST:BEEP:STAT {'ON' if on else 'OFF'}")

    def autozero(self, on: bool = True):
        self.write(f"SYST:AZER {'ON' if on else 'OFF'}")

    # --- Safe release (avoid 'stuck screen') ---
    def safe_release(self):
        """
        Leave front panel live and return Local control.
        Use at the end of scripts or in context manager exit.
        """
        try:
            self.abort()
            self.clear_status()
            self.trig_source("IMM")
            self.sample_count(1)
            self.init_cont(True)
            self.display_on(True)
            self.clear_errors()
            self.to_local()
        except Exception:
            pass

    # Context manager uses safe_release() on exit
    def __exit__(self, exc_type, exc, tb):
        try:
            self.safe_release()
        finally:
            super().__exit__(exc_type, exc, tb)
        return False


# --------------------------
# Example concrete class: Keysight 34465A
# --------------------------

@dataclass
class RangeRule:
    rng: float
    down_if_lt: float
    up_if_gt: float

class Keysight34465A(ScpiInstrument):
    """
    Minimal helpers for Keysight 34465A TrueVolt DMM.
    """

    RANGE_TABLE = [
        RangeRule(0.0001, 0.0,      90e-6),   # 100 µA
        RangeRule(0.001,  250e-6,   0.9e-3),  # 1 mA
        RangeRule(0.01,   1e-3,     0.009),   # 10 mA
        RangeRule(3.0,    0.1,      2.8),     # 3 A
    ]

    # --- Basic configuration ---
    def conf_curr_dc(self, rng: Union[float,str] = 3, nplc: float = 0.1, autozero: bool = False):
        """Configure DC current (fixed range or 'AUTO')."""
        self.write(f"CONF:CURR:DC {rng}")
        self.write(f"CURR:DC:NPLC {nplc}")
        self.autozero(autozero)

    def conf_volt_dc(self, rng: Union[float,str] = 10, nplc: float = 0.1, autozero: bool = False):
        """Configure DC voltage (fixed range or 'AUTO')."""
        self.write(f"CONF:VOLT:DC {rng}")
        self.write(f"VOLT:DC:NPLC {nplc}")
        self.autozero(autozero)

    # --- One-shot reads ---
    def read_value(self) -> float:
        """Single reading using currently configured function/range."""
        return float(self.query("READ?"))

    def measure_curr_dc(self, rng: Union[float,str] = "AUTO", resolution: Union[float,str] = "DEF") -> float:
        return float(self.query(f"MEAS:CURR:DC? {rng},{resolution}"))

    def measure_volt_dc(self, rng: Union[float,str] = "AUTO", resolution: Union[float,str] = "DEF") -> float:
        return float(self.query(f"MEAS:VOLT:DC? {rng},{resolution}"))

    # --- Software auto-range for DC current ---
    def enable_curr_autorange(self, nplc: float = 0.1, autozero: bool = False, min_switch_s: float = 0.5):
        """Initialize software autorange (starts at 3 A)."""
        self._auto_idx = 3
        self._last_switch = 0.0
        self._min_switch = float(min_switch_s)
        self.conf_curr_dc(self.RANGE_TABLE[self._auto_idx].rng, nplc, autozero)

    def _maybe_step(self, abs_val: float):
        now = time.time()
        if now - self._last_switch < self._min_switch:
            return
        idx = self._auto_idx
        rule = self.RANGE_TABLE[idx]
        if idx > 0 and abs_val < rule.down_if_lt:
            self._auto_idx -= 1
        elif idx < len(self.RANGE_TABLE) - 1 and abs_val > rule.up_if_gt:
            self._auto_idx += 1
        else:
            return
        new_rng = self.RANGE_TABLE[self._auto_idx].rng
        self.write(f"CURR:DC:RANG {new_rng}")
        self._last_switch = now

    def read_curr_auto(self) -> Tuple[float, float]:
        """Read current and auto-adjust range; returns (value_A, active_range_A)."""
        val = self.read_value()
        self._maybe_step(abs(val))
        return val, self.RANGE_TABLE[self._auto_idx].rng

    # --- Convenience logger ---
    def log_current(self, duration_s: float = 10.0, period_s: float = 0.5,
                    autorange: bool = False, nplc: float = 0.1, autozero: bool = False,
                    filename: Optional[str] = None):
        """
        Log DC current for a fixed duration. Prints live values; optional CSV (if filename given).
        """
        if autorange:
            self.enable_curr_autorange(nplc=nplc, autozero=autozero)
        else:
            self.conf_curr_dc(3, nplc=nplc, autozero=autozero)

        writer = None
        f = None
        if filename:
            import csv
            f = open(filename, "w", newline="")
            writer = csv.writer(f)
            writer.writerow(["timestamp", "amps", "microamps", "range_A"])

        t0 = time.time()
        try:
            while time.time() - t0 < duration_s:
                if autorange:
                    v, rng = self.read_curr_auto()
                else:
                    v, rng = self.read_value(), 3.0
                ts = time.strftime("%Y-%m-%d %H:%M:%S")
                print(f"{ts}  {v:.6f} A  ({v*1e6:.1f} µA)  [range={rng}]")
                if writer:
                    writer.writerow([ts, f"{v:.9f}", f"{v*1e6:.3f}", rng])
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
        # dmm.log_current(duration_s=5, autorange=True)


# ---------- USB auto-discovery helpers ----------
def discover_usb_resources(session: "ScpiInstrument", pattern: str = "USB?*INSTR"):
    """Return a tuple of VISA USB resources visible to this session's ResourceManager."""
    return session.list_resources(pattern)


def first_34465a_resource(session: "ScpiInstrument"):
    """
    Try each USB resource and return the first whose *IDN? contains '34465A'.
    Leaves no stray open sessions.
    """
    for res in session.list_usb_instruments():
        try:
            # Open a temporary session to probe IDN
            session.open(res)
            idn = session.idn()
            if "34465A" in idn.upper():
                return res
        except Exception:
            pass
    # close if last loop left it open
    session.close()
    return None


# Add a classmethod on Keysight34465A for convenience
def _add_open_first_on_class():
    def open_first(cls, timeout_ms: int = 5000):
        """
        Create a Keysight34465A connected to the first discovered 34465A on USB.
        Raises RuntimeError if none found.
        """
        tmp = cls(resource=None, timeout_ms=timeout_ms)
        res = first_34465a_resource(tmp)
        if not res:
            raise RuntimeError("No Keysight 34465A found on USB.")
        # Re-open as a proper instance bound to the discovered resource
        tmp.open(res)
        return tmp
    setattr(Keysight34465A, "open_first", classmethod(open_first))

_add_open_first_on_class()


# ---------- Example auto-discovery usage ----------
if __name__ == "__main__":
    try:
        dmm = Keysight34465A.open_first()
        print("Connected via:", dmm.resource)
        print("IDN:", dmm.idn())
        dmm.conf_curr_dc(3, nplc=0.1)
        print("I =", dmm.read_value(), "A")
        dmm.safe_release()
    except Exception as e:
        print("Auto-discovery failed:", e)


# ---------- Extended USB discovery ----------
def list_usb_table(session: "ScpiInstrument"):
    """
    Return a list of tuples (resource, idn_string) for each USB instrument found.
    """
    results = []
    for res in session.list_usb_instruments():
        try:
            session.open(res)
            idn = session.idn()
            results.append((res, idn))
        except Exception:
            results.append((res, "IDN? failed"))
        finally:
            session.close()
    return results


def discover(print_table: bool = False):
    """
    Discover all USB instruments and optionally print a neat table.
    Returns a list of (resource, idn) pairs.
    """
    tmp = ScpiInstrument(resource=None)
    results = list_usb_table(tmp)
    if print_table:
        print("USB Instruments:")
        for res, idn in results:
            print(f"  {res:<50} {idn}")
    return results


# Convenience classmethods on Keysight34465A

def _add_open_helpers():
    def open_by_serial(cls, serial: str, timeout_ms: int = 5000):
        """
        Open a Keysight34465A by matching a serial substring in VISA resource or IDN.
        """
        tmp = cls(resource=None, timeout_ms=timeout_ms)
        for res, idn in list_usb_table(tmp):
            if serial.upper() in res.upper() or serial.upper() in idn.upper():
                tmp.open(res)
                return tmp
        raise RuntimeError(f"No Keysight 34465A with serial '{serial}' found.")

    def open_by_vidpid(cls, vid: str = "0x2A8D", pid: str = "0x0301", timeout_ms: int = 5000):
        """
        Open the first Keysight 34465A matching a given vendor/product ID.
        Default VID=0x2A8D (Keysight), PID=0x0301 (34465A).
        """
        tmp = cls(resource=None, timeout_ms=timeout_ms)
        for res in tmp.list_usb_instruments():
            if vid.upper() in res.upper() and pid.upper() in res.upper():
                tmp.open(res)
                return tmp
        raise RuntimeError(f"No Keysight 34465A with VID={vid}, PID={pid} found.")

    setattr(Keysight34465A, "open_by_serial", classmethod(open_by_serial))
    setattr(Keysight34465A, "open_by_vidpid", classmethod(open_by_vidpid))

_add_open_helpers()


# ---------- Example extended discovery usage ----------
if __name__ == "__main__":
    discover(print_table=True)
    try:
        dmm = Keysight34465A.open_by_serial("MY57503803")
        print("Opened by serial:", dmm.resource)
        print("IDN:", dmm.idn())
        dmm.safe_release()
    except Exception as e:
        print("Open by serial failed:", e)


# ---------- Advanced discovery & openers ----------
def discover_usb(print_table: bool = True, probe_idn: bool = True):
    """
    Discover USB INSTR resources visible to VISA. Optionally probe *IDN?.
    Returns a list of dicts: [{"resource": ..., "idn": "..."}].
    """
    from visa_base import VisaSession
    sess = VisaSession()
    rows = []
    for res in sess.list_usb_instruments():
        info = {"resource": res, "idn": ""}
        if probe_idn:
            try:
                sess.open(res)
                info["idn"] = sess.query("*IDN?").strip()
            except Exception:
                info["idn"] = ""
            finally:
                sess.close()
        rows.append(info)

    if print_table:
        print("\nUSB VISA Instruments:")
        print("-" * 80)
        print(f"{'RESOURCE':45}  {'IDN'}")
        print("-" * 80)
        for r in rows:
            idn = r['idn'] if r['idn'] else "(no response)"
            print(f"{r['resource']:45}  {idn}")
        print("-" * 80)
    return rows


def open_by_serial_34465a(serial: str, timeout_ms: int = 5000):
    """
    Return a connected Keysight34465A matching the given serial substring (case-insensitive).
    Raises RuntimeError if not found.
    """
    tmp = Keysight34465A(resource=None, timeout_ms=timeout_ms)
    serial_up = serial.upper()
    for res in tmp.list_usb_instruments():
        try:
            tmp.open(res)
            idn = tmp.idn().upper()
            if "34465A" in idn and serial_up in idn:
                # keep tmp open and return it
                return tmp
        except Exception:
            pass
    tmp.close()
    raise RuntimeError(f"No 34465A found with serial containing '{serial}'.")


def open_by_vid_pid_34465a(vendor_id: str = "0x2A8D", product_id: str = "0x0301", timeout_ms: int = 5000):
    """
    Return a connected Keysight34465A matching VID/PID (strings like '0x2A8D').
    If multiple match, returns the first that identifies as 34465A via *IDN?.
    """
    tmp = Keysight34465A(resource=None, timeout_ms=timeout_ms)
    vid_up, pid_up = vendor_id.upper(), product_id.upper()
    for res in tmp.list_usb_instruments():
        r = res.upper()
        if vid_up in r and pid_up in r:
            try:
                tmp.open(res)
                if "34465A" in tmp.idn().upper():
                    return tmp
            except Exception:
                pass
    tmp.close()
    raise RuntimeError(f"No 34465A found with VID={vendor_id} PID={product_id}.")


# Bind as classmethods for nicer API
def _bind_more_openers():
    setattr(Keysight34465A, "open_by_serial", classmethod(lambda cls, serial, timeout_ms=5000: open_by_serial_34465a(serial, timeout_ms)))
    setattr(Keysight34465A, "open_by_vid_pid", classmethod(lambda cls, vendor_id="0x2A8D", product_id="0x0301", timeout_ms=5000: open_by_vid_pid_34465a(vendor_id, product_id, timeout_ms)))

_bind_more_openers()


# ---------- Example discovery usage ----------
if __name__ == "__main__":
    # Pretty table of USB instruments (+IDN)
    discover_usb(print_table=True, probe_idn=True)
