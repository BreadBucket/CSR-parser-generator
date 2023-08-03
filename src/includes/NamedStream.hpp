#include <string>
#include <iostream>
#include <fstream>


template <typename T>
class NamedStream {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	std::string path = {};
	T* stream = nullptr;
	bool managed = false;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	NamedStream() = default;
	
	NamedStream(std::string&& path){
		open(move(path));
	};
	
public:
	~NamedStream(){
		if (managed)
			delete stream;
	}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	void open(std::string&& path);
	
// ----------------------------------- [ Operators ] ---------------------------------------- //
public:
	template <typename X> requires (
		std::is_base_of<std::ios,X>::value
	)
	inline operator X&(){
		return *stream;
	}
	
public:
	template <typename X> requires (
		std::is_fundamental<X>::value
	)
	inline T& operator <<(X x){
		*stream << x;
		return *stream;
	}
	
	template <typename X> requires (
		std::negation<std::is_fundamental<X>>::value
	)
	inline T& operator <<(const X& x){
		*stream << x;
		return *stream;
	}
	
public:
	template <typename X>
	inline T& operator >>(X& x){
		*stream >> x;
		return *stream;
	}
	
// ------------------------------------------------------------------------------------------ //
};




// ----------------------------------- [ Functions ] ---------------------------------------- //


template <>
void NamedStream<std::ostream>::open(std::string&& path){
	if (path == "0"){
		throw std::ios_base::failure("Invalid stream specifier: '" + path + "'.");
	} else if (path == "1"){
		stream = &std::cout;
		managed = false;
	} else if (path == "2"){
		stream = &std::cerr;
		managed = false;
	}
	
	// File
	else {
		stream = new std::ofstream(path);
		managed = true;
		
		if (stream->fail()){
			managed = false;
			delete stream;
			throw std::ios_base::failure("Failed to create output file '" + path + "'.");
		}
		
	}
	
	this->path = move(path);
}


template <>
void NamedStream<std::istream>::open(std::string&& path){
	if (path == "0"){
		stream = &std::cin;
		managed = false;
	} else if (path == "1" || path == "2"){
		throw std::ios_base::failure("Invalid stream specifier: '" + path + "'.");
	}
	
	// File
	else {
		stream = new std::ifstream(path);
		managed = true;
		
		if (stream->fail()){
			managed = false;
			delete stream;
			throw std::ios_base::failure("Failed to create input file '" + path + "'.");
		}
		
	}
	
	this->path = move(path);
}


template <>
void NamedStream<std::iostream>::open(std::string&& path){
	if (path == "0" || path == "1" || path == "2"){
		throw std::ios_base::failure("Invalid stream specifier: '" + path + "'.");
	}
	
	// File
	else {
		stream = new std::fstream(path);
		managed = true;
		
		if (stream->fail()){
			managed = false;
			delete stream;
			throw std::ios_base::failure("Failed to create file '" + path + "'.");
		}
		
	}
	
	this->path = move(path);
}


// ------------------------------------------------------------------------------------------ //