#include <iostream>
#include "video_decoder_config.h"

namespace media{
VideoDecoderConfig::VideoDecoderConfig()
	: codec_id_(CODEC_INVALID),
      profile_(VIDEO_CODEC_PROFILE_UNKNOWN),
      video_pixel_format_(UNKNOWN),
      width_(0),
      height_(0),
      decoded_width_(0),
      decoded_height_(0){

      }

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
const char* VideoDecoderConfig::VideoCodecToString(VideoCodec video_codec){
	switch(video_codec){
		case CODEC_H264:
			return "CODEC_H264";
        case CODEC_MPEG2:
        	return "CODEC_MPEG2";
        case CODEC_MPEG4:
        	return "CODEC_MPEG4";
        default:
        	return "CODEC_INVALID";
	}
}
const char* VideoDecoderConfig::ProfileToString(VideoCodecProfile profile){
  std::cout<<"profile:"<<profile<<std::endl;
	switch(profile){
        case H264PROFILE_BASELINE:
        	return "H264PROFILE_BASELINE";
        case H264PROFILE_MAIN:
          return "H264PROFILE_MAIN";
        case H264PROFILE_EXTENDED:
        	return "H264PROFILE_EXTENDED";
        case H264PROFILE_HIGH:
        	return "H264PROFILE_HIGH";
        case H264PROFILE_HIGH10PROFILE:
        	return "H264PROFILE_HIGH10PROFILE";
        case H264PROFILE_HIGH422PROFILE:
        	return "H264PROFILE_HIGH422PROFILE";
        case H264PROFILE_HIGH444PREDICTIVEPROFILE:
        	return "H264PROFILE_HIGH444PREDICTIVEPROFILE";
        case H264PROFILE_SCALABLEBASELINE:
        	return "H264PROFILE_SCALABLEBASELINE";
        case H264PROFILE_SCALABLEHIGH:
        	return "H264PROFILE_SCALABLEHIGH";
        case H264PROFILE_STEREOHIGH:
        	return "H264PROFILE_SCALABLEHIGH";
        case H264PROFILE_MULTIVIEWHIGH:
        	return "H264PROFILE_SCALABLEHIGH";
        default:
          std::cout<<"profile:"<<profile<<std::endl;
        	return "unknown profile";
        }
}

const char* VideoDecoderConfig::PixelFormatToString(VideoPixelFormat pixel_format){
	switch(pixel_format){
		case YV12:
			return "YV12";
        case YV16:
        	return "YV16";
        case I420:
        	return "I420";
        case YV12A:
        	return "YV12A";
        case NATIVE_TEXTURE:
        	return "NATIVE_TEXTURE";
        case YV12J:
        	return "YV12J";
        case NV12:
        	return "NV12";
        case YV24:
        	return "YV24";
        case ARGB:
        	return "ARGB";
        case YV12HD:
        	return "YV12HD";
        default:
        	return "pixleformat unkonwn";
        
	}
}

void VideoDecoderConfig::ShowVideoConfigInfo(){
	printf("video:\n");
	printf("    codec:%s; pixelformat:%s; profile:%s; width:%d, height:%d\n"
		, VideoCodecToString(codec_id_)
		, PixelFormatToString(video_pixel_format_)
		, ProfileToString(profile_)
		, width_
		, height_);
}
} // namespace media