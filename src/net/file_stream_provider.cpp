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

#include <stdio.h>
#include "file_stream_provider.h"
#include <string>

namespace net {

FileStreamProvider::FileStreamProvider(FILE* fp)
    : _fp(fp), _pos(0), _error(false) {
  fseek(_fp, 0, SEEK_END);
  _size = ftell(_fp);
  fseek(_fp, 0, SEEK_SET);
  printf("position = %ld\n", ftell(_fp));
}
size_t FileStreamProvider::read(void* dst, size_t num) {
  if (_fp) {
    int cont = 0;
    cont = fread(dst, 1, num, _fp);
    _pos += cont;
    return cont;
  }
  return -1;
}
long FileStreamProvider::tell() const {
  if (_fp) {
    return ftell(_fp);
  }
  return -1;
}
int FileStreamProvider::seek(long p) {
  int result = -1;
  if (_fp) {
    result = fseek(_fp, p, SEEK_SET);
  }
  return result;
}
void FileStreamProvider::go_to_end() {
  if (_fp) {
    fseek(_fp, 0, SEEK_END);
  }
}
bool FileStreamProvider::eof() const {
  if (_fp) {
    return feof(_fp) == 0x0010;
  }
  return true;
}
bool FileStreamProvider::bad() const { return _error; }
size_t FileStreamProvider::size() const { return _size; }

}  // namespace
