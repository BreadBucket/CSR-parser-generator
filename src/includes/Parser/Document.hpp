#pragma once
#include <vector>
#include <memory>

#include "SourceString.hpp"
#include "ParsedReduction.hpp"
#include "SymbolEnum.hpp"
#include "Graph.hpp"


namespace csr {
class Document {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	std::string name;
	std::vector<ParsedReduction> parsedReductions;
	std::vector<SourceString> code;
	
public:
	std::vector<std::shared_ptr<Reduction>> reductions;
	SymbolEnum symEnum;
	Graph graph;
	
// ------------------------------------------------------------------------------------------ //	
};
}