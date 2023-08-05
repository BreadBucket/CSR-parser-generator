#pragma once
#include <vector>
#include <memory>


namespace csr {
	class Reduction;
	class ParsedReduction;
	class SourceString;
	class SymbolEnum;
	class Graph;
	class Document;
}


class csr::Document {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	std::string name;
	std::vector<std::shared_ptr<ParsedReduction>> parsedReductions;
	std::vector<SourceString> code;
	
public:
	std::vector<std::shared_ptr<Reduction>> reductions;
	std::shared_ptr<SymbolEnum> symEnum;
	std::shared_ptr<Graph> graph;
	
// ------------------------------------------------------------------------------------------ //	
};