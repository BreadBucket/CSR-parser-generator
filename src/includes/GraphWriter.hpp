#pragma once
#include <string>
#include <iostream>

#include "Graph.hpp"


namespace csr {
class GraphWriter {
// ---------------------------------- [ Structures ] ---------------------------------------- //
public:
	enum class Format {
		UNKNOWN,
		TXT
	};
	
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	Format format = Format::TXT;
	bool unicode = true;
	bool ansi = true;
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	void write(const Graph& graph, std::ostream& stream);

public:
	static Format getFormat(const std::string& filePath);
	
// ------------------------------------------------------------------------------------------ //
};
}