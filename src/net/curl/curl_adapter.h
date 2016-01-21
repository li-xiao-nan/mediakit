#ifndef NET_CURL_CURL_ADAPTER_H
#define NET_CURL_CURL_ADAPTER_H

#include "net/io_channel.h"

extern "C"{
#include "curl/curl.h"
}

namespace net{
class CurlStreamFile : public IOChannel{
public:
	typedef map<string, string> PostData;

	CurlStreamFile(const string& url, const string& cachefile);


	CurlStreamFile(const string& url, const string& vars,
			const string& cachefile);
	~CurlStreamFile();

	//see dox in IOChannel.h
	virtual std::streamsize read(void *dst, std::streamsize bytes);
	virtual std::streamsize readNonBlocking(void* dst, std::streamsize bytes);
	virtual bool eof() const;
	virtual bool bad() const;
	virtual std::streampos tell() const ; // return the global position within the file
	virtual void go_to_end();
	virtual bool seek(std::streampos pos);
	//return the size of the stream
	// if size of the stream is unknown, 0 is returned .
	//in that case you might try calling this function again after filling
	//the cache a bit
	//Another approach might be filling the cache ourselves
	//aming at obtainning a useful value
	virtual size_t size() const;
private:
	void init(const string& url, const string& cachefile);
	//use this file to cache data
	FILE* _cache;
	//_cache file descriptor
	int _cachefd;

	/*we keep a copy here to be sure the char* is alive for
	 * the whole CurlStreamFile lieftime
	 * */
	string _url;
	//the libcurl esay hanle
	CURL *_curlHandle;
	//the libcrul multi handle
	CURLM *_mCurlHandle;
	//transfer in progress
	int _running;
	//stream error, false on no error
	bool _error;
	//post data, empty if no post has been requested
	string _postdata;
	//Current size of cached data
	std::streampos _cached;
	//total stream size
	mutable size_t _size;

	//Attempt at filling the cache up to the given size
	//will call libcurl routines to fetch data
	void fillCache(std::streampos size);

	//process pending curl message(handle 404)
	void processMessages();
	//filling the cache as much as possable without blocking.
	//will call libcurl routines to fetch data
	void fillCacheNonBlocking();
	//appedn sz bytes to the cache;
	std::streamsize cache(void* from, std::streamsize sz);

	//callback for libcurl ,will  be called
	//by fillCache() and will call cache();
	static size_t recv(void *buf, size_t size, size_t nmemb, void *userp);
	//list of custom headers for this stream
	struct curl_slist *_customHeaders;
};

} // namespace net

#endif