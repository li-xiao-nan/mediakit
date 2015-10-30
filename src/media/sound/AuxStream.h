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

#ifndef AUXSTREAM_H_
#define AUXSTREAM_H_

#include "InputStream.h"

namespace MediaCore {

typedef uint16_t (*auxStreamPtr)(void *udata,
		int16_t *samples, uint16_t nSamples, bool& eof);
class AuxStream : public InputStream{
public:
	AuxStream(auxStreamPtr cb, void * arg):
		_samplesFetched(0),
		_eof(false),
		_cb(cb),
		_cbArg(arg)
	{}

	//see dox in inputstream.h
	uint16_t fetchSamples(int16_t *to, uint16_t nSamples){
		uint16_t wrote = _cb(_cbArg, to, nSamples, _eof);
		_samplesFetched += wrote;
		return wrote;
	}

	//see dox in inputstream.h
	uint16_t samplesFetched() const{
		return _samplesFetched;
	}
	//see dox in inputStream.h
	bool eof() const{
		return _eof;
	}
private:
	uint16_t _samplesFetched;
	bool _eof;
	auxStreamPtr _cb;
	void *_cbArg;
};//class AuxStream : public InputStream

} // namespace

#endif /* AUXSTREAM_H_ */
