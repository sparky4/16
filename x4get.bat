@echo off
setlocal
rem for %@eval[%#+1] in (%*) do (
rem for %# in (1,1,%*) do (
:loop
	set /a o=0
:oooo
	set /a q=%@EVAL[%@LEN[%1] - 4]
	set w=%@INSTR[0,%q,%1]
	iff NOT "%w" == "" then
	set /a o=%o+1
	set y=%w.z%o
	echo y=%y
rem  	pause
	htget http://4ch.mooo.com/16/%w.zip.00%o >> %y
	iff "%@FILESIZE[%y,b]" == "65536" then
rem  		echo o=%o
		goto oooo
	endiff
	iff NOT "%#" == "0" then
		shift /1
		echo %1
		goto loop
	endiff
	endiff
rem  echo y="%y"
rem  echo q=%q
rem  echo o=%o
rem  echo w=%w
rem  echo %%1=%1
endlocal
