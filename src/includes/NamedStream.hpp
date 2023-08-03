#pragma once
#include <string>
#include <iostream>
#include <fstream>


// ----------------------------------- [ Structures ] --------------------------------------- //


template<typename T>
struct io_to_fio {
	using type = std::fstream;
	static constexpr bool valid = false;
};

template<>
struct io_to_fio<std::ostream> {
	using type = std::ofstream;
	static constexpr bool valid = true;
};

template<>
struct io_to_fio<std::istream> {
	using type = std::ifstream;
	static constexpr bool valid = true;
};

template<>
struct io_to_fio<std::iostream> {
	using type = std::fstream;
	static constexpr bool valid = true;
};


// ------------------------------------------------------------------------------------------ //




template <typename T> requires ( io_to_fio<T>::valid )
class NamedStream {
// ------------------------------------------------------------------------------------------ //
public:
	using fio = io_to_fio<T>::type;
	
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	std::string path;
	
private:
	T* stream;	// Not null
	fio fileStream;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	NamedStream() : fileStream(), stream{&fileStream} {}
	
	NamedStream(std::string&& path) : NamedStream() {
		open(move(path));
	}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	inline bool isFile(){
		return ((void*)stream == (void*)&fileStream);
	}
	
	inline void close(){
		fileStream.close();
		stream = &fileStream;
	}
	
public:
	/**
	 * @brief 
	 * @throws runtime_error when opening IO stream fails.
	 */
	void open(std::string&& path);
	
	/**
	 * @brief 
	 * @throws runtime_error when opening IO stream fails.
	 */
	inline void open(const std::string& path){
		open(std::string(path));
	}
	
// ----------------------------------- [ Operators ] ---------------------------------------- //
public:
	inline operator T&(){
		return *stream;
	}
	
	inline operator fio&(){
		return fileStream;
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