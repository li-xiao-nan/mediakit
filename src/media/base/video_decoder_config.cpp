#include <iostream>
#include "video_decoder_config.h"

namespace media{
VideoDecoderConfig::VideoDecoderConfig()
	: codec_id_(AV_CODEC_ID_NONE),
      profile_(FF_PROFILE_UNKNOWN),
      video_pixel_format_(AV_PIX_FMT_NONE),
      width_(0),
      height_(0),
      decoded_width_(0),
      decoded_height_(0){}

void VideoDecoderConfig::Initialize(VideoCodec video_codec,
                   VideoCodecProfile video_codec_profile,
                   VideoPixelFormat video_pixel_format,
                   int width,
                   int height,
                   int decoded_width,
                   int decoded_height,
                   uint8_t* extra_data,
                   int extra_data_size){
	codec_id_ = video_codec;
	profile_ = video_codec_profile;
	video_pixel_format_ = video_pixel_format;
	width_ = width;
	height_ = height;
	decoded_width_ = decoded_width;
	decoded_height_ = decoded_height;
	extra_data_.assign(extra_data, extra_data + extra_data_size);
}
        
VideoCodec VideoDecoderConfig::codec_id(){
	return codec_id_;
}

VideoCodecProfile VideoDecoderConfig::profile(){
	return profile_;
}

VideoPixelFormat VideoDecoderConfig::pixel_format(){
	return video_pixel_format_;
}

int VideoDecoderConfig::width(){
	return width_;
}

int VideoDecoderConfig::height(){
	return height_;
}

uint8_t* VideoDecoderConfig::extra_data(){
	if(extra_data_.empty())
		return NULL;
	else
		return &extra_data_[0];
}

int VideoDecoderConfig::extra_data_size(){
	return extra_data_.size();
}

void VideoDecoderConfig::ShowVideoConfigInfo(){
	printf("video:\n");
	printf("    codec:%d; pixelformat:%d; profile:%d; width:%d, height:%d\n"
		, codec_id_
		, video_pixel_format_
		, profile_
		, width_
		, height_);
}
} // namespace media