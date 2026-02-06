echo off
cd ../../../..

pytest -s -v tests/SEM2.5_Project_ManDiag/Sample_Tests/0X0A_PowerGroup --html=out\reports\0X0A_PowerGroup_report.html

PAUSE
