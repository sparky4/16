#!/bin/bash
if [ -f "BCEXMM.MAP" ]; then
	mv BCEXMM.EXE bcexmm.ex0
	mv bcexmm.ex0 bcexmm.exe
	mv BCEXMM.MAP bcexmm.meh
fi
