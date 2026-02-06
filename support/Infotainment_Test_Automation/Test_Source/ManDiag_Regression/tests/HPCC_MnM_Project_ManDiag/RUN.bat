echo off
cd ../..

pytest -s -v tests\MIB4_Project_ManDiag --html=out\reports\MIB4_Mandiag_All_feature_testing_report.html

PAUSE
