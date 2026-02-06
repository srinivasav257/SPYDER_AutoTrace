echo off
cd ../../../..

pytest -s -v tests/SEM2.5_Project_ManDiag/Sample_Tests/0x78_HWID --html=out\reports\0x78_HWID_report.html

PAUSE
