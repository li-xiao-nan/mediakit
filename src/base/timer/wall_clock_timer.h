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

#ifndef BASE_WALLCLOCKTIMER_H_
#define BASE_WALLCLOCKTIMER_H_

#include <stdint.h>

namespace MediaCore {

class WallClockTimer {
 public:
  WallClockTimer();
  int64_t Elapsed() const;
  void Start();
  void Stop();
  void Restart();
  void Pause();
  void Resume();
  void Seek(int64_t);

 private:
  int64_t start_timestamp_;
  int64_t pause_timestamp_;
  int64_t pause_state_interval_;
  int64_t seek_timestamp_interval_;
  bool is_ticking_;
  bool is_pausing_;
};

}  // namespace

#endif /* WALLCLOCKTIMER_H_ */
