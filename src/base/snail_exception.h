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

#ifndef SNAILEXCEPTION_H_
#define SNAILEXCEPTION_H_
#include <stdexcept>
#include <string>

namespace base {

class SnailException :public std::runtime_error{
public:
	SnailException(const std::string& s):
		std::runtime_error(s){
	}
	SnailException():
		std::runtime_error("Generic error")
	{}
	virtual ~SnailException() throw() {}
};

} // namespace

#endif /* SNAILEXCEPTION_H_ */
