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

#ifndef IMAGETYPE_H_
#define IMAGETYPE_H_

#include <string>
#include <stdint.h>
#include <iostream>

namespace MediaCore {

enum ImageType{
	IMAGE_INVALID,
	IMAGE_YUV,
	IMAGE_RGB,
	IMAGE_ARGB,
	IMAGE_TYPE_COUNT
};

class VideoImage{
public:
	VideoImage(int w, int h, ImageType type);
	~VideoImage();
	unsigned char* begin() const{
		return _data;
	}
	unsigned int _w;
	unsigned int _h;
	ImageType  _imageType;
	unsigned char *_data;
	unsigned char *_yuvData[3];
	unsigned char *_yuv;
	int _yuvStride[3];
	int _yuvLineCnt[3];
	uint64_t _pts;

};

} // namespace



#endif /* IMAGETYPE_H_ */
