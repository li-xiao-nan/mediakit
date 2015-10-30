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

#ifndef SNAILSLEEP_H_
#define SNAILSLEEP_H_

// Headers for sleep
#if defined(_WIN32) || defined(WIN32)
# include <windows.h>
#else
# include <time.h>
#endif
namespace MediaCore {

inline void snailSleep(size_t useconds)
{
#if defined(_WIN32) || defined(WIN32)
    Sleep(useconds / 1000);
#else
    const size_t m = 1000000;
    const struct timespec t = { useconds / m, (useconds % m) * 1000 };
    ::nanosleep(&t, 0);
#endif
}

} // namespace

#endif /* SNAILSLEEP_H_ */

