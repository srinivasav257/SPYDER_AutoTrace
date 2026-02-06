echo off
cd ../../../..

pytest -s -v tests/SEM2.5_Project_ManDiag/Sample_Tests/0x64_SWID --html=out\reports\0x64_SWID_report.html

PAUSE
