echo off
cd ../../../..

pytest -s -v tests/SEM2.5_Project_ManDiag/Sample_Tests/0x1F_AnalogTuner --html=out\reports\0x1F_AnalogTuner_report.html

PAUSE
