#!/bin/bash
if [ -f "BCEXMM.MAP" ]; then
	if [ -f "BCEXMM.EXE" ]; then
		mv BCEXMM.EXE bcexmm.ex0
		mv bcexmm.ex0 bcexmm.exe
	fi
	mv BCEXMM.MAP bcexmm.meh
fi
