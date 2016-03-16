@echo off
set file="%1"
set maxbytesize=65536

FOR /F "usebackq" %%A IN ('%file%') DO set size=%%~zA

if %size% LSS %maxbytesize% (
    echo.File is ^< %maxbytesize% bytes
) ELSE (
    echo.File is ^>= %maxbytesize% bytes
)

for /f %%A in ('dir ^| find "*.z?*"') do set cnt=%%A
echo File count = %cnt%
