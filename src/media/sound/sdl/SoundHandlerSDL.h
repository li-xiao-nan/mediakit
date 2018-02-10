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

#ifndef SOULDHANDLERSDL_H_
#define SOULDHANDLERSDL_H_
#include "media/sound/SoundHandler.h"
#include "SDL_audio.h"
#include "SDL.h"

namespace MediaCore {

class SoundHandlerSDL : public SoundHandler{
public:
	SoundHandlerSDL();
	~SoundHandlerSDL();
	virtual void plugInputStream(auto_ptr<InputStream> in);
	//see dox in SoundHandler.h
	void fetchSamples(int16_t* to, uint16_t nSamples);
	virtual void mix(int16_t *outSamples, int16_t *inSamples,
			uint16_t nSamples, float volume);
    virtual void OpenAudio(int smaples_rate, uint8_t channel_count);
private:
	SDL_AudioSpec _audioSpec;

	//Initialize audio card
	void initAudio();
	void closeAudio();
	bool _audioOpened;

	//callback invoked by the SDL audio thread
	static void sdlAudioCB(void *opaque, uint8_t *buffer, int bufferLength);

};//class SoundHandlerSDL

} // namespace

#endif /* SOULDHANDLERSDL_H_ */

#endif
