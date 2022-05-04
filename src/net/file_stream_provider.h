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

#ifndef fileStream_h
#define fileStream_h

#include "net/io_channel.h"
#include <stdio.h>

#include "base/macros.h"

namespace net {

class FileStreamProvider: public IOChannel{
public:
	FileStreamProvider(FILE* fp);
  ~FileStreamProvider() override;
	virtual size_t read(void* dst, size_t num);
	virtual long tell() const;
	virtual int seek(long p);
	virtual void go_to_end();
	virtual bool eof() const;
	virtual bool bad() const ;
	virtual size_t size() const;
private:
	FILE* _fp;
	int _pos;
	bool _error;
	long _size;

  DISALLOW_COPY_AND_ASSIGN(FileStreamProvider);
};

} // namespace
#endif
