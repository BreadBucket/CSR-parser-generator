#pragma once
#include <string>
#include <vector>
#include <memory>

#include "NamedStream.hpp"
#include "Symbol.hpp"
#include "SymbolEnum.hpp"
#include "Graph.hpp"
#include "Document.hpp"



namespace csr {
	class Generator;
}



class csr::Generator {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	NamedStream<std::ostream> out;
	NamedStream<std::ostream> out_header;
	NamedStream<std::ostream> out_tokenHeader;
	
private:
	const Graph* graph;
	const SymbolEnum* symbols;
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	void generate(const Graph& graph, const SymbolEnum& symbols);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	static void convertBadChars(std::string& s);
	static void generateSymbolNames(const std::vector<std::shared_ptr<Symbol>>& symbols);
	static void generateReductionNames(const std::vector<std::shared_ptr<Reduction>>& reductions);
	static void generateStateNames(const std::vector<State*>& states);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	void processTemplate(std::ostream& out, const char* data);
	
// ------------------------------------------------------------------------------------------ //
};
