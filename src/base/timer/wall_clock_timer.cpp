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

namespace MediaCore {

WallClockTimer::WallClockTimer() : start_timestamp_(0), is_ticking_(false) {}

void WallClockTimer::Start() {
  start_timestamp_ = getTicks();
  is_ticking_ = true;
}

void WallClockTimer::Stop() { is_ticking_ = false; }

void WallClockTimer::Restart() { Start(); }

int64_t WallClockTimer::Elapsed() const {
  int64_t current_timestamp = getTicks();
  if (!is_ticking_ || current_timestamp <= start_timestamp_) return 0;
  return current_timestamp - start_timestamp_;
}

}  // namespace
