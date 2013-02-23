// tests.cpp : Defines the entry point for the console application.
//

#include "../libJSONpp/config.h"
#include "stdlib.h"

#include "../libJSONpp/value.h"

#ifndef JSON_WINDOWS
#include <stdio.h>
#endif

#define INCLUDE_TESTS_IN_THIS_MODULE
#include "tests1.h"

void mypause();

using namespace std;


int main(int argc, char * argv[])
{
#ifdef JSON_WINDOWS
	_CrtMemState s1, s2,s3;
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
#endif

	printf("JSON tests\n\n");
	int ntests = sizeof(tests)/sizeof(tests[0]);
	int cgrp=1;
	int oldgrp=0;
	int errs=0;
	printf("Running %i tests:\n\n",ntests);
	for (int i=0;i<ntests;++i){
		if (oldgrp != cgrp){
			printf(" Group %i\n---------\n",cgrp);
			oldgrp=cgrp;
		}
		printf("     %-3i [%s]:\t",i,tests[i].name);
#ifdef JSON_WINDOWS
		_CrtMemCheckpoint( &s1 );
#endif
		bool rv=tests[i].fn();
#ifdef JSON_WINDOWS
		_CrtMemCheckpoint( &s2 );
#endif
		if (rv){
			printf("OK\n");
		}else{
			printf("FAILED (%s)\n",tests[i].desc);
			if (tests[i].fatal){
				++errs;
			}
		}
#ifdef JSON_WINDOWS		
		if ( _CrtMemDifference( &s3, &s1, &s2) ) {
			printf("Memory Leak Detected!  Memory usage has risen from %ul to %ul\n",s1.lTotalCount,s2.lTotalCount);
			_CrtDumpMemoryLeaks();
			mypause();
			return 0;
		}
#endif
		if (tests[i].eog)++cgrp;
	}
	printf("\nTests complete.  %i fatal errors out of %i tests\n",errs,ntests);
		
	fflush(stdout);

	printf("\n");
#ifdef JSON_WINDOWS
	_CrtDumpMemoryLeaks();
#endif
	mypause();
}

void mypause(){
#ifdef JSON_WINDOWS
	system("pause");
#else

#endif
}




