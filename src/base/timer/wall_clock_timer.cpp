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

#include "wall_clock_timer.h"
#include "ClockTime.h"
#include "log/log_wrapper.h"

namespace MediaCore {

WallClockTimer::WallClockTimer() :
  start_timestamp_(0)
  ,is_ticking_(false)
  ,pause_state_interval_(0)
  ,is_pausing_(false)
  ,seek_timestamp_interval_(0){}

void WallClockTimer::Start() {
  start_timestamp_ = getTicks();
  is_ticking_ = true;
}

void WallClockTimer::Stop() { is_ticking_ = false; }

void WallClockTimer::Restart() { Start(); }

int64_t WallClockTimer::Elapsed() const {
  int64_t current_timestamp = getTicks();
  if (!is_ticking_ || current_timestamp <= start_timestamp_) return 0;
  int64_t elapsed =  
    current_timestamp - start_timestamp_ - pause_state_interval_;
  if(seek_timestamp_interval_ > 0) {
      elapsed += seek_timestamp_interval_;
  } else {
    elapsed -= seek_timestamp_interval_;
  }
  return elapsed;
}

void WallClockTimer::Pause() {
  if(is_pausing_) {
    return;
  } else {
  pause_timestamp_ = getTicks();
  is_pausing_ = true;
  }
}
void WallClockTimer::Resume() {
  if(!is_pausing_) return;
  pause_state_interval_ += getTicks() - pause_timestamp_;
  is_pausing_ = false;
}

void WallClockTimer::Seek(int64_t seek_timestamp) {
  seek_timestamp_interval_ += seek_timestamp - Elapsed();
  LogMessage(media::LOG_LEVEL_DEBUG, "seek_timestamp_interval_:" + std::to_string(seek_timestamp_interval_));
}

}  // namespace
