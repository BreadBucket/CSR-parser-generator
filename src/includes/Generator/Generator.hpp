#pragma once

#include <vector>
#include <ostream>
#include <memory>

#include "Symbol.hpp"
#include "SymbolEnum.hpp"
#include "Graph.hpp"
#include "Document.hpp"



namespace csr {
	class Generator;
}



class csr::Generator {
// ------------------------------------[ Properties ] --------------------------------------- //
private:
	// const Graph* graph;
	// const std::vector<std::shared_ptr<Symbol>>* symbols;
	// std::ostream* out;
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	void generate(const Graph& graph, const SymbolEnum& symbols);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	static void convertBadChars(std::string& s);
	static void generateSymbolNames(const std::vector<std::shared_ptr<Symbol>>& symbols);
	static void generateReductionNames(const std::vector<std::shared_ptr<Reduction>>& reductions);
	static void generateStateNames(const std::vector<State*>& states);
	
// ------------------------------------------------------------------------------------------ //
};
