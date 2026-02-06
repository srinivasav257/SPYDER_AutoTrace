# My Project 
This is the Python test script for MnM-HPCC ITS mandiag feature test Automation

# File Structure of This Script.

infotainment_diag_framework/
│── framework_modules/
│   ├── core/                     # Core reusable components
│   │   ├── trace_logger.py
│   │   ├── exception_handler.py
│   │   ├── base_serial.py
│   │   ├── visa_base.py
│   │   ├── scpi_instruments.py
│   │
│   ├── devices/                  # Hardware-specific drivers
│   │   ├── mandiag.py
│   │   ├── io_controller.py
│   │   ├── power_supply.py
│   │   ├── Keysight_34465A_DMM.py
│   │
│   ├── utilities/                # Helper utilities
│   │   ├── util_excel_report.py
│   │   ├── util_keysight_34465A.py
│   │   ├── util_man_diag.py
│
│── tests/                        # All test cases grouped by features
│   ├── session/
│   │   ├── test_0x0001_VIP_EnterExit_Mandiag_Session.py
│   ├── power/
│   │   ├── test_0x0A04_Sleep.py
│   │   ├── test_0x0A05_Unit_Restart.py
│   │   ├── test_0x0A06_Battery_Calibration.py
│   ├── can/
│   │   ├── test_0x3301_CAN_Dummy_Feature.py
│
│── Project_Configs/       # Config files for specific configuration
│   ├── EXP_project_setup.py
│   ├── HPCC_project_setup.py
│   ├── IHP_project_setup.py
│   ├── SEM25_project_setup.py
│   ├── MIB4_project_setup.py
│
├── config.ini
├── pytest.ini
│── conftest.py                   # Global pytest fixtures
│── requirements.txt              # Dependencies (pyvisa, pyserial, openpyxl, pytest-html, etc.)
│── README.md                     # Project overview & usage
│── out/                          # Auto-generated logs & reports
│   ├── logs/
│   ├── reports/

Future Proofing : Easy to extend if you add more devices (Ethernet, USB test devices).
                  CI/CD ready (pytest -v --html=out/reports/test_report.html).

# Pre Requirement: Install Python version ~3.1.0 above and Install all the package required to run this Script with below cmd
	pass this below cmd from root path of the project
	>> pip install -r requirements.txt
	FYI: " pip freeze > requirements.txt " can be used to generate requirements.txt file with all dependent module list 

# Importing Modules

from framework_modules.core.trace_logger import app_logger
from framework_modules.core.base_serial import BaseSerialDevice
from framework_modules.devices.mandiag import Mandiag
from framework_modules.devices.power_supply import TenmaPowerSupply
from framework_modules.devices.io_controller import IOController
from framework_modules.devices.Keysight_34465A_DMM import Keysight34465A
from framework_modules.utilities.util_excel_report import ExcelReport
from framework_modules.utilities.util_man_diag import parse_response

# How TO RUN The Test Script

Option 1: send below cmd from root folder of the project. (recommended for selective features test)
		  # this will run all the tests present in folder and generates single report
		  >> pytest -m <marker>      # checks for exact match to market string
          >> pytest -k <marker_str>  # checks for substring string match 

Option 2: send below cmd from root folder of the project, (recommended for all features test)
		  # this will run all the tests present in test folder and generates individual test report for each sub-folder
		  >> tests/SEM2.5_Project_ManDiag/RUN.bat
		  
Option 3: send below cmd from root folder of the project, 
		  # this will run only tests present in provided feature Group test folder and generates single test report
		  >> pytest folder_path
		     Ex: pytest tests\1_System_FeaturesGroup_0x00\

Option 4: send below cmd from root folder of the project, 
		  # this will run only tests present in provide file and generates single test report
		  >> pytest folder_path\file_name.py
		     Ex: pytest tests\1_System_FeaturesGroup_0x00\DR_MD_0x0004_OPERATING_SYSTEM_BOOT_STATUS.py
			 
Option 5: send below cmd from root folder of the project, 
		  # this will run only test function provided in the cmd and generates test report
		  >> pytest file_path\test_file_name.py::test_func_name 
		     Ex: pytest tests\test_audio\test_volume_control.py::test_decrease_volume

# Important points:

1. All the test HTML test reports and test logs will be saved under folder name "reports" , make sure the folder available before running the
   script to avoid possible errors or permission issues.
   
2. Ensure that all test functions are correctly named with the "test_" prefix and reside in files starting with "test_"
   
3. Explanation: Some useful Pytest decorators
   --------------------------------------------------------------------------------------------------------------------
    @pytest.mark.dependency(name="test_name"): This decorator assigns a name to the test case. The name is used to
                                                refer to this test when specifying dependencies.
    @pytest.mark.dependency(depends=["test_name"]): This decorator specifies that the test case depends on another
                                                   test case. If the depended-upon test case fails, the dependent test case is skipped.
    @pytest.mark.<marker_name>  : This will create marker to the test which can used for selective test execution
    @pytest.mark.run(order=<order_number>): This decorator specifies the order of the test case execution.
    @pytest.mark.parametrize(...) : This will help to when writing same tests with different input values.
    @pytest.fixture: fixture sets up and tears down the environment for the ManDiag session tests.
                    Setup: Initializes the session state to inactive before running each test.
                    Teardown: Resets the session state to inactive after each test to ensure no test affects another.

🔹 Steps to Add a New Project
--------------------------------------------------------------------------
1. Create a Project Setup File
Inside Project_Config/, copy an existing setup file and rename it:

Project_Config/
│── MIB4_project_setup.py
│── HPCC_project_setup.py
│── NewProject_project_setup.py   ← (create this)


Inside NewProject_project_setup.py, define:
Device fixtures (ManDiag_component, PowerSupply, IO_controller, etc.)

Mainly update these functions
# Step 1: System connectivity check
System_Check(device)
# Step 2: Read system info
Read_System_Info(device)

def Read_System_Info(diag):
    commands = {
        "Main ECU Software Version": "AA BB CC DD 01",
        "Gateway Software Version": "AA BB CC DD 02",
        "Cluster HW Version": "AA BB CC DD 03",
    }

2. Update config.ini
----------------------------------------------------------------------------------
Set the active project:

[Project]
name = NewProject

Adjust other device configs as needed:

[Ports]
vip = COM10
soc = COM5
tenma = COM11
io_controller = COM12
VISA_ADDR = USB0::0x2A8D::0x1301::MY12345678::INSTR

3. Update Confest to import your project setup file
----------------------------------------------------------------------------------
elif project_name == "NEWPRO":
    from Project_Config.NEWPRO_project_setup import *
    print("[INFO] Loaded project setup: NEWPRO")

4. Run Tests
-----------------------------------------------------------------------------------
Run pytest as usual — the framework will auto-load the correct project setup based on config.ini.

Note: whenever new marker is added to test case that should be registered in pytest.ini file

Extra:
----------------------------------------------------------
pytest -v --html=out/reports/report.html --self-contained-html

C:\MyPro\Python_Automation_ITS>pip freeze > requirements.txt
C:\MyPro\Python_Automation_ITS>sed -i 's/==.*//' requirements.txt
pip install -r requirements.txt