echo off
cd ../../../..

pytest -s -v tests/SEM2.5_Project_ManDiag/Sample_Tests/0x3D_BT --html=out\reports\0x3D_BT_report.html

PAUSE
