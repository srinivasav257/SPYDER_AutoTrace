echo off
cd ../../../..

pytest -s -v tests/SEM2.5_Project_ManDiag/Sample_Tests/0x3C_WIFI --html=out\reports\0x3C_WIFI_report.html

PAUSE
