/*
 * Copyright (C) 2013, 2014 Mark Li (lixiaonan06@163.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "Timer.h"
#include "ClockTime.h"
#include "CurrentTime.h"
#include "ThreadGlobalData.h"
#include <iostream>
#include <stdio.h>
namespace timer{
TimerBase::TimerBase():
	_heap(NULL){
//	if(!_heap){
//		_heap = threadGlobalData()->threadTimersManager()->timersHeap();
	//}
}

void TimerBase::fire(){
	//do nothing
}
void TimerBase::start(double nextInterval, double repeatInterval){

	_nextInterval  = nextInterval;
	_repeatInterval  = repeatInterval;
	setFireTime(nextInterval+monotonicallyIncreasingTime());
}

void TimerBase::setFireTime(double fireTime){
	if(!fireTime) return;
	_fireTime = fireTime;
	static unsigned timerAddOrder;
	_addOrder = timerAddOrder++;
	pushSelf();
	printf("_fireTime=%lf\n", _fireTime);
	threadGlobalData()->threadTimersManager()->updateSharedTimer();
}
void TimerBase::startOneShot(double nextInterval){ start(nextInterval, 0); }
double TimerBase::nextFireTime() 	  { return _fireTime; }
double TimerBase::repeatInterval()   { return _repeatInterval; }
unsigned TimerBase::order() { return _addOrder;}
void TimerBase::pushSelf(){
	if(!_heap){
		_heap = &threadGlobalData()->threadTimersManager()->timersHeap();
		_tmp = (int*)&threadGlobalData()->threadTimersManager()->timersHeap();
		_heap = (ThreadTimersManager::PriorityHeap*)_tmp;
	}	
	_heap->push(this);
}

void TimerBase::popSelf(){
	if(!_heap || _heap->empty()) return ;
	_heap->pop();
}
}//namespace timer
