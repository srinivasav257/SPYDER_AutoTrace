echo off
cd ../../../..

pytest -s -v tests/SEM2.5_Project_ManDiag/Sample_Tests/0x51_DTC --html=out\reports\0x51_DTC_report.html

PAUSE
