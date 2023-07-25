#pragma once
#include <string>
#include <ostream>

#include "Graph.hpp"


namespace csr {
class GraphSerializer {
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
	void serialize(const Graph& graph, std::ostream& stream);
	void serialize_txt(const Graph& graph, std::ostream& stream);

public:
	static Format getFormat(const std::string& filePath);
	
// ------------------------------------------------------------------------------------------ //
};
}