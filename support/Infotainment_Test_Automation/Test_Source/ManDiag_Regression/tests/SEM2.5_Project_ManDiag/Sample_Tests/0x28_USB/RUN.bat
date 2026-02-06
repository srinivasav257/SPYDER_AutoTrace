echo off
cd ../../../..

pytest -s -v tests/SEM2.5_Project_ManDiag/Sample_Tests/0x28_USB --html=out\reports\0x28_USB_report.html

PAUSE
