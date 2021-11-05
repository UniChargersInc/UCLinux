#ifndef MKR_NEW_DBG_H_INCLUDED
#define MKR_NEW_DBG_H_INCLUDED

/*
http://stackoverflow.com/questions/3202520/c-memory-leak-testing-with-crtdumpmemoryleaks-does-not-output-line-numb
int* p = new (_NORMAL_BLOCK, __FILE__, __LINE__) int(5);

You can wrap this in a conditionally defined macro, e.g.
#ifdef _DEBUG
#define DEBUG_NEW_PLACEMENT (_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_NEW_PLACEMENT
#endif

int* p = new DEBUG_NEW_PLACEMENT int(5);

While you do see people defining a macro new to completely hide this form client code, I do not personally recommend it as it breaks anything already intentionally using placement new and you have to make sure that any headers using placement new (such as many standard headers) are included before any header redefining new. This can make it easy to let some inline uses of new in header files slip through without being 'adjusted'.

*/
#if WIN32
#define MEMORY_LEAK_EXPLORE 1


#if MEMORY_LEAK_EXPLORE
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define DEBUG_NEW_PLACEMENT (_NORMAL_BLOCK, __FILE__, __LINE__)

#define MKR_NEW new DEBUG_NEW_PLACEMENT
#else
#define DEBUG_NEW_PLACEMENT
#define MKR_NEW new DEBUG_NEW_PLACEMENT
#endif
/*
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG
*/

#else
#define DEBUG_NEW_PLACEMENT
#define MKR_NEW new DEBUG_NEW_PLACEMENT
#endif

#endif