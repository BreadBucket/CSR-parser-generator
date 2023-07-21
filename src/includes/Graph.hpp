#pragma once
#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <memory>

#include "Symbol.hpp"
#include "Reduction.hpp"
#include "CSRException.hpp"



namespace CSR {
	class Item;
	class State;
	class Connection;
	class Graph;
	class GraphException;
}


namespace CSR {
	typedef std::unordered_map<std::string,int> Map_SymbolToId;
	typedef std::unordered_map<int,std::string> Map_IdToSymbol;
}




class CSR::Item {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	int reductionId;
	
public:
	int observed;
	int missing;
	int extra;
	std::vector<int> symbols;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	Item() : reductionId{-1} {}
	explicit Item(int reductionId) : reductionId{reductionId} {}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	inline int iobserved() const {
		return 0;
	}
	
	inline int imissing() const {
		return observed;
	}
	
	inline int iextra() const {
		return observed + missing;
	}
	
public:
	inline int& pobserved(int i){
		return symbols[iobserved() + i];
	}
	
	inline int& pmissing(int i){
		return symbols[imissing() + i];
	}
	
	inline int& pextra(int i){
		return symbols[iextra() + i];
	}
	
public:
	inline const int& pobserved(int i) const {
		return symbols[iobserved() + i];
	}
	
	inline const int& pmissing(int i) const {
		return symbols[imissing() + i];
	}
	
	inline const int& pextra(int i) const {
		return symbols[iextra() + i];
	}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	inline int size() const {
		return symbols.size();
	}
	
	inline bool empty() const {
		return observed == 0;
	}
	
	inline bool done() const {
		return missing == 0;
	}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	bool equivalent(const Item& other) const;
	
public:
	void set(const Reduction& r, const Map_SymbolToId& symbol_to_id);
	
// ----------------------------------- [ Operators ] ---------------------------------------- //
public:
	inline int& operator[](int i){
		return symbols[i];
	}
	
	inline const int& operator[](int i) const {
		return symbols[i];
	}
	
// ------------------------------------------------------------------------------------------ //
};




class CSR::State {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	int id;
	std::string name;
	
public:
	std::vector<Item> items;
	std::vector<Item> const* emptyItems = nullptr;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	State() : id{-1} {}
	explicit State(int id) : id{id} {}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	void setDefaultName();
	void sort();
	bool equivalent(const State& other) const;
	
public:
	/**
	 * @brief Clone and evolve state according to a given symbol.
	 *        An item is cloned when the symbol is equal to the first missing symbol of the item:
	 *         observed count is increased; missing count is decreased.
	 *        An item is also cloned when the item has no missing symbols:
	 *         symbol is appended as an extra symbol.
	 *        Items from the empty set are evolved according to the same rules.
	 * @param base State to clone.
	 * @param symbol Symbol to affect item cloning.
	 */
	void evolve(const State& base, int symbol);
	
// ------------------------------------------------------------------------------------------ //
};




class CSR::Graph {
// ------------------------------------[ Variables ] ---------------------------------------- //
private:
	Map_SymbolToId symbol_to_id;
	Map_IdToSymbol id_to_symbol;
	
private:
	std::vector<Item> emptySet;
	std::vector<State> states;
	std::deque<int> evolutionQueue;
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	void create(const std::vector<Reduction>& v);
	
private:
	void createMap(const std::vector<Reduction>& v);
	
	/**
	 * @brief Evolve state using all missing symbols from the item list.
	 *        Only non-existing states are accepted.
	 *        ID's of new states are appended to `evolutionQueue`.
	 * @param stateIndex Index of the evolving state from the `states` list.
	 */
	void evolve(int stateIndex);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	static void createEmptySet(const std::vector<Reduction>& v, const Map_SymbolToId& map, std::vector<Item>& out);
	
// ------------------------------------------------------------------------------------------ //
};




class CSR::GraphException : public CSR::CSRException {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	int i = -1;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	using CSRException::CSRException;
	
	GraphException(int i, const char* msg) : CSRException(msg), i{i} {}
	
// ------------------------------------------------------------------------------------------ //
};
