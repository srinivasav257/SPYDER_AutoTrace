echo off
cd ../../../..

pytest -s -v tests/SEM2.5_Project_ManDiag/Sample_Tests/0x14_Audio --html=out\reports\0x14_Audio_report.html

PAUSE
