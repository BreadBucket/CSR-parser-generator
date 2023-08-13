#pragma once
#include <string>
#include <vector>
#include <memory>

#include "NamedStream.hpp"


struct Tab;

namespace csr {
	class Symbol;
	class Reduction;
	class State;
	class Connection;
	class Graph;
	class Document;
	class Generator;
}


class csr::Generator {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	NamedStream<std::ostream> out;
	NamedStream<std::ostream> out_header;
	NamedStream<std::ostream> out_tokenHeader;
	
public:
	bool inline_header = true;
	bool inline_tokenHeader = true;
	std::string headerPath;			// Include as file
	std::string tokenHeaderPath;	// Include as file
	
// ------------------------------------[ Variables ] ---------------------------------------- //
private:
	Document* doc;
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	void generate(Document& doc);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	static void convertBadChars(std::string& s);
	static void generateSymbolNames(const std::vector<std::shared_ptr<Symbol>>& symbols);
	static void generateReductionNames(const std::vector<std::shared_ptr<Reduction>>& reductions);
	static void generateStateNames(const std::vector<State*>& states);
	static void generateReductionItemNames(const std::vector<Connection*>& connections);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	void processTemplate(std::ostream& out, const char* data);
	
private:
	static void generateTokenEnum(std::ostream& out, const Tab& tab, const std::vector<std::shared_ptr<Symbol>>& symbols);
	static void generateTokenEnumNameSwitch(std::ostream& out, const Tab& tab, const std::vector<std::shared_ptr<Symbol>>& symbols);
	static void generateStateSwitch(std::ostream& out, const Tab& tab, const std::vector<State*> states);
	static void generateTransitionSwitch(std::ostream& out, const Tab& tab, const std::vector<State*> states);
	static void generateReductions(std::ostream& out, const Tab& tab, Document& doc);
	static void generateReductionItems(std::ostream& out, const Tab& tab, Graph& graph);
	
// ------------------------------------------------------------------------------------------ //
};
