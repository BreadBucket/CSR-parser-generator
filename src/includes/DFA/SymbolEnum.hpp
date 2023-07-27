#pragma once
#include <vector>
#include <unordered_map>
#include <memory>

#include "Symbol.hpp"


namespace csr {
class SymbolEnum {
// ------------------------------------[ Variables ] ---------------------------------------- //
private:
	SymbolID nextId = SymbolID(0);
	std::vector<std::shared_ptr<Symbol>> symbols;
	std::unordered_map<std::string,int> map_name;
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	inline int size() const {
		return symbols.size();
	}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	const Symbol* get(SymbolID id) const;
	const Symbol* get(const std::string& name) const;
	
public:
	std::shared_ptr<Symbol> share(SymbolID id) const;
	std::shared_ptr<Symbol> share(const std::string& name) const;
	
public:
	const std::shared_ptr<Symbol>& put(const std::string& name);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	/**
	 * @brief Get the underlying list of symbols ordered by their ID.
	 * @return Vector of shared symbol pointers.
	 */
	inline const std::vector<std::shared_ptr<Symbol>>& getSymbols() const {
		return symbols;
	}
	
// ------------------------------------------------------------------------------------------ //
};
}