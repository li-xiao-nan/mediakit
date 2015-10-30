#include <iostream>
#include "Timer.h"
using namespace timer;
class test{
public:
	test();
	void testFunc();
	void run(Timer<test>*);
	void active();
private:
	Timer<test> _timer;
};

test::test():
   _timer(this,&test::run)
{
	}

void test::run(Timer<test>*){

	testFunc();
}

void test::active(){
	_timer.fired();
}

void test::testFunc(){

	std::cout<<"you see this,so you got what you want"<<std::endl;
}
int  main(int argc, char **argv){

	test t;
	t.active();
return 0;
}
