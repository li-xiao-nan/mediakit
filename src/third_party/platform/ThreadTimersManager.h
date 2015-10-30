#ifndef ThreadTimersManager_h
#define ThreadTimersManager_h

#include <iostream>
#include <queue>
#include <vector>

using namespace std;
namespace timer{
class TimerBase;
class SharedTimer;
class ThreadTimersManager{
public:
	//function object for priority_queue sort action
	class LittleFunc{
		public:
		bool operator()(TimerBase *t1,TimerBase *t2) const ;
	};
	
	typedef priority_queue<TimerBase*, vector<TimerBase*>, LittleFunc> PriorityHeap;

	ThreadTimersManager(bool isMainThread=false);
	void setSharedTimer(SharedTimer *sharedTimer);
	void updateSharedTimer();
	PriorityHeap& timersHeap(){return _timersHeap;}
	static void sharedTimerFired();
	void sharedTimerFiredInterval();	
private:
	double _pendingNextSharedTimerFireTime;
	bool _firingTimer;	
	PriorityHeap  _timersHeap;
	SharedTimer *_sharedTimer;	
};
}//namespace timer
#endif
