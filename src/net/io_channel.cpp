#include "io_channel.h"

#include <iostream>
#include <memory>

#include "net/file_stream_provider.h"
#include "net/curl/curl_adapter.h"

namespace net {

//static function
IOChannel* IOChannel::CreateIOChannel(Url url)
{
	IOChannel* stream = NULL;
	if(url.protocol() == "file"){
		string path = url.path();
		if(0){

		}else{
			FILE *inFp = fopen(path.c_str(),"rb");
			if(!inFp){
				printf("<lxn>%s(%d)-->fopen the file of %s failed\n",__FILE__,__LINE__,path.c_str());
				printf("fopen->ERRInfo:%s\n",strerror(errno));
				return stream;
			}
			stream = new FileStreamProvider(inFp);
		}
	}else if(url.protocol() == "http"){
		stream = new CurlStreamFile(url.str(),"");
	}
	return stream;
}

unsigned int
IOChannel::read_le32(){
	unsigned int result = static_cast<unsigned int>(read_byte());
	result |= static_cast<unsigned int>(read_byte()) << 8;
	result |= static_cast<unsigned int>(read_byte()) <<16;
	result |= static_cast<unsigned int>(read_byte()) <<24;
	return result;
}

unsigned char
IOChannel::read_le16(){
	unsigned char result = static_cast<unsigned char>(read_byte());
	result |= static_cast<unsigned char>(read_byte()) << 8;
	return  result;
}

int
IOChannel::read_string(char* dst, int max_length){
	int i = 0;
	while(i < max_length){
		dst[i] = read_byte();
		if(dst[i] == '\0') return i;
		i++;
	}
	dst[max_length -1] = '\0';
	return -1;
}
float
IOChannel::read_float32(){
	union{
		float f;
		unsigned int i;
	}u;
	u.i = read_le32();
	return u.f;
}
unsigned char
IOChannel::read_byte(){
	unsigned char u;
	if(read(&u,1) == -1){
		printf("Could not read a single byte from input \n");

	}
	return u;
}
size_t
IOChannel::write(const void*,size_t num){
	return 1;
}

} // namespace net

