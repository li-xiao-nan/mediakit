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

#ifndef VIRTUALCOLOCK_H_
#define VIRTUALCOLOCK_H_
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

namespace MediaCore{

class VirtualClock{
public:
	virtual uint64_t elapsed() const = 0;
	virtual void restart()=0;
	virtual ~VirtualClock(){}
};

class InterruptableVirtualClock : public VirtualClock
{
public:
	InterruptableVirtualClock(VirtualClock& src):
		 _src(src),
		 _elapsed(0),
		 _offset(_src.elapsed()),
		 _paused(true){

	}

	uint64_t elapsed() const{
		if(!_paused){
			_elapsed = _src.elapsed() - _offset;
		}
		return _elapsed;
	}

	void restart(){
		_elapsed = 0;
		_offset = _src.elapsed();
	}
	void pause(){
		if(_paused) return ;
		_paused = true;
	}
	void resume(){
		if(!_paused) return ;
		_paused = false;

		uint64_t now = _src.elapsed();
		_offset = (now - _elapsed);
		assert(now-_offset == _elapsed);
	}
private:
	VirtualClock& _src;
	mutable uint64_t _elapsed;
	uint64_t _offset;
	bool _paused;
};

}//namespce

#endif /* VIRTUALCOLOCK_H_ */
