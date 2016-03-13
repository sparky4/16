@echo off
for /f %%A in ('dir ^| find "*.z?*"') do set cnt=%%A
echo File count = %cnt%
