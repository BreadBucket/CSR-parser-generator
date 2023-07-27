#pragma once
#include <string>
#include <ostream>

#include "SymbolEnum.hpp"
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
	void serialize(std::ostream& stream, const Graph& graph, const SymbolEnum& symEnum);
	void serialize_txt(std::ostream& stream, const Graph& graph, const SymbolEnum& symEnum);

public:
	static Format getFormat(const std::string& filePath);
	
// ------------------------------------------------------------------------------------------ //
};
}