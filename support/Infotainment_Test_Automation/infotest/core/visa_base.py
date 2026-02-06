"""
visa_base.py
------------
Generic VISA session base class for instrument control using PyVISA.
Provides resource discovery, open/close, context management, and I/O helpers.
"""

from __future__ import annotations
from typing import Optional, Tuple, List
import pyvisa as visa


class VisaSession:
    """
    Generic VISA session wrapper.
    - Manages ResourceManager and a single opened resource
    - Supports context-manager usage (with ... as ...)
    - Provides read/write/query helpers
    - Includes USB resource discovery helpers
    """

    def __init__(
        self,
        resource: Optional[str] = None,
        timeout_ms: int = 5000,
        backend: Optional[str] = None,   # '@ni', '@py', etc., or None to auto-detect
    ):
        self.backend = backend
        self.rm: visa.ResourceManager = visa.ResourceManager(self.backend) if self.backend else visa.ResourceManager()
        self.inst: Optional[visa.resources.MessageBasedResource] = None
        self.timeout_ms = int(timeout_ms)
        self.resource = resource
        self.opened = False

        if resource:
            self.open(resource)

    # ---- Open/Close ----

    def open(self, resource: str):
        """Open a VISA session to the provided resource string."""
        self.close()
        self.resource = resource
        self.inst = self.rm.open_resource(resource, timeout=self.timeout_ms)
        self.opened = True

    def close(self):
        """Close any open VISA session; safe to call multiple times."""
        try:
            if self.inst is not None:
                try:
                    self.inst.close()
                finally:
                    self.inst = None
            self.opened = False
        except Exception:
            self.inst = None
            self.opened = False

    # ---- Context manager ----

    def __enter__(self):
        if not self.opened and self.resource:
            self.open(self.resource)
        return self

    def __exit__(self, exc_type, exc, tb):
        self.close()
        return False

    def __del__(self):
        try:
            self.close()
        except Exception:
            pass

    # ---- Discovery helpers ----

    def list_resources(self, pattern: str = "?*") -> Tuple[str, ...]:
        """List any VISA resources (optionally using a resource expression)."""
        return tuple(self.rm.list_resources(pattern))

    def list_usb_instruments(self) -> Tuple[str, ...]:
        """List USB instruments (USBTMC/LXI-USB) resources only."""
        return tuple(self.rm.list_resources("USB?*INSTR"))

    @staticmethod
    def _match_usb(resource: str, vid: Optional[str], pid: Optional[str], serial: Optional[str]) -> bool:
        r = resource.upper()
        ok = True
        if vid:
            ok &= (vid.upper() in r)
        if pid:
            ok &= (pid.upper() in r)
        if serial:
            ok &= (serial.upper() in r)
        return ok

    def find_first_usb(
        self,
        vendor_id: Optional[str] = None,
        product_id: Optional[str] = None,
        serial: Optional[str] = None
    ) -> Optional[str]:
        """Return the first USB resource string matching vid/pid/serial (or None)."""
        for res in self.list_usb_instruments():
            if self._match_usb(res, vendor_id, product_id, serial):
                return res
        return None

    # ---- I/O helpers ----

    def write(self, cmd: str):
        """Send a command string (SCPI or otherwise)."""
        if not self.inst:
            raise RuntimeError("Session not open")
        return self.inst.write(cmd)

    def read(self) -> str:
        """Read a line of ASCII response."""
        if not self.inst:
            raise RuntimeError("Session not open")
        return self.inst.read()

    def query(self, cmd: str) -> str:
        """Write then read (ASCII)."""
        if not self.inst:
            raise RuntimeError("Session not open")
        return self.inst.query(cmd)

    def query_float(self, cmd: str) -> float:
        return float(self.query(cmd))

    def query_int(self, cmd: str) -> int:
        return int(float(self.query(cmd)))

    def query_ascii_values(self, cmd: str) -> List[float]:
        """Query comma-separated ASCII values; returns list of floats."""
        if not self.inst:
            raise RuntimeError("Session not open")
        return self.inst.query_ascii_values(cmd)
