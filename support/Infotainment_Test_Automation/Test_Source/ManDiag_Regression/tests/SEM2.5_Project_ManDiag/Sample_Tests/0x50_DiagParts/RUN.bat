echo off
cd ../../../..

pytest -s -v tests/SEM2.5_Project_ManDiag/Sample_Tests/0x50_DiagParts --html=out\reports\0x50_DiagParts_report.html

PAUSE
