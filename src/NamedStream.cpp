#include "NamedStream.hpp"
#include "stdexcept"

using namespace std;


// ----------------------------------- [ Functions ] ---------------------------------------- //


template <>
void NamedStream<ostream>::open(string&& path){
	this->path.clear();
	close();
	
	if (path.empty()){
		throw runtime_error("Missing file path.");
	} else if (path == "0"){
		throw runtime_error("Invalid stream specifier: '" + path + "'.");
	} else if (path == "1"){
		stream = &cout;
	} else if (path == "2"){
		stream = &cerr;
	}
	
	// File
	else {
		fileStream.open(path);
		
		if (fileStream.fail()){
			throw runtime_error("Failed to create output file '" + path + "'.");
		}
		
		stream = &fileStream;
		this->path = move(path);
	}
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


template <>
void NamedStream<istream>::open(string&& path){
	this->path.clear();
	close();
	
	if (path.empty()){
		throw runtime_error("Missing file path.");
	} else if (path == "0"){
		stream = &cin;
	} else if (path == "1" || path == "2"){
		throw runtime_error("Invalid stream specifier: '" + path + "'.");
	}
	
	// File
	else {
		fileStream.open(path);
		
		if (fileStream.fail()){
			throw runtime_error("Failed to create input file '" + path + "'.");
		}
		
		stream = &fileStream;
		this->path = move(path);
	}
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


template <>
void NamedStream<iostream>::open(string&& path){
	this->path.clear();
	close();
	
	if (path.empty()){
		throw runtime_error("Missing file path.");
	} else if (path == "0" || path == "1" || path == "2"){
		throw runtime_error("Invalid stream specifier: '" + path + "'.");
	}
	
	// File
	else {
		fileStream.open(path);
		
		if (fileStream.fail()){
			throw runtime_error("Failed to create file '" + path + "'.");
		}
		
		stream = &fileStream;
		this->path = move(path);
	}
	
}


// ------------------------------------------------------------------------------------------ //