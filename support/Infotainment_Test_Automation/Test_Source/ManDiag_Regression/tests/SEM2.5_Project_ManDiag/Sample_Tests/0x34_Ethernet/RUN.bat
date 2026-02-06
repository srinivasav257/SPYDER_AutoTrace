echo off
cd ../../../..

pytest -s -v tests/SEM2.5_Project_ManDiag/Sample_Tests/0x34_Ethernet --html=out\reports\0x34_Ethernet_report.html

PAUSE
