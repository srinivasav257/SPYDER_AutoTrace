# ---------- USB auto-discovery helpers ----------
from infotest.core.scpi_instruments import ScpiInstrument
from infotest.devices.Keysight_34465A_DMM import Keysight34465A


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
