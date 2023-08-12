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
	class Document;
	class Generator;
}


class csr::Generator {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	NamedStream<std::ostream> out;
	NamedStream<std::ostream> out_header;
	NamedStream<std::ostream> out_tokenHeader;
	
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
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	void processTemplate(std::ostream& out, const char* data);
	
private:
	static void generateTokenEnum(std::ostream& out, const Tab& tab, const std::vector<std::shared_ptr<Symbol>>& symbols);
	static void generateTokenEnumNameSwitch(std::ostream& out, const Tab& tab, const std::vector<std::shared_ptr<Symbol>>& symbols);
	static void generateStateSwitch(std::ostream& out, const Tab& tab, const std::vector<State*> states);
	static void generateTransitionSwitch(std::ostream& out, const Tab& tab, const std::vector<State*> states);
	static void generateReductions(std::ostream& out, const Tab& tab, Document& doc);
	
// ------------------------------------------------------------------------------------------ //
};
