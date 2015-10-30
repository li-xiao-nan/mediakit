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
#if 1

#include "SoundHandlerSDL.h"
#include <iostream>
#include <assert.h>
using namespace std;

namespace MediaCore {

SoundHandlerSDL::SoundHandlerSDL():
		_audioOpened(false){
	initAudio();
}
SoundHandlerSDL::~SoundHandlerSDL(){
	SDL_PauseAudio(1);
	SDL_CloseAudio();
}
void SoundHandlerSDL::initAudio(){
	closeAudio();
}
void SoundHandlerSDL::OpenAudio(int sample_rate, uint8_t channel_count){
	if(_audioOpened)
        closeAudio();

    _audioSpec.freq = sample_rate;//44100;

	_audioSpec.format = AUDIO_S16SYS;
	_audioSpec.channels = channel_count;
	_audioSpec.callback = SoundHandlerSDL::sdlAudioCB;
	_audioSpec.userdata = this;
	_audioSpec.samples = 1024;

	if(SDL_OpenAudio(&_audioSpec, NULL)<0){
		cout<<"open sdl audio device failed"<<endl;
		exit(0);
	}
	_audioOpened = true;
}
void SoundHandlerSDL::closeAudio(){
	SDL_CloseAudio();
	_audioOpened = false;
}
void SoundHandlerSDL::plugInputStream(auto_ptr<InputStream> in){
	SoundHandler::plugInputStream(in);

	SDL_PauseAudio(0);
}
void SoundHandlerSDL::sdlAudioCB(void *opaque, uint8_t *buffer, int bufferLength){
	if(bufferLength < 0){
		cout<<"Negative buffer length in sdlAudioCallback"<<endl;
		return ;
	}
	if(bufferLength == 0){
		cout<<"Zero buffer length in sdlAudioCallback"<<endl;
		return ;
	}
	uint16_t bufferLen = static_cast<uint16_t>(bufferLength);
	int16_t *samples = reinterpret_cast<int16_t*>(buffer);
	assert(!(bufferLen%4));
	uint16_t nSamples = bufferLen/2;

	SoundHandlerSDL *handler = static_cast<SoundHandlerSDL*>(opaque);
	handler->fetchSamples(samples,nSamples);
}

void SoundHandlerSDL::fetchSamples(int16_t *to, uint16_t nSamples){
	SoundHandler::fetchSamples(to, nSamples);
	if(!hasInputStreams()){
		cout<<"Pausing SDL Audio"<<endl;
		SDL_PauseAudio(1);
	}
}

void SoundHandlerSDL::mix(int16_t *outSamples, int16_t *inSamples,
		uint16_t nSamples, float volume){
	Uint8 *out = reinterpret_cast<Uint8*>(outSamples);
	Uint8 *in = reinterpret_cast<Uint8*>(inSamples);
	uint16_t nBytes = nSamples*2;
	SDL_MixAudio(out, in, nBytes, SDL_MIX_MAXVOLUME*volume);
}

} // namespace

#endif
