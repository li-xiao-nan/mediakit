#ifndef IOCHANNEL_H_
#define IOCHANNEL_H_

#include <string>
#include <stdio.h>
#include <iostream>

#include "url.h"

namespace net {

class IOChannel{
public:
	virtual ~IOChannel(){}

	static IOChannel* CreateIOChannel(Url url);

	unsigned int read_le32();
	unsigned char read_le16();

	unsigned char read_byte();
	virtual size_t read(void* dst, size_t num) =0;
	virtual size_t readNonBlocking(void* dst, size_t num){
		return read(dst,num);
	}

	virtual size_t write(const void* src, size_t num);

	int read_string(char* dst, int max_length);

	float read_float32();
	//return current stream position
	virtual long tell() const  = 0;
	virtual int seek(long p) = 0;
	virtual void go_to_end() = 0;
	virtual bool eof() const =0;
	virtual bool bad() const = 0;
	virtual size_t size() const {return static_cast<size_t>(-1);}
	virtual void reset(){}
};


} // namespace net
#endif /* IOCHANNEL_H_ */

