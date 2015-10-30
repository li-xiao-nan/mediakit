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
#include "ImageType.h"
#include <stdlib.h>
#include <iostream>

namespace MediaCore {

VideoImage::VideoImage(int w,int h, enum ImageType type):
		_w(w),
		_h(h),
		_imageType(type),
		_data(NULL),
		_pts(0){
	switch(_imageType){
	case IMAGE_RGB:
		_data = new unsigned char[_w*_h*3];
		if(!_data){
			//throw exception("new the image data buffer failed");
			std::cout<<"create the image data buffer failed"<<std::endl;
		}
		break;
	case IMAGE_ARGB:
		_data = new unsigned char[_w*_h*sizeof(unsigned char)*4];
		if(!_data){
			//throw exception("new the image data buffer failed");
			std::cout<<"create the image data buffer failed"<<std::endl;
		}
		break;
	case IMAGE_YUV:
		_imageType = IMAGE_YUV;
		_yuvStride[0] = w;
		_yuvStride[1] = w>>1;
		_yuvStride[2] = w>>1;
		_yuvLineCnt[0] = h;
		_yuvLineCnt[1] = h>>1;
		_yuvLineCnt[2] = h>>1;
		_yuvData[0] = (unsigned char*)malloc(_yuvStride[0]*_yuvLineCnt[0]);
		_yuvData[1] = (unsigned char*)malloc(_yuvStride[1]*_yuvLineCnt[1]);
		_yuvData[2] = (unsigned char*)malloc(_yuvStride[2]*_yuvLineCnt[2]);

	/*	_yuvData  = (unsigned char*)malloc(_yuvStrid[0]*_yuvLineCnt[0]+_yuvStride[1]*_yuvLineCnt[1]+_yuvStride[2]*_yuvLineCnt[2]);*/

		_yuv = (unsigned char*)malloc(_w*_h*sizeof(unsigned char)*3);
		break;
	default:
		break;
	}//switch(_type)
}

VideoImage::~VideoImage(){
	if(_data){
		//cout<<"delete the data"<<endl;
		delete (_data);
	}
	
	if(_imageType == IMAGE_YUV){
	free(_yuvData[0]);
	free(_yuvData[1]);
	free(_yuvData[2]);
	//free(_yuvData);
	free(_yuv);
	}
}

} // namespace


