#include <iostream>
#include "Timer.h"
#include "ThreadTimersManager.h"
using namespace std;
using namespace timer;

int main(int argc, char **argv){
	ThreadTimersManager collection;
	
	TimerBase t1,t2,t3,t4;
	t1.start(100, 0);
	t2.start(10,0);
	t3.start(3,0);
	t4.start(5,0);
	
	collection.timeHeap()->push(&t1);
	collection.timeHeap()->push(&t2);
	collection.timeHeap()->push(&t3);
	collection.timeHeap()->push(&t4);
	
	while(!collection.timeHeap()->empty()){
		cout<<collection.timeHeap()->top()->nextFireInterval()<<std::endl;
		collection.timeHeap()->pop();
	}	return 0;
}
