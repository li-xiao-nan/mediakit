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


#ifndef INPUTSTREAM_H_
#define INPUTSTREAM_H_
#include <stdint.h>

namespace MediaCore {

//Instance of this class are sounds you can pull samples from
/* The format of the samples you pull is expected to be PCM samples
 * as signed 16bit values
 * in system-endian format.
 * It is expected that consecutive samples fetched are one for left
 * and one for right stereo channel, in that order
 */
class InputStream{
public:
	/*fetch the given amount of samples, non-blocking and thread-safe
	 * @param to
	 *     Ouput buffer, must be at least nSamples*bytes.
	 * @param nSamples
	 *    Number of samples to fetch
	 *  @return number of samples actually written to the ouput buffer
	 */
	virtual uint16_t fetchSamples(int16_t *to, uint16_t nSamples)=0;
	virtual uint16_t samplesFetched() const = 0;
	///return true if there'll be no more data to fetch.
	virtual bool eof() const=0;
	virtual ~InputStream() {}
};

} // namespace

#endif /* INPUTSTREAM_H_ */
