echo off
cd ../../..

pytest -s -v tests/SEM2.5_Project_ManDiag/Sample_Tests --html=out\reports\Sample_Tests_report.html

PAUSE
