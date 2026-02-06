echo off
cd ../..

pytest -s -v tests\SEM2.5_Project_ManDiag --html=out\reports\Mandiag_All_feature_testing_report.html

PAUSE
