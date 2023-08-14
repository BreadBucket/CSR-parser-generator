#include "Graph.hpp"
#include <deque>
#include <unordered_set>

#include "Symbol.hpp"
#include "Reduction.hpp"
#include "CSRException.hpp"


using namespace std;
using namespace csr;


// ----------------------------------- [ Functions ] ---------------------------------------- //


// DEBUG
#include <iostream>
// #include "ANSI.h"
// #define STR(obj)	str(obj)
// #define CSTR(obj)	str(obj).c_str()
// #define PRINTF(...) printf(__VA_ARGS__)
#define PRINTF(...)


// // DEBUG
// static string str(Symbol* symbol){
// 	if (symbol != nullptr)
// 		return symbol->name;
// 	else
// 		return "null";
// }


// // DEBUG
// static string str(const Item& item){
// 	string s = {};

// 	auto add = [&](int i, int n){
// 		if (n <= 0){
// 			s += "ϵ";
// 		} else {
// 			while (n > 0){
// 				s += str(item.symbols[i]);
// 				s += " ";
// 				i++;
// 				n--;
// 			}
// 			s.pop_back();
// 		}
// 	};
	
// 	add(item.iobserved(), item.observed);
// 	s += " • ";
// 	add(item.imissing(), item.missing);
// 	s += " + ";
// 	add(item.iextra(), item.extra);
	
// 	return s;
// }
	

// // DEBUG
// static string str(const State& state){
// 	string s = "S" + to_string(state.id) + ":\n";

// 	for (const Item& item : state.items){
// 		s += "  ";
// 		s += str(item);
// 		s += "\n";
// 	}
	
// 	if (state.emptyItems != nullptr){
// 		for (const Item& item : *state.emptyItems){
// 			s += "  [";
// 			s += str(item);
// 			s += "]\n";
// 		}
// 	}
	
// 	if (s.size() > 0 && s.back() == '\n')
// 		s.pop_back();
// 	return s;
// }
	
	
// // DEBUG
// static string str(const Connection& v){
// 	string s = {};

// 	if (v.from != nullptr)
// 		s += "S" + to_string(v.from->id);
// 	else
// 		s += "(null)";

// 	s += " --[";
// 	s += str(v.symbol);
// 	s += "]--> ";
	
// 	if (v.to != nullptr)
// 		s += "S" + to_string(v.to->id);
// 	else if (v.reductionItem != nullptr)
// 		// s += str(v.reductionItem->reduction);
// 		s += "REDUCTION ITEM";
// 	else
// 		s += "(null)";
	
// 	return s;
// }


// // DEBUG
// static string str(const vector<Connection*>& v){
// 	string s = {};

// 	for (const Connection* c : v){
// 		s += str(*c);
// 		s += '\n';
// 	}

// 	if (s.size() > 0 && s.back() == '\n')
// 		s.pop_back();
// 	return s;
// }


// ----------------------------------- [ Structures ] --------------------------------------- //


struct Graph::InternalData {
	deque<State*> evolutionQueue;		// Unevolved state references from `states`.
	vector<Symbol*> evolutionSymbols;
};


// ----------------------------------- [ Functions ] ---------------------------------------- //


constexpr bool equals(const Symbol* lhs, const Symbol* rhs){
	if (lhs == nullptr || rhs == nullptr)
		return (lhs == rhs);
	else
		return (lhs->id == rhs->id);
}


bool equals(const vector<shared_ptr<Symbol>>& lhs, const vector<Symbol*>& rhs){
	if (lhs.size() != rhs.size())
		return false;
	
	for (int i = 0 ; i < lhs.size() ; i++){
		if (!equals(lhs[i].get(), rhs[i]))
			return false; 
	}
	
	return true;
}


template <>
struct std::equal_to<Symbol*> {
	constexpr bool operator()(const Symbol* lhs, const Symbol* rhs) const noexcept {
		if (lhs == rhs)
			return true;
		else if (lhs != nullptr && rhs != nullptr)
			return (lhs->id == rhs->id);
		else
			return false;
	}
};


template <>
struct std::hash<csr::Symbol*> {
	constexpr int operator()(const Symbol* p) const noexcept {
		if (p == nullptr)
			return 0;
		else
			return p->id;
	}
};


// ----------------------------------- [ Functions ] ---------------------------------------- //


static void buildEmptySet(const vector<shared_ptr<Reduction>>& reductions, vector<Item>& emptySet){
	emptySet.clear();
	
	for (const shared_ptr<Reduction>& r : reductions){
		if (r == nullptr){
			continue;
		}
		
		// Check if equivalent item exists
		bool distinct = true;
		for (const Item& item : emptySet){
			if (equals(r->left, item.symbols)){
				distinct = false;
				break;
			}
		}
		
		// Create item for the empty set
		if (distinct){
			Item& item = emptySet.emplace_back();
			
			item.symbols.reserve(r->left.size());
			for (const shared_ptr<Symbol>& p : r->left){
				item.symbols.push_back(p.get());
			}
			
			item.reduction = r.get();
			item.observed = 0;
			item.missing = r->left.size();
			item.extra = 0;
		}
		
	}
	
}


static void getAllSymbols(const vector<Item>& items, vector<Symbol*>& symbols){
	unordered_set<Symbol*> set = {};
	symbols.reserve(set.size() * 2);
	set.reserve(set.size() * 2);
	
	for (const Item& item : items){
	for (Symbol* sym : item.symbols){
		if (sym != nullptr)
			set.emplace(sym);
	}}
	
	symbols.assign(set.begin(), set.end());
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Graph::build(const vector<shared_ptr<Reduction>>& v){
	clear();
	data = new InternalData();
	
	buildEmptySet(v, *emptySet);
	getAllSymbols(*emptySet, data->evolutionSymbols);
	
	
	State* s0 = states.emplace_back(new State(0));
	s0->emptyItems = emptySet;
	data->evolutionQueue.push_back(s0);
	
	
	while (data->evolutionQueue.size() > 0){
		PRINTF("\n\n");	// DEBUG
		State* s = data->evolutionQueue.front();
		data->evolutionQueue.pop_front();
		evolve(*s);
	}
	
	
	// Release unused memory
	delete data;
}


void Graph::clear(){
	for (auto p : states)
		delete p;
	for (auto p : connections)
		delete p;
	
	states.clear();
	connections.clear();
	emptySet->clear();
	
	delete data;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Graph::evolve(State& base){
	PRINTF(ANSI_RESET "STEP %s\n", CSTR(base));
	
	const vector<Symbol*>& symbols = data->evolutionSymbols;
	State* tempState = nullptr;
	
	for (Symbol* symbol : symbols){
		if (tempState == nullptr)
			tempState = new State();
		
		tempState->evolve(base, symbol);
		
		// Find existing state
		State* existingState = nullptr;
		for (int ii = 0 ; ii < states.size() ; ii++){
			if (tempState->equivalent(*states[ii])){
				existingState = states[ii];
				break;
			}
		}
		
		// DEBUG
		PRINTF(ANSI_YELLOW "%s " ANSI_RESET, CSTR(symbol));
		tempState->id = states.size();
		if (existingState != nullptr){
			tempState->id = existingState->id;
			PRINTF(ANSI_RED "%s\n" ANSI_RESET, CSTR(*tempState));
		}
		
		
		// Create connection to existing state
		if (existingState != nullptr){
			if (existingState->id != 0){
				Connection* c = new Connection(&base, symbol, existingState);
				connections.push_back(c);
				base.connections.push_back(c);
			}
			continue;
		}
		
		// Check for reduction item
		const Item* ri = tempState->getReductionItem();
		if (ri != nullptr){
			// Copy item, state object holding the item object is temporary
			PRINTF("<< " ANSI_PURPLE "%s" ANSI_RESET " >>\n", CSTR(*ri));
			Connection* c = new Connection(&base, symbol, make_unique<Item>(*ri));
			connections.push_back(c);
			base.connections.push_back(c);
			continue;
		}
		
		// State is completely new
		else {
			PRINTF(ANSI_GREEN "%s\n" ANSI_RESET, CSTR(*tempState));
			
			// Register new state
			tempState->id = states.size();
			states.push_back(tempState);
			data->evolutionQueue.push_back(tempState);
			
			// Finalize connection
			Connection* c = new Connection(&base, symbol, tempState);
			connections.push_back(c);
			base.connections.push_back(c);
			
			tempState = nullptr;
		}
		
	};
	
	delete tempState;
}


void State::evolve(const State& base, Symbol* symbol){
	// Reserve space
	items.clear();
	if (base.emptyItems != nullptr)
		items.reserve(base.items.size() + base.emptyItems->size());
	else
		items.reserve(base.items.size());
	
	
	/**
	 * @brief When symbol is accepted, temporary item object is replaced.
	 */
	auto expandItem = [&](const Item& base, Symbol* symbol){
		
		// Accept item and observe 1 symbol
		if (base.missing != 0){
			if (equals(base[base.imissing()], symbol)){
				Item& item = items.emplace_back();
				item = base;
				item.observed++;
				item.missing--;
			}
		}
		
		// Accept item and add extra symbol
		else {
			Item& item = items.emplace_back();
			item.symbols.reserve(base.symbols.size() + 1);
			item = base;
			item.symbols.push_back(symbol);
			item.extra++;
		}
		
	};
	
	// Handle relevant items
	for (const Item& baseItem : base.items){
		expandItem(baseItem, symbol);
	}
	
	// Handle empty set
	emptyItems = base.emptyItems;
	if (base.emptyItems != nullptr){
		for (const Item& baseItem : *base.emptyItems)
			expandItem(baseItem, symbol);
	}
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


const Item* State::getReductionItem() const {
	if (items.size() <= 0)
		return nullptr;
	
	// Find largest leftmost item
	const Item* ll = &items[0];
	int ll_pos = ll->position();
	int ll_size = ll->reductionSize();
	
	for (int i = 1 ; i < items.size() ; i++){
		int pos = items[i].position();
		int size = items[i].reductionSize();
		
		if (pos < ll_pos || (pos == ll_pos && size > ll_size)){
			ll = &items[i];
			ll_pos = pos;
			ll_size = size;
		}
		
	}
	
	// Item must be completely observed
	if (ll->missing == 0)
		return ll;
	else
		return nullptr;
}


bool State::equivalent(const State& other) const {
	if (items.size() != other.items.size())
		return false;
	
	for (int i = 0 ; i < other.items.size() ; i++){
		if (!containsEquivalent(other.items[i]))
			return false;
	}
	
	return true;
}


bool State::containsEquivalent(const Item& item) const {
	for (int i = 0 ; i < items.size() ; i++){
		if (item.equivalent(items[i]))
			return true;
	}
	return false;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


bool Item::equivalent(const Item& other) const {
	if (reduction != other.reduction)
		return false;
	else if (observed != other.observed || missing != other.missing || extra != other.extra)
		return false;
	else
		return symbols == other.symbols;
}


// ------------------------------------------------------------------------------------------ //