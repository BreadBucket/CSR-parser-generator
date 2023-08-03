#include "NamedStream.hpp"

using namespace std;


// ----------------------------------- [ Functions ] ---------------------------------------- //


template <>
void NamedStream<std::ostream>::open(std::string&& path){
	this->path.clear();
	close();
	
	if (path.empty()){
		throw std::ios_base::failure("Missing file path.");
	} else if (path == "0"){
		throw std::ios_base::failure("Invalid stream specifier: '" + path + "'.");
	} else if (path == "1"){
		stream = &std::cout;
	} else if (path == "2"){
		stream = &std::cerr;
	}
	
	// File
	else {
		fileStream.open(path);
		
		if (fileStream.fail()){
			throw std::ios_base::failure("Failed to create output file '" + path + "'.");
		}
		
		stream = &fileStream;
		this->path = move(path);
	}
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


template <>
void NamedStream<std::istream>::open(std::string&& path){
	this->path.clear();
	close();
	
	if (path.empty()){
		throw std::ios_base::failure("Missing file path.");
	} else if (path == "0"){
		stream = &std::cin;
	} else if (path == "1" || path == "2"){
		throw std::ios_base::failure("Invalid stream specifier: '" + path + "'.");
	}
	
	// File
	else {
		fileStream.open(path);
		
		if (fileStream.fail()){
			throw std::ios_base::failure("Failed to create input file '" + path + "'.");
		}
		
		stream = &fileStream;
		this->path = move(path);
	}
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


template <>
void NamedStream<std::iostream>::open(std::string&& path){
	this->path.clear();
	close();
	
	if (path.empty()){
		throw std::ios_base::failure("Missing file path.");
	} else if (path == "0" || path == "1" || path == "2"){
		throw std::ios_base::failure("Invalid stream specifier: '" + path + "'.");
	}
	
	// File
	else {
		fileStream.open(path);
		
		if (fileStream.fail()){
			throw std::ios_base::failure("Failed to create file '" + path + "'.");
		}
		
		stream = &fileStream;
		this->path = move(path);
	}
	
}


// ------------------------------------------------------------------------------------------ //