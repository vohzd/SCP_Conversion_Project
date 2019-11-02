#ifndef MARKUPOUTSTREAM_HPP
#define MARKUPOUTSTREAM_HPP

#include <iostream>
#include <string>

class MarkupOutStream{
	public:
		class MarkupOutString;
		
		MarkupOutStream();
		MarkupOutStream(std::ostream* outputStream);
		
		MarkupOutStream& operator<<(std::string str);
		MarkupOutStream& operator<<(const MarkupOutStream::MarkupOutString& str);
		MarkupOutStream& operator<<(MarkupOutStream::MarkupOutString&& str);
		
	private:
		std::ostream* outStream;
		
	public:
		class MarkupOutString{
			public:
				friend class MarkupOutStream;
				friend MarkupOutString allowMarkup(std::string str);
				friend MarkupOutString operator""_AM(const char* str, long unsigned int length);
				
			private:
				MarkupOutString(std::string str);
				
				std::string buffer;
		};
};

MarkupOutStream::MarkupOutString allowMarkup(std::string str);
MarkupOutStream::MarkupOutString operator""_AM(const char* str, long unsigned int length);

#endif // MARKUPOUTSTREAM_HPP
