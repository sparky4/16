set o=1
rem	echo %1
set q=%@EVAL[%@LEN[%1] - 4]
SET w=%@INSTR[0,%q,%1]
echo %w
goto new
:oooo
set o=%@eval[%o + 1]
rem	echo %o
:new
set y="%w.z%o"
htget http://4ch.mooo.com/16/%w.zip.00%o >> %y
if %@FILESIZE[%y,b] == 65536 goto OOOO
