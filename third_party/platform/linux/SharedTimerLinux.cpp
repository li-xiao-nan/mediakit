
#include "../SharedTimer.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
namespace timer {

	static void (*func)(void);

	void signalCB(int id){
		func();
	}

	void setSharedTimerFiredFunction(void (*f)()){
		printf("OK: you have set the fired function\n");
		func = f;
	}	

	void setSharedTimerFiredInterval(double interval){
		signal(SIGALRM, signalCB);
		int  usecond = (interval-static_cast<int>(interval))*1000000;
		struct itimerval val;
		val.it_value.tv_sec = static_cast<int>(interval);
		val.it_value.tv_usec = usecond;
		val.it_interval.tv_sec = 0;
		val.it_interval.tv_usec = 0;
		setitimer(ITIMER_REAL, &val, NULL);
	}
	void stopSharedTimer(){
		struct itimerval val;
		val.it_value.tv_sec = 0;
		val.it_value.tv_usec = 0;
		val.it_interval = val.it_value;
		setitimer(ITIMER_REAL, &val, NULL);
	}	
} // namespace timer
