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
	virtual std::streamsize read(void* dst, std::streamsize num) =0;
	virtual std::streamsize readNonBlocking(void* dst, std::streamsize num){
		return read(dst,num);
	}

	virtual std::streamsize write(const void* src, std::streamsize num);

	int read_string(char* dst, int max_length);

	float read_float32();
	//return current stream position
	virtual std::streampos tell() const  = 0;
	virtual bool seek(std::streampos p) = 0;
	virtual void go_to_end() = 0;
	virtual bool eof() const =0;
	virtual bool bad() const = 0;
	virtual size_t size() const {return static_cast<size_t>(-1);}
	virtual void reset(){}
};


} // namespace net
#endif /* IOCHANNEL_H_ */

