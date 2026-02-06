echo off
cd ../../..

pytest -s -v tests/MIB4_Porsche_Project_ManDiag/0x0A_Power --html=out\reports\0x0A_all_Power_features_report.html

PAUSE
