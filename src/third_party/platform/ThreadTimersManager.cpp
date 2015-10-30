#include "ClockTime.h"
#include "CurrentTime.h"
#include "SharedTimer.h"
#include "ThreadTimersManager.h"
#include "ThreadGlobalData.h"
#include "Timer.h"
#include <iostream>
#include <stdio.h>
using namespace std;
namespace timer{

const double maxSliceOfFireTimers = 0.05; //second
SharedTimer* mainThreadSharedTimer(){
	return new MainThreadSharedTimer();
}
bool ThreadTimersManager::LittleFunc::operator()(TimerBase *t1, TimerBase *t2) const {
	if(t1->nextFireTime() != t2->nextFireTime())
		return (t1->nextFireTime() >= t2->nextFireTime());
	return (t1->order() >= t2->order());
}

ThreadTimersManager::ThreadTimersManager(bool isMainThread):
	_pendingNextSharedTimerFireTime(0),
	_firingTimer(false),
	_sharedTimer(NULL){
		if(1/*isMainThread()*/){
			setSharedTimer(mainThreadSharedTimer());
		}
}

void ThreadTimersManager::setSharedTimer(SharedTimer *sharedTimer){
	if(_sharedTimer){
		_sharedTimer->setFiredFunction(0);
		_sharedTimer->setFiredInterval(0);
		_sharedTimer->stop();
		_sharedTimer = NULL;
	}

	_sharedTimer = sharedTimer;
	if(_sharedTimer){
		_sharedTimer->setFiredFunction(ThreadTimersManager::sharedTimerFired);
	updateSharedTimer();
	}
}
void ThreadTimersManager::updateSharedTimer(){
	if(!_sharedTimer) return ;

	if(_firingTimer || _timersHeap.empty()){
		_pendingNextSharedTimerFireTime = 0;
		_sharedTimer->stop();	
	}else{
		double nextFireTime = _timersHeap.top()->nextFireTime();
		//[mɒnə'tɒnɪklɪ]
		double currentMonotonicTime = monotonicallyIncreasingTime();
		if(_pendingNextSharedTimerFireTime){
			if(_pendingNextSharedTimerFireTime <= currentMonotonicTime && nextFireTime <= currentMonotonicTime)
				return ;
		}
		_pendingNextSharedTimerFireTime = nextFireTime;
		_sharedTimer->setFiredInterval(max(nextFireTime-currentMonotonicTime, 0.0));
	}
}
//static
void ThreadTimersManager::sharedTimerFired(){
		threadGlobalData()->threadTimersManager()->sharedTimerFiredInterval();
	}
void ThreadTimersManager::sharedTimerFiredInterval(){
	if(_firingTimer) return ;
	std::cout<<"now to fire the timers\n"<<std::endl;
	_firingTimer = true;
	_pendingNextSharedTimerFireTime = 0;
	
	double fireTime = monotonicallyIncreasingTime();
	printf("%s-->fireTime = %lf  \n", __FUNCTION__, fireTime);
	double timeToQuit = fireTime + maxSliceOfFireTimers;
	while(!_timersHeap.empty()&&_timersHeap.top()->nextFireTime()<=fireTime){
		TimerBase *timer = _timersHeap.top();
		_timersHeap.pop();
		timer->fire();
		double repeatInterval = timer->repeatInterval();
		timer->setFireTime(repeatInterval?fireTime+repeatInterval:0);
		printf("_timerHeas.size()=%d\n", _timersHeap.size());
		if(monotonicallyIncreasingTime()>=timeToQuit){
			printf("%s --> time to quit fire\n", __FUNCTION__);
			break;
		}
	}
	_firingTimer = false;	
	updateSharedTimer();
}
}//namepsace timer
