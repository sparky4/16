/*
	TestPat.hpp version 1.0
	by Robert Schmidt of Ztiff Zox Softwear 1993

	Declares the TestPatterns class which is further defined in
		TestPat.cpp.
*/

#ifndef _TestPat_HPP
#define _TestPat_HPP

#include <stdlib.h>
#include "RegTable.HPP"

class TestPatterns
	{
public:
	// testType declares the possible types of screen patterns to test your
	//	wonderful modes with.  Note that 'tests' is there just to provide a
	//	constant equaling the number of tests.
	enum testType
		{ autoDetect, testText16, testText8, test4x16, test1x256,
		  test4x256, tests };
private:
	static char *string[tests];
	testType testNo;
public:
	TestPatterns(testType t=autoDetect) { testNo = t; }
	testType operator++()	{ if (++testNo==tests) testNo=testType(0);
							  return testNo; }
	testType operator--()	{ if (--testNo<testType(0)) testNo=tests-1;
							  return testNo; }
	testType getTest()		{ return testNo; }
	void run(RegisterTable &);
	void tellTest();
	};

#endif