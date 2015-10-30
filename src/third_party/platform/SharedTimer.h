#ifndef SharedTimer_h
#define SharedTimer_h

namespace timer{
	
	class SharedTimer {
	public:
	    SharedTimer() { }
	    virtual ~SharedTimer() { }

	    virtual void setFiredInterval(double) = 0;
	    virtual void setFiredFunction(void (*func)(void)) = 0;
	    virtual void stop() = 0;
	};

	void setSharedTimerFiredFunction(void (*)());
	void setSharedTimerFiredInterval(double);
	void stopSharedTimer();

	class MainThreadSharedTimer : public SharedTimer {
	public:
		virtual void setFiredFunction(void (*func)(void)){
			setSharedTimerFiredFunction(func);
		}

		virtual void setFiredInterval(double interval){
			setSharedTimerFiredInterval(interval);
		}

		virtual void stop(){
			stopSharedTimer();
		}
	};


} // namespace timer


#endif // SharedTimer_h
