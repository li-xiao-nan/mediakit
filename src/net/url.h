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

#ifndef URL_H_
#define URL_H_

#include <iosfwd>
#include <string>
#include <map>

using namespace std;

namespace net{

class Url{
public:
	friend ostream& operator<<(ostream& o, const Url& u);
	Url(const string& absUrl); //absolute url
	Url(const string& relUrl, const string& baseUrl);

	//utility function for gotting the filed of the url;
	const string& protocol() const ;
	const string& hostName() const ;
	const string& port() const;
	const string& path() const; //the path starts with '/'
	const string& anchor() const ; //the anchor is the string after the '#' character
	const string& queryString() const ; //the query is the string after the '?' character
	void   setQueryString(const string& value);
	string  str() const ;//return the full URL as a string
  string OriginalUrl() const;

	///@function name: parseQueryString
	static void parseQueryString(const string& str,
			map<string, string>& targetMap);

	///@static function name: encode(string& str);
	//brief: Encode a string to URL-encoded format converting all dodgy to %AB hex squences
	// Character that need escaping are:
	// - ASCII control characters: 0-31 and 127
	// - Non-ASCII chars: 128-255
	// - Unsafe characters : SPACE " < > # % { } | \ ^ ~ [] '
	// Encoding is a% followed by two hexadecimal characters , case insensitive
	// See RFC1738 http://www.rfc-editor.org/rfc/rfc1738.txt, see 2.2 "URL Character Encoding issues"

	static void encode(string& str);
	static string encode(const string& str);

	//brief: decode a string from URL-encoded format
	// converting all hexadecimal sequence to ASCII characters.
	static void decode(string& str);

	//private member functions
private:
	void initUrl(const string& asdUrl);
	void initUrl(const string& relUrl, const string& baseUrl);
	void splitPort(); //extract tcp/ip port from path
	void splitAnchor(); //extract anchor from path
	void splitQueryString();
	///@function name: normalizePath()
	/// normalization  currently include removal of adjacent slashes, "./" dirs
	// components removal, and "../../" components resolution
	// void normalizePath(string& path);

private:
	string _protocol;
	string _hostName;
	string _port;
	string _path;
	string _anchor;
	string _queryString;
  string original_url_;
};//class url

} // namespace
#endif /* URL_H_ */
