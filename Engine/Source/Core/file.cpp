#include "file.h"

#include <fstream>
#include <sstream>
#include <iostream>

std::string readFileToString(const std::string& filePath) {
	std::ifstream fileStream = std::ifstream(filePath);
	if (!fileStream) {
		std::cerr << "Error opening file:" << filePath << std::endl;
		return "";
	}

	std::streambuf* fileBuffer = fileStream.rdbuf();
	std::stringstream buffer;
	buffer << fileBuffer;
	fileStream.close();
	return buffer.str();
}