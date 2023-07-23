#pragma once
#include <unordered_map>
#include <memory>

#include "Symbol.hpp"


namespace csr {
class SymbolEnum {
// ------------------------------------[ Variables ] ---------------------------------------- //
public:
	const std::shared_ptr<Symbol> nullSymbol = nullptr;
	
// ------------------------------------[ Variables ] ---------------------------------------- //
private:
	SymbolID prevId = SymbolID(0);
	std::unordered_map<SymbolID,std::shared_ptr<Symbol>> map_id;
	std::unordered_map<std::string,std::shared_ptr<Symbol>> map_name;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	SymbolEnum() = default;
	SymbolEnum(const std::shared_ptr<Symbol>& null) : nullSymbol{null} {};
	SymbolEnum(std::shared_ptr<Symbol>&& null) : nullSymbol{move(null)} {};
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	const Symbol* get(SymbolID id) const;
	const Symbol* get(const std::string& name) const;
	
public:
	std::shared_ptr<Symbol> share(SymbolID id) const;
	std::shared_ptr<Symbol> share(const std::string& name) const;
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	SymbolID put(const std::string& name);
	
// ------------------------------------------------------------------------------------------ //
};
}