//#define MEMORY_TEST_ENABLE

#ifdef MEMORY_TEST_ENABLE
//Memory leaks
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//
#endif

#define DEBUGMODE

#include "Application.h"

//To show console comment out line below
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

int main()
{
	Application app;
	app.Run();

#ifdef MEMORY_TEST_ENABLE
	//Memory leaks check
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}