#include "Exception/OException.h"
#include <sstream>

OException::OException(int line, const char *file) noexcept
	: line(line),
	file(file) {
}

const char *OException::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << std::endl
			<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char *OException::GetType() const noexcept {
	return "O Exception";
}

int OException::GetLine() const noexcept {
	return line;
}

const std::string &OException::GetFile() const noexcept {
	return file;
}

std::string OException::GetOriginString() const noexcept {
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
			<< "[Line] " << line;
	return oss.str();
}
