#ifndef MEDIA_BASE_VIDEO_DECODER_CONFIG_H_
#define MEDIA_BASE_VIDEO_DECODER_CONFIG_H_

#include <vector>

namespace media{
    enum VideoCodec{
        CODEC_H264,
        CODEC_MPEG2,
        CODEC_MPEG4,
        CODEC_INVALID
    };
    
    enum VideoCodecProfile{
        VIDEO_CODEC_PROFILE_UNKNOWN = -1,
        H264PROFILE_MIN = 0,
        H264PROFILE_BASELINE = H264PROFILE_MIN,
        H264PROFILE_MAIN = 1,
        H264PROFILE_EXTENDED = 2,
        H264PROFILE_HIGH = 3,
        H264PROFILE_HIGH10PROFILE = 4,
        H264PROFILE_HIGH422PROFILE = 5,
        H264PROFILE_HIGH444PREDICTIVEPROFILE = 6,
        H264PROFILE_SCALABLEBASELINE = 7,
        H264PROFILE_SCALABLEHIGH = 8,
        H264PROFILE_STEREOHIGH = 9,
        H264PROFILE_MULTIVIEWHIGH = 10,
        VIDEO_CODEC_PROFILE_MAX = H264PROFILE_MULTIVIEWHIGH,
    };
    
    enum VideoPixelFormat{
        UNKNOWN = 0,  // Unknown format value.
        YV12 = 1,     // 12bpp YVU planar 1x1 Y, 2x2 VU samples
        YV16 = 2,     // 16bpp YVU planar 1x1 Y, 2x1 VU samples
        I420 = 3,     // 12bpp YVU planar 1x1 Y, 2x2 UV samples.
        YV12A = 4,    // 20bpp YUVA planar 1x1 Y, 2x2 VU, 1x1 A samples.
        NATIVE_TEXTURE = 6,  // Native texture.  Pixel-format agnostic.
        YV12J = 7,  // JPEG color range version of YV12
        NV12 = 8,  // 12bpp 1x1 Y plane followed by an interleaved 2x2 UV plane.
        YV24 = 9,  // 24bpp YUV planar, no subsampling.
        ARGB = 10,  // 32bpp ARGB, 1 plane.
        YV12HD = 11,  // Rec709 "HD" color space version of YV12
        // Please update UMA histogram enumeration when adding new formats here.
        FORMAT_MAX = YV12HD,  // Must always be equal to largest entry logged.
      };
    
    class VideoDecoderConfig{
    public:
        VideoDecoderConfig();
        void Initialize(VideoCodec video_codec,
                   VideoCodecProfile video_codec_profile,
                   VideoPixelFormat video_pixel_format,
                   int width,
                   int height,
                   int decoded_width,
                   int decoded_height,
                   uint8_t* extra_data,
                   int extra_data_size);
        VideoCodec codec_id();
        VideoCodecProfile profile();
        VideoPixelFormat pixel_format();
        int width();
        int height();
        uint8_t* extra_data();
        int extra_data_size();
        void ShowVideoConfigInfo();
    private:
        const char* VideoCodecToString(VideoCodec video_codec);
        const char* ProfileToString(VideoCodecProfile profile);
        const char* PixelFormatToString(VideoPixelFormat pixel_format);

        VideoCodec codec_id_;
        VideoCodecProfile profile_;
        VideoPixelFormat video_pixel_format_;
        int width_;
        int height_;
        int decoded_width_;
        int decoded_height_;
        std::vector<uint8_t> extra_data_;
    };
} // namespace media

#endif