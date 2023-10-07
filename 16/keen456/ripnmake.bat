@echo off

cd keen4-6\static

echo Trying to extract data from KEEN 4 ...
ck4patch ripck4.pat
echo.

echo Trying to extract data from KEEN 5 ...
ck5patch ripck5.pat
echo.

echo Trying to extract data from KEEN 6 ...
ck6patch ripck6.pat
echo.

echo Converting data files to .OBJ ...
call make.bat

cd ..\..