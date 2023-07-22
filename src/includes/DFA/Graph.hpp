#pragma once
#include <string>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <memory>

#include "ParsedReduction.hpp"
#include "CSRException.hpp"



namespace csr {
	typedef int SymbolID;
	class Item;
	class ReductionItem;
	class State;
	class Connection;
	class Graph;
	class GraphException;
}


namespace csr {
	typedef std::unordered_map<std::string,SymbolID> Map_SymbolToId;
	typedef std::unordered_map<SymbolID,std::string> Map_IdToSymbol;
}




class csr::Item {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	const ReductionItem* reduction = nullptr;
	
public:
	int observed = 0;	// Amount of observed symbols.
	int missing = 0;	// Amount of missing symbols.
	int extra = 0;		// Amount of extra symbols.
	std::vector<SymbolID> symbols;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	Item() = default;
	explicit Item(const ReductionItem* reduction) : reduction{reduction} {}
	
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
	inline SymbolID& pobserved(int i){
		return symbols[iobserved() + i];
	}
	
	inline SymbolID& pmissing(int i){
		return symbols[imissing() + i];
	}
	
	inline SymbolID& pextra(int i){
		return symbols[iextra() + i];
	}
	
public:
	inline const SymbolID& pobserved(int i) const {
		return symbols[iobserved() + i];
	}
	
	inline const SymbolID& pmissing(int i) const {
		return symbols[imissing() + i];
	}
	
	inline const SymbolID& pextra(int i) const {
		return symbols[iextra() + i];
	}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	inline int size() const {
		return symbols.size();
	}
	
	inline int reductionSize() const {
		return observed + missing;
	}
	
	inline bool empty() const {
		return observed == 0;
	}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	/**
	 * @brief Compare items.
	 * @param other Comparison item.
	 * @return True if observed, missing and extra items match.
	 */
	bool equivalent(const Item& other) const;
	
public:
	/**
	 * @brief Convert symbols from reduction into their respective ID's
	 *         and insert them into the missing symbol list.
	 *        Clear observed and extra symbol list.
	 * @param r Reduction containing the symbols on the left side.
	 * @param symbol_to_id Enum map to convert symbol names to their ID's.
	 */
	void set(const ParsedReduction& r, const Map_SymbolToId& symbol_to_id);
	
// ----------------------------------- [ Operators ] ---------------------------------------- //
public:
	inline SymbolID& operator[](int i){
		return symbols[i];
	}
	
	inline const SymbolID& operator[](int i) const {
		return symbols[i];
	}
	
// ------------------------------------------------------------------------------------------ //
};




class csr::ReductionItem {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	int id = -1;
	std::vector<SymbolID> left;
	std::vector<SymbolID> right;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	ReductionItem() = default;
	explicit ReductionItem(int id) : id{id} {};
	
// ------------------------------------------------------------------------------------------ //	
};




class csr::State {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	int id = -1;
	std::vector<Item> items;
	std::vector<Item> const* emptyItems = nullptr;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	State() = default;
	explicit State(int id) : id{id} {}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	/**
	 * @brief Compare states.
	 *        States are equivalent if they have the same amount of relevant items and
	 *         contain all of the equivalent relevant items.
	 * @param other Comparing state.
	 * @return True if state is equivalent to `other`.
	 */
	bool equivalent(const State& other) const;
	bool containsEquivalent(const Item& item) const;
	void sort();
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
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
	void evolve(const State& base, SymbolID symbol);
	
	/**
	 * @brief Check if the state contains a reduction item.
	 *        A reduction item is the longest item that is fully observed and has no other non-fully observed rivals.
	 * @return Item* Pointer to the reduction item within the state, otherwise null.
	 */
	const Item* getReductionItem() const;
	
// ------------------------------------------------------------------------------------------ //
};




class csr::Connection {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	SymbolID symbol = SymbolID(-1);
	const State* from = nullptr;
	const State* to = nullptr;
	const ReductionItem* reductionItem = nullptr;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	Connection() = default;
	
	Connection(const State* from, SymbolID symbol, const State* to) :
		symbol{symbol}, from{from}, to{to} {}
	
	Connection(const State* from, SymbolID symbol, const ReductionItem* reductionItem) :
		symbol{symbol}, from{from}, reductionItem{reductionItem} {}
	
// ------------------------------------------------------------------------------------------ //
};




class csr::Graph {
// ------------------------------------[ Variables ] ---------------------------------------- //
private:
	Map_SymbolToId symbol_to_id;
	Map_IdToSymbol id_to_symbol;
	
private:
	std::vector<State*> states;
	std::vector<Connection*> connections;
	std::vector<ReductionItem*> reductionItems;
	std::vector<Item>* emptySet;
	
private:
	std::unordered_set<SymbolID> emptySetEvolutionSymbols;
	std::deque<State*> evolutionQueue;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	Graph(){
		emptySet = new std::vector<Item>();
	}
	
	~Graph(){
		for (auto p : states)
			delete p;
		for (auto p : connections)
			delete p;
		for (auto p : reductionItems)
			delete p;
		delete emptySet;
	}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	void build(const std::vector<ParsedReduction>& v);
	void clear();
	
private:
	void createEnum(const std::vector<ParsedReduction>& v);
	void createReductionItems(const std::vector<ParsedReduction>& v);
	void createEmptySet();
	
	/**
	 * @brief Evolve state using all missing symbols from the item list.
	 *        Only non-existing states are accepted.
	 *        New states are appended to the `evolutionQueue`.
	 * @param state The evolving state.
	 */
	void evolve(const State& state);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	static void getEvolutionSymbols(const std::vector<Item>& v, std::unordered_set<SymbolID>& out_set);
	
// ------------------------------------------------------------------------------------------ //
};




class csr::GraphException : public csr::CSRException {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	int i = -1;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	using CSRException::CSRException;
	GraphException(int i, const char* msg) : CSRException(msg), i{i} {}
	
// ------------------------------------------------------------------------------------------ //
};
