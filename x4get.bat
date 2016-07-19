@echo off
iff exist %* then
	del %*
endiff
setlocal
set url="4ch.mooo.com/16"
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
rem	 echo y=%y
	if %o lt 100 (
		if %o lt 10 (
			set z=00%o
		) else (
			set z=0%o
		) endif
	) else if %o gt 99 (
		set z=%o
	) endif

	htget http://%url/%1.zip.%z > %y
rem  	pause
	iff NOT "%@FILESIZE[%y,b]" == "0" then
		type %y >> %w.zip
		iff "%@FILESIZE[%y,b]" == "65536" then
rem  				echo o=%o
			goto oooo
		endiff
		iff NOT "%#" == "0" then
			shift /1
			iff exist %w.zip then
				unzip %w.zip
				del %w.z*
			endiff
			goto loop
		else
			del %#
		endiff
	else
		del %y
		echo Error null zip file wwww
	endiff
	else
		rem del %y.z*
		rem need to add a cleaner wwww
		echo Error wwww invalid file name.
	endiff
rem  echo y="%y"
rem  echo q=%q
rem  echo o=%o
rem  echo w=%w
rem  echo %%1=%1
endlocal
