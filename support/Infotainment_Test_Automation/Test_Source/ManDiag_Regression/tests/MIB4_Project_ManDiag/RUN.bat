echo off
cd ../..

pytest -s -v tests\MIB4_Project_ManDiag\0x0A_Power\test_0x0A04_Sleep.py --html=out\reports\test_0x0A04_Sleep_testing_report.html

PAUSE

