#include "Timer.h"
#include "CurrentTime.h"
#include <iostream>
#include <unistd.h>
#include <stdio.h>
using namespace std;
using namespace timer;
class TimerTest{
public:
	TimerTest();
        void fireFunc(Timer<TimerTest> *timer);
	void addTimers(double t);
private:
	Timer<TimerTest> _timer;
}; 

TimerTest::TimerTest():
	_timer(this,&TimerTest::fireFunc){
//do nothing
}

void TimerTest::fireFunc(Timer<TimerTest> *timer){
	printf("ok: you fire me in %lf \n", monotonicallyIncreasingTime());
}

void TimerTest::addTimers(double t){
	
	_timer.start(t,1);
}


int main(int argc, char **argv){
	TimerTest t1, t2;
	t1.addTimers(1);
//	t2.addTimers(2);
	while(1){

	}
	printf("sleep 30 second is time to\n");
}
