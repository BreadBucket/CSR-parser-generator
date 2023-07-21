#include "Graph.hpp"

#include <unordered_set>
#include <algorithm>

#include "util/utils.hpp"
#include "util/ANSI.h"	// DEBUG


using namespace std;
using namespace CSR;


// ----------------------------------- [ Functions ] ---------------------------------------- //


// DEBUG
string str(int id, const Map_IdToSymbol& id_to_symbol){
	auto p = id_to_symbol.find(id);
	if (p != id_to_symbol.end())
		return p->second;
	else
		return to_string(id);
}



// DEBUG
string str(const Item& item, const Map_IdToSymbol& id_to_symbol){
	string s = {};
	
	auto add = [&](int i, int n){
		if (n <= 0){
			s += "ϵ";
		} else {
			while (n > 0){
				int symbol = item.symbols[i];
				auto p = id_to_symbol.find(symbol);
				
				if (p != id_to_symbol.end())
					s += p->second;
				else
					s += to_string(symbol);
				
				s += " ";
				i++;
				n--;
			}
			s.pop_back();
		}
	};
	
	add(item.iobserved(), item.observed);
	s += " • ";
	add(item.imissing(), item.missing);
	s += " + ";
	add(item.iextra(), item.extra);
	
	return s;
}


// DEBUG
string str(const State& state, const Map_IdToSymbol& id_to_symbol){
	string s = state.name + ":\n";
	
	for (const Item& item : state.items){
		s += "  ";
		s += str(item, id_to_symbol);
		s += "\n";
	}
	
	if (state.emptyItems != nullptr){
		for (const Item& item : *state.emptyItems){
			s += "  [";
			s += str(item, id_to_symbol);
			s += "]\n";
		}
	}
	
	s.pop_back();
	return s;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Graph::create(const vector<Reduction>& v){
	// Validate
	int a, b;
	if (!Reduction::distinct(v, &a, &b))
		throw DuplicateReduction(b, a, "Duplicate left side of reduction.");
	
	createMap(v);
	createEmptySet(v, symbol_to_id, emptySet);
	
	evolutionQueue.clear();
	states.clear();
	states.reserve(64);
	
	State& s0 = states.emplace_back(0);
	s0.emptyItems = &emptySet;
	s0.setDefaultName();
	
	
	
	
	evolve(0);
	// while (evolutionQueue.size() > 0){
	// 	printf("\n\n");
		
		
	// 	int x = evolutionQueue.front();
	// 	evolutionQueue.pop_front();
	// 	evolve(x);
		
	// 	break;
	// }
	
	
}


void Graph::createMap(const std::vector<Reduction>& v){
	symbol_to_id.clear();
	id_to_symbol.clear();
	
	int id = 1;
	auto add = [&](const vector<Symbol>& v){
		for (const Symbol& s : v){
			auto pair = symbol_to_id.try_emplace(s.name, id);
			
			// If inserted
			if (get<1>(pair)){
				id_to_symbol[id] = s.name;
				id++;
			}
			
		}
	};
	
	for (const Reduction& r : v){
		add(r.left);
		add(r.right);
	}
	
}


void Graph::createEmptySet(const vector<Reduction>& v, const Map_SymbolToId& map, vector<Item>& out){
	out.clear();
	out.reserve(v.size());
	
	for (int i = 0 ; i < v.size() ; i++){
		Item& item = out.emplace_back();
		item.set(v[i], map);
		item.reductionId = i;
	}
	
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Graph::evolve(int x){
	if (!VALID_INDEX(x, states.size()))
		return;
	
	printf("STEP " ANSI_CYAN "%s\n" ANSI_RESET, str(states[x], id_to_symbol).c_str());
	
	
	// Get unique symbols
	unordered_set<int> symbols = {};
	{
		const State& base = states[x];
		
		for (const Item& item : base.items){
			if (item.missing > 0)
				symbols.emplace(item.pmissing(0));
		}
		
		if (base.emptyItems != nullptr){
			for (const Item& item : *base.emptyItems){
				if (item.missing > 0)
					symbols.emplace(item.pmissing(0));
			}
		}
	
		if (symbols.size() <= 0){
			return;
		}
		
	}
	
	
	
	State* state = &states.emplace_back(states.size());
	state->setDefaultName();
	
	for (int symbol : symbols){
		state->evolve(states[x], symbol);
		
		// Find existing state
		bool exists = false;
		for (int ii = 0 ; ii < states.size()-1 ; ii++){
			exists |= state->equivalent(states[ii]);
			if (exists)
				break;
		}
		
		
		printf(ANSI_YELLOW "%s " ANSI_RED, str(symbol, id_to_symbol).c_str());
		if (!exists) printf(ANSI_GREEN);
		printf("%s\n" ANSI_RESET, str(*state, id_to_symbol).c_str());
		
		// New state confirmed
		if (!exists){
			evolutionQueue.push_back(state->id);
			state = &states.emplace_back(states.size());
			state->setDefaultName();
		}
		
	}
	
	
	states.pop_back();
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void State::evolve(const State& base, int symbol){
	// Reserve space
	items.clear();
	if (base.emptyItems != nullptr)
		items.reserve(base.items.size() + base.emptyItems->size());
	else
		items.reserve(base.items.size());
	
	
	// Temporary item object
	Item* item = &items.emplace_back();
	
	/**
	 * @brief When symbol is accepted, temporary item object is replaced.
	 */
	auto expandItem = [&](const Item& base, int symbol){
		
		// Accept missing symbol
		if (base.missing != 0){
			if (base.pmissing(0) == symbol){
				*item = base;
				item->observed++;
				item->missing--;
				item = &items.emplace_back();
			}
		}
		
		// Expand extra symbols
		else {
			item->symbols.reserve(base.symbols.size() + 1);
			*item = base;
			item->symbols.push_back(symbol);
			item->extra++;
			item = &items.emplace_back();
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
	
	
	// Delete temporary item object
	items.pop_back();
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void State::setDefaultName(){
	name.clear();
	name.push_back('S');
	name += to_string(id);
}


void State::sort(){
	std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) -> bool {
		if (a.reductionId < b.reductionId)
			return true;
		else if (a.reductionId == b.reductionId){
			if (a.observed > b.observed)
				return true;
		}
		return false;
	});
}


bool State::equivalent(const State& other) const {
	if (items.size() != other.items.size())
		return false;
	
	for (int i = 0 ; i < items.size() ; i++){
		if (items[i].reductionId != other.items[i].reductionId)
			return false;
		else if (!items[i].equivalent(other.items[i]))
			return false;
	}
	
	return true;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Item::set(const Reduction& r, const Map_SymbolToId& map){
	observed = 0;
	missing = 0;
	extra = 0;
	
	symbols.clear();
	symbols.reserve(r.left.size());
	
	for (const Symbol& s : r.left){
		auto p = map.find(s.name);
		if (p != map.end()){
			symbols.push_back(p->second);
			missing++;
		}
	}
	
}


bool Item::equivalent(const Item& other) const {
	if (observed != other.observed || missing != other.missing || extra != other.extra)
		return false;
	else
		return symbols == other.symbols;
}


// ------------------------------------------------------------------------------------------ //