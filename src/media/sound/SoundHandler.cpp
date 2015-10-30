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

#include "SoundHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

namespace MediaCore {

SoundHandler::SoundHandler():
		_paused(false),
		_volume(100)
{}
void SoundHandler::plugInputStream(auto_ptr<InputStream> in){
	if(!_inputStreams.insert(in.release()).second){
		cout<<__FILE__<<"->:_inputStreams container still has a pointer"<<endl;
		exit(0);
	}
	//Increment number of sound start request for the testing framework
	++_soundsStarted;
}

void SoundHandler::unplugAllInputStream(){
	for(InputStreams::iterator it=_inputStreams.begin(),
			itEnd = _inputStreams.end();
			it!=itEnd; ++it){
		delete *it;
	}
	_inputStreams.clear();
}
void SoundHandler::unplugCompletedInputStreams(){
	for(InputStreams::iterator it=_inputStreams.begin(),
			end=_inputStreams.end(); it!=end;){
		InputStream *is = *it;
		if(is->eof()){
			InputStreams::iterator it2 = it;
			++it2;
			InputStreams::size_type erased = _inputStreams.erase(is);
			if(erased !=1){
				cout<<__FILE__<<"-->earse the complete inputStream failed"<<endl;
				exit(0);
			}
			it=it2;
			delete is;
		}//if(if->eof())
		else{
			it++;
		}

	}//for.....
}
void SoundHandler::unplugInputStream(InputStream *id){
	InputStreams::iterator it2 = _inputStreams.find(id);
	if(it2 == _inputStreams.end()){
		cout<<"not find the the id InputStream"<<endl;
		return ;
	}
	_inputStreams.erase(it2);
	delete id;
}
InputStream* SoundHandler::attachAuxStreamer(auxStreamPtr ptr, void *uData){
		assert(uData);
		assert(ptr);
		auto_ptr<InputStream> newStreamer(new AuxStream(ptr,uData));
		InputStream *ret = newStreamer.get();
		plugInputStream(newStreamer);
		return ret;
}
void SoundHandler::fetchSamples(int16_t *to, uint16_t nSamples){
	if(isPaused()){
		return ;
	}
	float  finalVolumeFact = getFinalVolume()/100.0;
	std::fill(to, to+nSamples, 0);
	//call SnailPlayer or Sound audio callbacks
	if(!_inputStreams.empty()){
		int16_t *buf = new int16_t[nSamples];

		for(InputStreams::iterator it=_inputStreams.begin(),
				end=_inputStreams.end();
		it!=end; ++it){
			InputStream *is= *it;
			uint16_t wrote = is->fetchSamples(buf,nSamples);
			if(wrote < nSamples){
				std::fill(buf+wrote,buf+nSamples,0);
			}
			mix(to, buf, nSamples, finalVolumeFact);
		}//for(InputStreams::iterator it=_....
		unplugCompletedInputStreams();
		delete[] (buf);
	}//if(!_inputStreams.empty())
}
SoundHandler::~SoundHandler(){
	unplugAllInputStream();
}

} // namespace
