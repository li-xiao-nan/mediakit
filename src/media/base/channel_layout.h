//
//  channel_layout.h
//  mediacore
//
//  Created by 李楠 on 15/10/6.
//
//

#ifndef MEDIA_BASE_CHANNLE_LAYOUT_H
#define MEDIA_BASE_CHANNLE_LAYOUT_H
namespace media {
enum ChannelLayout {
  CHANNEL_LAYOUT_NONE = 0,
  CHANNEL_LAYOUT_UNSUPPORTED = 1,

  // Front C
  CHANNEL_LAYOUT_MONO = 2,

  // Front L, Front R
  CHANNEL_LAYOUT_STEREO = 3,

  // Front L, Front R, Back C
  CHANNEL_LAYOUT_2_1 = 4,

  // Front L, Front R, Front C
  CHANNEL_LAYOUT_SURROUND = 5,

  // Front L, Front R, Front C, Back C
  CHANNEL_LAYOUT_4_0 = 6,

  // Front L, Front R, Side L, Side R
  CHANNEL_LAYOUT_2_2 = 7,

  // Front L, Front R, Back L, Back R
  CHANNEL_LAYOUT_QUAD = 8,

  CHANNEL_LAYOUT_MAX = CHANNEL_LAYOUT_QUAD,

};

enum Channel {
  CHANNEL_LEFT = 0,
  CHANNEL_RIGHT,
  CHANNEL_CENTER,
  CHANNEL_LFE,
  CHANNEL_BACK_LEFT,
  CHANNEL_BACK_RIGHT,
  CHANNEL_LEFT_OF_CENTER,
  CHANNEL_RIGHT_OF_CENTER,
  CHANNEL_BACK_CENTER,
  CHANNEL_SIDE_LEFT,
  CHANNEL_SIDE_RIGHT,
  CHANNEL_MAX =
      CHANNEL_SIDE_RIGHT,  // Must always equal the largest value ever logged.
};

}  // namespace media
#endif
