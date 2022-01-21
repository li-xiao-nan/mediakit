#ifndef ThreadGlobalData_h
#define ThreadGlobalData_h

#include "ThreadTimersManager.h"

namespace timer{
	class ThreadTimersManager;
class ThreadGlobalData{
public:
	ThreadTimersManager*	threadTimersManager(){ return &_threadTimersManager;}	
private:
	ThreadTimersManager _threadTimersManager;
};

static ThreadGlobalData *gThreadGlobalData;

ThreadGlobalData* threadGlobalData();

} // namespace timer

#endif
