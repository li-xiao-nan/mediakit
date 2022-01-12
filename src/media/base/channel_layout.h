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
