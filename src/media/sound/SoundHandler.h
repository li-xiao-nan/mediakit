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

#ifndef SOUNDHANDLER_H_
#define SOUNDHANDLER_H_
#include "InputStream.h"
#include "AuxStream.h"
#include <set>
#include <iostream>
#include <memory>

using namespace std;

namespace MediaCore {

class SoundHandler{
public:
	~SoundHandler();
	SoundHandler();
    virtual void OpenAudio(int sample_rate, uint8_t channels) = 0;
	virtual void plugInputStream(auto_ptr<InputStream> in);
	virtual void unplugAllInputStream();
	virtual void unplugInputStream(InputStream *id);
	void unplugCompletedInputStreams();
	virtual InputStream* attachAuxStreamer(auxStreamPtr ptr, void *uData);
	virtual void fetchSamples(int16_t *to, uint16_t nSamples);
	virtual void mix(int16_t *outSamples, int16_t *inSamples,
			uint16_t nSamples, int volume) = 0;
	bool hasInputStreams() const{
		return !_inputStreams.empty();
	}
	//get/set play state functions
	bool isPaused(){return _paused;}
	int getFinalVolume(){return _volume;}
private:
	typedef set<InputStream*> InputStreams;

	//Sound input streams
	///Elements owned by this class
	InputStreams _inputStreams;
	//Special test-number .stores count of started sounds
	size_t _soundsStarted;
	//play statae fileds
	bool _paused;
	int _volume; //final output volume

};//class SoundHandler


} // namespace

#endif /* SOUNDHANDLER_H_ */
