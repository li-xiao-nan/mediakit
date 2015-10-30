#include "ThreadGlobalData.h"

namespace timer{

ThreadGlobalData* threadGlobalData(){
	if(!gThreadGlobalData){
		gThreadGlobalData = new ThreadGlobalData();
	}else{
		return gThreadGlobalData;
	}
}
} // namespace timer
