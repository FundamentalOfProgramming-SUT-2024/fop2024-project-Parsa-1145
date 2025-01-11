#include "Utilities/LinkedList.h"
#define KEY_MOUSE_MOVE 268435456
#define RELATIVE 1
#define ABSOLUTE 0
#define ALIGN_LEFT 1
#define ALIGN_CENTER 2
#define ALIGN_RIGHT 3
#define ALIGN_TOP 1
#define ALIGN_BOTTOM 3
#define VERTICAL_LAYOUT 1
#define WITH_PARENT 4

#define C_BG_GRAY0 10
#define C_BG_GRAY1 12
#define C_BG_BLACK 11


#define FOR(i, n) for(int i = 0 ; i < n ; i++)
#define startListIter(t, l) void** tmpPtr; while(tmpPtr){ t = tmpPtr[1];
#define endListIter }

#define PI 3.141592654

typedef  void(*voidCB)();