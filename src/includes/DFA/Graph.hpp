#pragma once
#include <string>
#include <vector>
#include <deque>
#include <memory>

#include "Symbol.hpp"
#include "Reduction.hpp"
#include "CSRException.hpp"



namespace csr {
	typedef int StateID;
	class Item;
	class State;
	class Connection;
	class Graph;
}




class csr::Item {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	Reduction* reduction = nullptr;
	
public:
	int observed = 0;	// Amount of observed symbols.
	int missing = 0;	// Amount of missing symbols.
	int extra = 0;		// Amount of extra symbols.
	std::vector<Symbol*> symbols;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	Item() = default;
	explicit Item(Reduction* reduction) : reduction{reduction} {}
	
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
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	inline int reductionSize() const {
		return observed + missing;
	}
	
	inline int position() const {
		return -observed - extra;
	}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	/**
	 * @brief Compare items.
	 * @param other Comparison item.
	 * @return True if observed, missing and extra items match.
	 */
	bool equivalent(const Item& other) const;
	
// ----------------------------------- [ Operators ] ---------------------------------------- //
public:
	inline Symbol*& operator[](int i){
		return symbols[i];
	}
	
	inline Symbol* operator[](int i) const {
		return symbols[i];
	}
	
// ------------------------------------------------------------------------------------------ //
};




class csr::State {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	StateID id = StateID(-1);
	std::vector<Item> items;
	std::vector<Item> const* emptyItems = nullptr;
	std::vector<Connection*> connections;
	std::string cname;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	State() = default;
	explicit State(StateID id) : id{id} {}
	
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
	
	/**
	 * @brief Check if the list of items already contains an equivalent item.
	 * @param item Item to find.
	 * @return True if the state already contains an equivalent item.
	 */
	bool containsEquivalent(const Item& item) const;
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	/**
	 * @brief Check if the state contains a reduction item.
	 *        A reduction item is the longest item that is fully observed and has no other non-fully observed rivals.
	 * @return Item* Pointer to the reduction item within the state, otherwise null.
	 */
	const Item* getReductionItem() const;
	
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
	void evolve(const State& base, Symbol* symbol);
	
// ------------------------------------------------------------------------------------------ //
};




class csr::Connection {
// ------------------------------------[ Properties ] --------------------------------------- //
public:
	Symbol* symbol = nullptr;
	State* from = nullptr;
	State* to = nullptr;
	Reduction* reductionItem = nullptr;
	
// ---------------------------------- [ Constructors ] -------------------------------------- //
public:
	Connection() = default;
	
	Connection(State* from, Symbol* symbol) :
		symbol{symbol}, from{from} {}
	
	Connection(State* from, Symbol* symbol, State* to) :
		symbol{symbol}, from{from}, to{to} {}
	
	Connection(State* from, Symbol* symbol, Reduction* reductionItem) :
		symbol{symbol}, from{from}, reductionItem{reductionItem} {}
	
// ------------------------------------------------------------------------------------------ //
};




/**
 * @brief Graph constructed from reductions does not own the reduction objects.
 *        All pointers held by graph elements are considered as a snapshot of references of when
 *         the graph was built. Destroying the original reduction objects invalidates all
 *         reduction pointers held by the graph elements.
 */
class csr::Graph {
// ------------------------------------[ Variables ] ---------------------------------------- //
public:
	std::vector<State*> states;
	std::vector<Connection*> connections;
	std::vector<Item>* emptySet;
	
private:
	std::deque<State*> evolutionQueue;	// Unevolved state references from `states`.
	
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
		delete emptySet;
	}
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
public:
	void build(const std::vector<std::shared_ptr<Reduction>>& reductions);
	
public:
	/**
	 * @brief Evolve state using all missing symbols from the item list.
	 *        Only non-existing states are accepted.
	 *        New states are appended to the `evolutionQueue`.
	 * @param state The evolving state.
	 */
	void evolve(State& state);
	
// ----------------------------------- [ Functions ] ---------------------------------------- //
private:
	void clear();
	
// ------------------------------------------------------------------------------------------ //
};
