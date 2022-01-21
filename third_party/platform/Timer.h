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

#ifndef Timer_h
#define Timer_h
#include <iostream>
#include "ClockTime.h"
#include "ThreadTimersManager.h"
namespace timer{

class ThreadTimersManager;

class TimerBase{
public:
	TimerBase();
	virtual void fire();
	void start(double nextInteral,double repeatInterval);
	void startOneShot(double nextInterval);
	void setFireTime(double fireTime);
	double nextFireTime();
	double repeatInterval();
	unsigned order();
	void pushSelf();
	void popSelf();
protected:
	double _fireTime;
	double _nextInterval;
	double _repeatInterval;
	unsigned _addOrder;
	ThreadTimersManager::PriorityHeap *_heap;//this is a global Var
	int *_tmp;

};//class TimerBase


template<class T>
class Timer : public TimerBase{
	//MK_MAKE_NOCOPYABLE(TimerBase);
private:
	typedef void(T::* TimerFunc)(Timer<T>*);
	T* const _obj;
	TimerFunc _func;
public:
	Timer( T* const obj, TimerFunc func);
	virtual void fire();	
};

template<class T>
Timer<T>::Timer(T* const obj, Timer<T>::TimerFunc func):
	_obj(obj),
	_func(func){
	//do nothing
	}

template<class T>
void Timer<T>::fire(){
	(_obj->*_func)(this);	
}

}//namespace timer

#endif //#ifndef Timer_h
